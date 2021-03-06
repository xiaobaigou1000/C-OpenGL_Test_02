#pragma once
#include<qopenglfunctions_4_5_core.h>
#include<glm.hpp>
#include<string>
#include<vector>
#include<functional>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"Mesh.h"


class Model
{
public:
    Model();

    void loadModel(std::string path);
    void init();
    void drawWithoutShaderBinding(QOpenGLShaderProgram* shader);
    void instancedDrawWithoutShaderBinding(QOpenGLShaderProgram* shader, unsigned int instanceNum);
    void setAdditionalVertexAttribute(std::function<void()> func);
private:
    std::string path;
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<std::shared_ptr<Mesh::Texture>> texture_loaded;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<std::shared_ptr<Mesh::Texture>> loadMaterialTextures(aiMaterial* material, aiTextureType type, Mesh::TextureType texType);
};
