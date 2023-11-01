#include "../shader/shaderLoader.h"
#include "../utils/fileUtils.h"
#include "assimpLoader.h"
#include <opencv2/opencv.hpp>
#include <utility>

/**
 * 类的构造函数，加载着色器并获取其中变量的位置
 */
AssimpLoader::AssimpLoader() {
    importerPtr = new Assimp::Importer;  // 创建一个Assimp的导入器实例
    scene = nullptr;  // 初始化场景指针为空
    isObjectLoaded = false;  // 将对象加载状态设置为未加载

    // 着色器相关设置 -- 加载、属性和统一变量位置
    std::string vertexShader = "shaders/modelTextured.vsh";  // 顶点着色器文件路径
    std::string fragmentShader = "shaders/modelTextured.fsh";  // 片段着色器文件路径
    shaderProgramID = LoadShaders(vertexShader, fragmentShader);  // 加载着色器程序并返回程序ID
    vertexAttribute = GetAttributeLocation(shaderProgramID, "vertexPosition");  // 获取顶点属性位置
    vertexUVAttribute = GetAttributeLocation(shaderProgramID, "vertexUV");  // 获取顶点UV属性位置
    mvpLocation = GetUniformLocation(shaderProgramID, "mvpMat");  // 获取MVP矩阵的统一变量位置
    textureSamplerLocation = GetUniformLocation(shaderProgramID, "textureSampler");  // 获取纹理采样器的统一变量位置

    CheckGLError("AssimpLoader::AssimpLoader");  // 检查OpenGL错误
}

/**
 * 类的析构函数，删除Assimp导入器指针并从GL中移除3D模型
 */
AssimpLoader::~AssimpLoader() {
    Delete3DModel();  // 调用Delete3DModel函数删除3D模型

    if (importerPtr) {  // 检查importerPtr是否为非空指针
        delete importerPtr;  // 删除importerPtr指针指向的对象
        importerPtr = nullptr;  // 将importerPtr设置为nullptr
    }

    scene = nullptr;  // 将scene指针设置为nullptr，与importerPtr一起删除
}


/**
 * 生成顶点位置、纹理坐标、面的缓冲区并加载数据
 */
void AssimpLoader::BuildFaceBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    if (meshInfo.numberOfFaces) {
        auto* faceArray = new unsigned int[mesh->mNumFaces * 3];
        unsigned int faceIndex = 0;

        for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
            const aiFace* face = &mesh->mFaces[t];
            memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
            faceIndex += 3;
        }

        glGenBuffers(1, &meshInfo.faceBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.faceBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3,
                     faceArray, GL_STATIC_DRAW);

        delete[] faceArray;
    }
}

void AssimpLoader::BuildVertexBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    if (mesh->HasPositions()) {
        glGenBuffers(1, &meshInfo.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices,
                     GL_STATIC_DRAW);
    }
}

void AssimpLoader::BuildTextureCoordBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    if (mesh->HasTextureCoords(0)) {
        auto* textureCoords = new float[2 * mesh->mNumVertices];
        for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
            textureCoords[k * 2] = mesh->mTextureCoords[0][k].x;
            textureCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;
        }

        glGenBuffers(1, &meshInfo.textureCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.textureCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->mNumVertices, textureCoords,
                     GL_STATIC_DRAW);

        delete[] textureCoords;
    }
}

void AssimpLoader::BindBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AssimpLoader::BuildTextureIndex(const aiMesh* mesh, MeshInfo& meshInfo) {
    aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];
    aiString texturePath;
    if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)) {
        unsigned int textureId = textureNameMap[texturePath.data];
        meshInfo.textureIndex = textureId;
    } else {
        meshInfo.textureIndex = 0;
    }
}

void AssimpLoader::BuildGLBuffers() {
    for (unsigned int n = 0; n < scene->mNumMeshes; ++n) {
        const aiMesh* mesh = scene->mMeshes[n];
        MeshInfo newMeshInfo{};
        newMeshInfo.numberOfFaces = mesh->mNumFaces;

        BuildFaceBuffer(mesh, newMeshInfo);
        BuildVertexBuffer(mesh, newMeshInfo);
        BuildTextureCoordBuffer(mesh, newMeshInfo);

        BindBuffers();

        BuildTextureIndex(mesh, newMeshInfo);

        modelMeshes.push_back(newMeshInfo);
    }
}


/**
 * 读取与所有材质相关的纹理并将图像加载到GL中
 */
void AssimpLoader::ExtractTextureFilenames() {
    textureNameMap.clear();

    for (unsigned int m = 0; m < scene->mNumMaterials; ++m) {
        int textureIndex = 0;
        aiString textureFilename;
        while (scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, textureIndex, &textureFilename) == AI_SUCCESS) {
            textureNameMap[textureFilename.data] = 0;
            textureIndex++;
        }
    }
}

void AssimpLoader::GenerateTextureGLNames(int numTextures, GLuint*& textureGLNames) {
    textureGLNames = new GLuint[numTextures];
    glGenTextures(numTextures, textureGLNames);
}

void AssimpLoader::LoadTextureImages(const std::string& modelDirectoryName, GLuint* textureGLNames) {
    int i = 0;
    for (auto& textureEntry : textureNameMap) {
        std::string textureFullPath = modelDirectoryName + "/" + textureEntry.first;
        textureEntry.second = textureGLNames[i];

        MyLOGI("Loading texture %s", textureFullPath.c_str());
        cv::Mat textureImage = cv::imread(textureFullPath);
        if (!textureImage.empty()) {
            cv::cvtColor(textureImage, textureImage, cv::COLOR_BGR2RGB);
            cv::flip(textureImage, textureImage, 0);

            glBindTexture(GL_TEXTURE_2D, textureGLNames[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureImage.cols, textureImage.rows, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, textureImage.data);

            CheckGLError("AssimpLoader::loadGLTexGen");
        } else {
            MyLOGE("Couldn't load texture %s", textureEntry.first.c_str());
            delete[] textureGLNames;
            return;
        }
        i++;
    }
}

bool AssimpLoader::LoadTexturesToGL(std::string modelFilename) {
    ExtractTextureFilenames();

    int numTextures = static_cast<int>(textureNameMap.size());
    MyLOGI("Total number of textures is %d", numTextures);

    GLuint* textureGLNames = nullptr;
    GenerateTextureGLNames(numTextures, textureGLNames);

    std::string modelDirectoryName = ExtractDirectoryName(std::move(modelFilename));

    LoadTextureImages(modelDirectoryName, textureGLNames);

    delete[] textureGLNames;
    return true;
}

/**
 * 加载包含多个网格的通用OBJ -- 假设每个网格关联一个纹理
 * 不处理材质属性（如漫反射、镜面反射等）
 */
bool AssimpLoader::Load3DModel(const std::string &modelFilename) {
    MyLOGI("Scene will be imported now");
    // 使用Assimp的导入器对象读取模型文件并将其导入到场景中
    /*
     scene = importerPtr->ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Quality);
     使用导入器读取模型文件并解析场景数据
     参数解释：
       - 第一个参数：模型文件名
       - 第二个参数：导入过程的预设选项，这里使用 aiProcessPreset_TargetRealtime_Quality，
                  表示使用适用于实时渲染的高质量预设选项
    返回值：
       - 解析成功时，将场景数据存储在变量 scene 中，可以通过 scene 访问模型的各个部分，如顶点、材质等
     */
    scene = importerPtr->ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Quality);

    // 检查导入是否失败
    // 如果场景数据为空，表示导入失败
    if (!scene) {
        // 获取导入器的错误信息字符串
        std::string errorString = importerPtr->GetErrorString();
        // 打印错误信息
        MyLOGE("场景导入失败: %s", errorString.c_str());
        return false;
    }
    MyLOGI("导入 %s 成功.", modelFilename.c_str());
    // 加载与模型相关的所有纹理到 OpenGL 中
    LoadTexturesToGL(modelFilename);
    MyLOGI("导入纹理成功");

    // 生成OpenGL缓冲区并填充顶点和纹理坐标数据
    BuildGLBuffers();
    MyLOGI("加载顶点与纹理坐标成功");

    // 标记模型加载完成
    isObjectLoaded = true;
    return true;
}

/**
 * 清除与3D模型相关的内存
 */
void AssimpLoader::Delete3DModel() {
    if (isObjectLoaded) {
        // 清除modelMeshes相关的内容
        modelMeshes.clear();

        // 清除textureNameMap相关的内容
        textureNameMap.clear();

        // 标记模型未加载
        isObjectLoaded = false;
    }
}

/**
 * 渲染3D模型，通过渲染对象中的每个网格实现
*/
void AssimpLoader::ClearBuffers() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AssimpLoader::UseShaderProgram() {
    glUseProgram(shaderProgramID);
}

void AssimpLoader::SetModelViewProjectionMatrix(const glm::mat4* mvpMat) {
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)mvpMat);
}

void AssimpLoader::SetActiveTextureUnit() {
    glActiveTexture(GL_TEXTURE0);
}

void AssimpLoader::SetTextureSamplerUniform() {
    glUniform1i(textureSamplerLocation, 0);
}

void AssimpLoader::RenderMeshes() {
    unsigned int numberOfLoadedMeshes = modelMeshes.size();

    for (unsigned int n = 0; n < numberOfLoadedMeshes; ++n) {
        if (modelMeshes[n].textureIndex) {
            glBindTexture(GL_TEXTURE_2D, modelMeshes[n].textureIndex);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelMeshes[n].faceBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, modelMeshes[n].vertexBuffer);
        glEnableVertexAttribArray(vertexAttribute);
        glVertexAttribPointer(vertexAttribute, 3, GL_FLOAT, 0, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, modelMeshes[n].textureCoordBuffer);
        glEnableVertexAttribArray(vertexUVAttribute);
        glVertexAttribPointer(vertexUVAttribute, 2, GL_FLOAT, 0, 0, 0);

        glDrawElements(GL_TRIANGLES, modelMeshes[n].numberOfFaces * 3, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void AssimpLoader::Render3DModel(glm::mat4* mvpMat) {
    if (!isObjectLoaded) {
        return;
    }

    ClearBuffers();
    UseShaderProgram();
    SetModelViewProjectionMatrix(mvpMat);
    SetActiveTextureUnit();
    SetTextureSamplerUniform();
    RenderMeshes();

    CheckGLError("AssimpLoader::Render3DModel()");
}

