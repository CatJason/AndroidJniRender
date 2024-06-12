#ifndef ASSIMP_LOADER_H
#define ASSIMP_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <GLES2/gl2.h>
#include <memory>
#include "../../../thirdpartCpp/glm/detail/type_mat.hpp"

struct MeshInfo {
    GLuint vertexBuffer;
    GLuint textureCoordBuffer;
    GLuint faceBuffer;
    unsigned int numberOfFaces;
    unsigned int textureIndex;
};

class AssimpLoader {
public:
    AssimpLoader();
    ~AssimpLoader();

    bool Load3DModel(const std::string& modelFilename);
    void Delete3DModel();
    void Render3DModel(glm::mat4* mvpMat);

private:
    void BuildFaceBuffer(const aiMesh* mesh, MeshInfo& meshInfo);
    void BuildVertexBuffer(const aiMesh* mesh, MeshInfo& meshInfo);
    void BuildTextureCoordBuffer(const aiMesh* mesh, MeshInfo& meshInfo);
    void BindBuffers();
    void BuildTextureIndex(const aiMesh* mesh, MeshInfo& meshInfo);
    void BuildGLBuffers();
    void ExtractTextureFilenames();
    void GenerateTextureGLNames(int numTextures, GLuint*& textureGLNames);
    void LoadTextureImages(const std::string& modelDirectoryName, GLuint* textureGLNames);
    void SetTextureParameters();
    bool LoadTexturesToGL(std::string modelFilename);
    void ClearBuffers();
    void UseShaderProgram();
    void SetModelViewProjectionMatrix(const glm::mat4* mvpMat);
    void SetActiveTextureUnit();
    void SetTextureSamplerUniform();
    void RenderMeshes();

    std::unique_ptr<Assimp::Importer> importerPtr; // 使用智能指针管理 Assimp::Importer
    const aiScene* scene;
    bool isObjectLoaded;

    std::unordered_map<std::string, unsigned int> textureNameMap;
    std::vector<MeshInfo> modelMeshes;

    GLuint shaderProgramID;
    GLint vertexAttribute;
    GLint vertexUVAttribute;
    GLint mvpLocation;
    GLint textureSamplerLocation;
};

#endif // ASSIMP_LOADER_H
