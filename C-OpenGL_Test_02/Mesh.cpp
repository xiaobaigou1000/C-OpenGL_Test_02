#include "Mesh.h"

Mesh::Mesh(Mesh&& from)noexcept
{
    vertices = std::move(from.vertices);
    indices = std::move(from.indices);
    textures = std::move(from.textures);
    VAO = from.VAO;
    VBO = from.VBO;
    EBO = from.EBO;

    from.VAO = from.VBO = from.EBO = 0;
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<std::shared_ptr<Texture>>& textures)
    :vertices(vertices), indices(indices), textures(textures), VAO(), VBO(), EBO()
{

}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<std::shared_ptr<Texture>>&& textures)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)), VAO(), VBO(), EBO()
{
}

void Mesh::init()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

    if (VBO == 0)
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    }

    if (EBO == 0)
    {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        indicesNum = indices.size();
    }

    if (VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }
    glBindVertexArray(0);
}

void Mesh::bind()
{
    glBindVertexArray(VAO);
}

void Mesh::draw()
{
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setShaderVariables(QOpenGLShaderProgram* shader)
{
    int diffuseNum = 1;
    int specularNum = 1;
    int texIndex = 0;
    for (auto& tex : textures)
    {
        std::string number;
        glActiveTexture(GL_TEXTURE0 + texIndex);
        tex->bind();
        TextureType currentTexType = tex->type;
        std::string texName;
        if (currentTexType == TextureType::Diffuse)
        {
            texName = "texture_diffuse";
        }
        else
        {
            texName = "texture_specular";
        }
        if (currentTexType == TextureType::Diffuse)
            number = std::to_string(diffuseNum++);
        else if (currentTexType == TextureType::Specular)
            number = std::to_string(specularNum++);
        glUniform1i(shader->uniformLocation(QString::fromStdString(texName + number)), texIndex);
        ++texIndex;
    }
    glActiveTexture(GL_TEXTURE0);
}
