#pragma once
#include<qopenglfunctions_4_5_core.h>
#include<qopengltexture.h>
#include<qopenglshaderprogram.h>
#include<glm.hpp>
#include<vector>
#include<string>
#include<memory>

class Mesh :public QOpenGLFunctions_4_5_Core
{
public:
    Mesh(Mesh&& from)noexcept;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    enum class TextureType
    {
        Diffuse, Specular
    };

    struct Texture
    {
        QOpenGLTexture* tex = nullptr;
        TextureType type;
        std::string path;

        void init()
        {
            if (tex)
                return;
            tex = new QOpenGLTexture(QImage(QString::fromStdString(path)).mirrored());
        }

        TextureType bind()
        {
            tex->bind();
            return type;
        }
    };

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<std::shared_ptr<Texture>>& textures);
    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<std::shared_ptr<Texture>>&& textures);

    void init();
    void bind();
    void draw();
    void setShaderVariables(QOpenGLShaderProgram* shader);

    unsigned int indicesNum;
private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;
};
