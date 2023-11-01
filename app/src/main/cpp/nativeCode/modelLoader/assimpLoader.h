#ifndef ASSIMPLOADER_H
#define ASSIMPLOADER_H

#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../gl/myGLM.h"
#include "../gl/GLManager.h"

// 用于渲染网格的信息
struct MeshInfo {
    GLuint  textureIndex;
    int     numberOfFaces;
    GLuint  faceBuffer;
    GLuint  vertexBuffer;
    GLuint  textureCoordBuffer;
};

class AssimpLoader {

public:
    AssimpLoader();
    ~AssimpLoader();

    void Render3DModel(glm::mat4 *MVP);
    bool Load3DModel(const std::string& modelFilename);
    void Delete3DModel();

private:
    void BuildGLBuffers();
    bool LoadTexturesToGL(std::string modelFilename);

    std::vector<struct MeshInfo> modelMeshes;       // 包含模型中每个网格的结构体
    Assimp::Importer *importerPtr;
    const aiScene* scene;                           // assimp的输出数据结构
    bool isObjectLoaded;

    std::map<std::string, GLuint> textureNameMap;   // (纹理文件名, GL中的纹理名称)

    GLuint  vertexAttribute, vertexUVAttribute;     // 着色器变量的属性
    GLuint  shaderProgramID;
    GLint   mvpLocation, textureSamplerLocation;    // 着色器中MVP的位置
    void BindBuffers();

    void BuildFaceBuffer(const aiMesh *mesh, MeshInfo &meshInfo);

    void BuildVertexBuffer(const aiMesh *mesh, MeshInfo &meshInfo);

    void BuildTextureCoordBuffer(const aiMesh *mesh, MeshInfo &meshInfo);

    void BuildTextureIndex(const aiMesh *mesh, MeshInfo &meshInfo);

    void GenerateTextureGLNames(int numTextures, GLuint *&textureGLNames);

    void ExtractTextureFilenames();

    void LoadTextureImages(const std::string &modelDirectoryName, GLuint *textureGLNames);

    void ClearBuffers();

    void UseShaderProgram();

    void SetModelViewProjectionMatrix(const glm::mat4 *mvpMat);

    void SetActiveTextureUnit();

    void SetTextureSamplerUniform();

    void RenderMeshes();
};

#endif // ASSIMPLOADER_H
