#pragma once
#include<qopenglfunctions_4_5_core.h>
#include<glm.hpp>
#include<string>
#include<vector>
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
    void draw(QOpenGLShaderProgram* shader);
private:
    std::string path;
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<std::shared_ptr<Mesh::Texture>> texture_loaded;

    void processNode(aiNode* node,const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<std::shared_ptr<Mesh::Texture>> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};
