#include "assimpLoader.h"
#include "../shader/shaderLoader.h"
#include "../utils/fileUtils.h"
#include "../utils/logUtils.h"
#include <opencv2/opencv.hpp>
#include <utility>
#include <memory>

// 如果没有 std::make_unique，可以手动实现它
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

/**
 * 类的构造函数，加载着色器并获取其中变量的位置
 */
AssimpLoader::AssimpLoader() {
    importerPtr = std::make_unique<Assimp::Importer>();
    scene = nullptr;
    isObjectLoaded = false;

    std::string vertexShader = "shaders/modelTextured.vsh";
    std::string fragmentShader = "shaders/modelTextured.fsh";
    shaderProgramID = LoadShaders(vertexShader, fragmentShader);
    vertexAttribute = GetAttributeLocation(shaderProgramID, "vertexPosition");
    vertexUVAttribute = GetAttributeLocation(shaderProgramID, "vertexUV");
    mvpLocation = GetUniformLocation(shaderProgramID, "mvpMat");
    textureSamplerLocation = GetUniformLocation(shaderProgramID, "textureSampler");

    CheckGLError("AssimpLoader::AssimpLoader");
}

AssimpLoader::~AssimpLoader() {
    Delete3DModel();
}

void AssimpLoader::BuildFaceBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    if (meshInfo.numberOfFaces) {
        std::vector<unsigned int> faceArray(mesh->mNumFaces * 3);
        unsigned int faceIndex = 0;

        for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
            const aiFace* face = &mesh->mFaces[t];
            memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
            faceIndex += 3;
        }

        glGenBuffers(1, &meshInfo.faceBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.faceBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray.data(), GL_STATIC_DRAW);
    }
}

void AssimpLoader::BuildVertexBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    if (mesh->HasPositions()) {
        glGenBuffers(1, &meshInfo.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
    }
}

void AssimpLoader::BuildTextureCoordBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    if (mesh->HasTextureCoords(0)) {
        std::vector<float> textureCoords(2 * mesh->mNumVertices);
        for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
            textureCoords[k * 2] = mesh->mTextureCoords[0][k].x;
            textureCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;
        }

        glGenBuffers(1, &meshInfo.textureCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.textureCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->mNumVertices, textureCoords.data(), GL_STATIC_DRAW);
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
            SetTextureParameters();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureImage.cols, textureImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage.data);

            CheckGLError("AssimpLoader::LoadTextureImages");
        } else {
            MyLOGE("Couldn't load texture %s", textureEntry.first.c_str());
            delete[] textureGLNames;
            return;
        }
        i++;
    }
}

void AssimpLoader::SetTextureParameters() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
bool AssimpLoader::Load3DModel(const std::string& modelFilename) {
    MyLOGI("Scene will be imported now");

    scene = importerPtr->ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Quality);

    if (!scene) {
        std::string errorString = importerPtr->GetErrorString();
        MyLOGE("场景导入失败: %s", errorString.c_str());
        return false;
    }

    MyLOGI("导入 %s 成功.", modelFilename.c_str());
    LoadTexturesToGL(modelFilename);
    MyLOGI("导入纹理成功");

    BuildGLBuffers();
    MyLOGI("加载顶点与纹理坐标成功");

    isObjectLoaded = true;
    return true;
}

/**
 * 清除与3D模型相关的内存
 */
void AssimpLoader::Delete3DModel() {
    if (isObjectLoaded) {
        modelMeshes.clear();
        textureNameMap.clear();
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
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(mvpMat));
}

void AssimpLoader::SetActiveTextureUnit() {
    glActiveTexture(GL_TEXTURE0);
}

void AssimpLoader::SetTextureSamplerUniform() {
    glUniform1i(textureSamplerLocation, 0);
}

void AssimpLoader::RenderMeshes() {
    for (const auto& meshInfo : modelMeshes) {
        if (meshInfo.textureIndex) {
            glBindTexture(GL_TEXTURE_2D, meshInfo.textureIndex);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.faceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
        glEnableVertexAttribArray(vertexAttribute);
        glVertexAttribPointer(vertexAttribute, 3, GL_FLOAT, 0, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.textureCoordBuffer);
        glEnableVertexAttribArray(vertexUVAttribute);
        glVertexAttribPointer(vertexUVAttribute, 2, GL_FLOAT, 0, 0, 0);

        glDrawElements(GL_TRIANGLES, meshInfo.numberOfFaces * 3, GL_UNSIGNED_INT, 0);

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
