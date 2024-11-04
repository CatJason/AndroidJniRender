#include <AssimpLoader.h>
#include <shader/shaderLoader.h>
#include <utils/fileUtils.h>
#include <utils/logUtils.h>
#include <utility>
#include <memory>
#include <android/bitmap.h>


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
    MyLOGI("AssimpLoader::AssimpLoader - 构造函数初始化");
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
    MyLOGI("AssimpLoader::~AssimpLoader - 析构函数调用");
    Delete3DModel();
}

/**
 * 构建面缓冲区
 * @param mesh aiMesh 对象
 * @param meshInfo 存储面信息的结构体
 */
void AssimpLoader::BuildFaceBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    MyLOGI("AssimpLoader::BuildFaceBuffer - 构建面缓冲区");
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

/**
 * 构建顶点缓冲区
 * @param mesh aiMesh 对象
 * @param meshInfo 存储顶点信息的结构体
 */
void AssimpLoader::BuildVertexBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    MyLOGI("AssimpLoader::BuildVertexBuffer - 构建顶点缓冲区");
    if (mesh->HasPositions()) {
        glGenBuffers(1, &meshInfo.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
    }
}

/**
 * 构建纹理坐标缓冲区
 * @param mesh aiMesh 对象
 * @param meshInfo 存储纹理坐标信息的结构体
 */
void AssimpLoader::BuildTextureCoordBuffer(const aiMesh* mesh, MeshInfo& meshInfo) {
    MyLOGI("AssimpLoader::BuildTextureCoordBuffer - 构建纹理坐标缓冲区");
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

/**
 * 绑定缓冲区
 */
void AssimpLoader::BindBuffers() {
    MyLOGI("AssimpLoader::BindBuffers - 绑定缓冲区");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * 构建纹理索引
 * @param mesh aiMesh 对象
 * @param meshInfo 存储纹理索引信息的结构体
 */
void AssimpLoader::BuildTextureIndex(const aiMesh* mesh, MeshInfo& meshInfo) {
    MyLOGI("AssimpLoader::BuildTextureIndex - 构建纹理索引");
    aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];
    aiString texturePath;
    if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)) {
        unsigned int textureId = textureNameMap[texturePath.data];
        meshInfo.textureIndex = textureId;
    } else {
        meshInfo.textureIndex = 0;
    }
}

/**
 * 构建OpenGL缓冲区
 */
void AssimpLoader::BuildGLBuffers() {
    MyLOGI("AssimpLoader::BuildGLBuffers - 构建GL缓冲区");
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
    MyLOGI("AssimpLoader::ExtractTextureFilenames - 提取纹理文件名");
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
    MyLOGI("AssimpLoader::GenerateTextureGLNames - 生成纹理GL名称");
    textureGLNames = new GLuint[numTextures];
    glGenTextures(numTextures, textureGLNames);
}

void AssimpLoader::LoadTextureImages(JNIEnv *env, const std::string& modelDirectoryName, GLuint* textureGLNames) {
    MyLOGI("AssimpLoader::LoadTextureImages - 加载纹理图像");
    int i = 0;

    for (auto& textureEntry : textureNameMap) {
        std::string textureFullPath = modelDirectoryName + "/" + textureEntry.first;
        textureEntry.second = textureGLNames[i];

        MyLOGI("Loading texture %s", textureFullPath.c_str());

        jobject bitmap = LoadBitmapFromFile(env, textureFullPath);
        if (bitmap) {
            LoadTextureFromBitmap(env, bitmap, textureGLNames[i]);
            env->DeleteLocalRef(bitmap);
        } else {
            MyLOGE("Couldn't load texture %s", textureEntry.first.c_str());
            delete[] textureGLNames;
            return;
        }
        i++;
    }
}

/**
 * 从文件加载位图
 */
jobject AssimpLoader::LoadBitmapFromFile(JNIEnv *env, const std::string& filePath) {
    jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");
    jmethodID decodeFileMethod = env->GetStaticMethodID(bitmapFactoryClass, "decodeFile", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");

    jstring jFilePath = env->NewStringUTF(filePath.c_str());
    jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass, decodeFileMethod, jFilePath);
    env->DeleteLocalRef(jFilePath);

    return bitmap;
}

/**
 * 从位图加载纹理
 */
void AssimpLoader::LoadTextureFromBitmap(JNIEnv *env, jobject bitmap, GLuint textureGLName) {
    MyLOGI("AssimpLoader::LoadTextureFromBitmap - 成功加载纹理");

    AndroidBitmapInfo info;
    void* pixels;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        MyLOGE("Failed to get bitmap info");
        return;
    }

    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        MyLOGE("Failed to lock pixels");
        return;
    }

    // 创建纹理
    glBindTexture(GL_TEXTURE_2D, textureGLName);
    SetTextureParameters();

    // 翻转 Y 轴
    unsigned char* flippedPixels = new unsigned char[info.width * info.height * 4]; // 假设每个像素4个字节
    unsigned char* originalPixels = static_cast<unsigned char*>(pixels);
    for (int y = 0; y < info.height; ++y) {
        memcpy(flippedPixels + (info.height - 1 - y) * info.width * 4,
               originalPixels + y * info.width * 4,
               info.width * 4);
    }

    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flippedPixels);

    // 清理
    delete[] flippedPixels;
    AndroidBitmap_unlockPixels(env, bitmap);
    // 检查 OpenGL 错误
    CheckGLError("AssimpLoader::LoadTextureFromBitmap");
}

void AssimpLoader::SetTextureParameters() {
    MyLOGI("AssimpLoader::SetTextureParameters - 设置纹理参数");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
/**
 * 将纹理加载到OpenGL中
 * @param env JNIEnv 指针
 * @param modelFilename 模型文件名
 * @return 如果成功加载纹理则返回true
 */
bool AssimpLoader::LoadTexturesToGL(JNIEnv *env, std::string modelFilename) {
    MyLOGI("AssimpLoader::LoadTexturesToGL - 将纹理加载到GL中");
    ExtractTextureFilenames();

    int numTextures = static_cast<int>(textureNameMap.size());
    MyLOGI("总纹理数量: %d", numTextures);

    GLuint* textureGLNames = nullptr;
    GenerateTextureGLNames(numTextures, textureGLNames);

    std::string modelDirectoryName = ExtractDirectoryName(std::move(modelFilename));
    LoadTextureImages(env, modelDirectoryName, textureGLNames); // 传递 env 参数

    delete[] textureGLNames;
    return true;
}

/**
 * 加载包含多个网格的通用OBJ模型
 * 假设每个网格关联一个纹理，不处理材质属性（如漫反射、镜面反射等）
 * @param env JNIEnv 指针
 * @param modelFilename 模型文件名
 * @return 如果成功加载模型则返回true
 */
bool AssimpLoader::Load3DModel(JNIEnv *env, const std::string& modelFilename) {
    MyLOGI("AssimpLoader::Load3DModel - 加载3D模型: %s", modelFilename.c_str());
    MyLOGI("场景将被导入");

    scene = importerPtr->ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Quality);

    if (!scene) {
        std::string errorString = importerPtr->GetErrorString();
        MyLOGE("场景导入失败: %s", errorString.c_str());
        return false;
    }

    MyLOGI("导入 %s 成功.", modelFilename.c_str());

    // 传递 env 参数
    LoadTexturesToGL(env, modelFilename);
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
    MyLOGI("AssimpLoader::Delete3DModel - 删除3D模型");
    if (isObjectLoaded) {
        modelMeshes.clear();
        textureNameMap.clear();
        isObjectLoaded = false;
    }
}

/**
 * 清除OpenGL缓冲区
 */
void AssimpLoader::ClearBuffers() {
    // MyLOGI("AssimpLoader::ClearBuffers - 清除缓冲区");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * 使用着色器程序
 */
void AssimpLoader::UseShaderProgram() {
    // MyLOGI("AssimpLoader::UseShaderProgram - 使用着色器程序");
    glUseProgram(shaderProgramID);
}

/**
 * 设置模型视图投影矩阵
 * @param mvpMat 指向MVP矩阵的指针
 */
void AssimpLoader::SetModelViewProjectionMatrix(const glm::mat4* mvpMat) {
    // MyLOGI("AssimpLoader::SetModelViewProjectionMatrix - 设置MVP矩阵");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(mvpMat));
}

/**
 * 设置活动纹理单元
 */
void AssimpLoader::SetActiveTextureUnit() {
    // MyLOGI("AssimpLoader::SetActiveTextureUnit - 设置活动纹理单元");
    glActiveTexture(GL_TEXTURE0);
}

/**
 * 设置纹理采样器uniform
 */
void AssimpLoader::SetTextureSamplerUniform() {
    // MyLOGI("AssimpLoader::SetTextureSamplerUniform - 设置纹理采样器");
    glUniform1i(textureSamplerLocation, 0);
}

/**
 * 渲染模型中的所有网格
 */
void AssimpLoader::RenderMeshes() {
    // MyLOGI("AssimpLoader::RenderMeshes - 渲染网格");
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

/**
 * 渲染3D模型
 * @param mvpMat 指向MVP矩阵的指针
 */
void AssimpLoader::Render3DModel(glm::mat4* mvpMat) {
    // MyLOGI("AssimpLoader::Render3DModel - 渲染3D模型");
    if (!isObjectLoaded) {
        MyLOGI("AssimpLoader::Render3DModel - 对象未加载");
        return;
    }
    UseShaderProgram();
    SetModelViewProjectionMatrix(mvpMat);
    SetActiveTextureUnit();
    SetTextureSamplerUniform();
    RenderMeshes();

    CheckGLError("AssimpLoader::Render3DModel()");
}