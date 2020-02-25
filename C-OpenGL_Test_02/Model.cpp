#include "Model.h"
#include<qdebug.h>

Model::Model()
{
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        qFatal((std::string("Error assimp::") + std::string(importer.GetErrorString())).c_str());
        return;
    }
    this->path = path;
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::drawWithoutShaderBinding(QOpenGLShaderProgram* shader)
{
    for (auto& i : meshes)
    {
        i.bind();
        i.setShaderVariables(shader);
        i.draw();
    }
}

void Model::instancedDrawWithoutShaderBinding(QOpenGLShaderProgram* shader, unsigned int instanceNum)
{
    for (auto& i : meshes)
    {
        i.bind();
        i.setShaderVariables(shader);
        i.glDrawElementsInstanced(GL_TRIANGLES, i.indicesNum, GL_UNSIGNED_INT, 0, instanceNum);
    }
}

void Model::setAdditionalVertexAttribute(std::function<void()> func)
{
    for (auto& i : meshes)
    {
        i.bind();
        func();
    }
}

void Model::init()
{
    for (auto& i : meshes)
    {
        i.init();
    }
    for (auto& i : texture_loaded)
    {
        i->init();
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    using glm::vec3;
    using glm::vec2;

    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Mesh::Texture>> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Mesh::Vertex vertex;

        vec3 vertexAndNormalVec;
        vertexAndNormalVec.x = mesh->mVertices[i].x;
        vertexAndNormalVec.y = mesh->mVertices[i].y;
        vertexAndNormalVec.z = mesh->mVertices[i].z;
        vertex.position = vertexAndNormalVec;

        vertexAndNormalVec.x = mesh->mNormals[i].x;
        vertexAndNormalVec.y = mesh->mNormals[i].y;
        vertexAndNormalVec.z = mesh->mNormals[i].z;
        vertex.normal = vertexAndNormalVec;

        if (mesh->HasTextureCoords(0))
        {
            vec2 texCoords;
            texCoords.x = mesh->mTextureCoords[0][i].x;
            texCoords.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = texCoords;
        }
        else
        {
            vertex.texCoords = vec2{ 0.0f, 0.0f };
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<std::shared_ptr<Mesh::Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Mesh::TextureType::Diffuse);
        std::vector<std::shared_ptr<Mesh::Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Mesh::TextureType::Specular);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<std::shared_ptr<Mesh::Texture>> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, Mesh::TextureType texType)
{
    std::vector<std::shared_ptr<Mesh::Texture>> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        std::string path = directory;
        path += '/';
        path += str.C_Str();

        bool skip = false;
        for (auto& i : texture_loaded)
        {
            if (i->path == path)
            {
                textures.push_back(i);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            Mesh::Texture* texture = new Mesh::Texture;
            texture->type = texType;
            texture->path = path;
            std::shared_ptr<Mesh::Texture> toPush{ texture };
            texture_loaded.push_back(toPush);
            textures.push_back(toPush);
        }
    }
    return textures;
}
