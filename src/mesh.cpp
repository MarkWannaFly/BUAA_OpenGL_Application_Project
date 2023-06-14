#include "mesh.h"

Mesh::Mesh(QOpenGLFunctions_4_5_Core *gl_funs, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    : p_gl_funs(gl_funs)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    Setup();
}

void Mesh::Draw(QOpenGLShaderProgram &shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        p_gl_funs->glActiveTexture(GL_TEXTURE0 + i);
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setUniformValue(("material." + name + number).c_str(), i);
        p_gl_funs->glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    p_gl_funs->glBindVertexArray(VAO);
    p_gl_funs->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    p_gl_funs->glBindVertexArray(0);
}

void Mesh::Setup()
{
    // 创建VAO、VBO和EBO
    p_gl_funs->glGenVertexArrays(1, &VAO);
    p_gl_funs->glGenBuffers(1, &VBO);
    p_gl_funs->glGenBuffers(1, &EBO);

    // 绑定VAO，记录配置
    p_gl_funs->glBindVertexArray(VAO);

    // 绑定VBO，将顶点数据复制到缓冲中，然后传递给OpenGL
    p_gl_funs->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    p_gl_funs->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 绑定EBO，将索引数据复制到缓冲中，然后传递给OpenGL
    p_gl_funs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    p_gl_funs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 设置顶点位置属性指针，告诉OpenGL如何解析顶点数据
    // 顶点位置属性，配置在0号顶点属性上
    p_gl_funs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    p_gl_funs->glEnableVertexAttribArray(0);
    // 顶点法线属性，配置在1号顶点属性上
    p_gl_funs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    p_gl_funs->glEnableVertexAttribArray(1);
    // 顶点纹理坐标属性，配置在2号顶点属性上
    p_gl_funs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    p_gl_funs->glEnableVertexAttribArray(2);

    // 解绑VAO
    p_gl_funs->glBindVertexArray(0);
}
