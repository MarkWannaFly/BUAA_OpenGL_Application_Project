#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include <QOpenGLTexture>

using std::vector;

class Model
{

public:
    Model(QOpenGLFunctions_4_5_Core *glfuns, const char *path);
    void Draw(QOpenGLShaderProgram &shader);

public:
    // model data
    QOpenGLFunctions_4_5_Core *p_gl_funs;
    vector<Mesh> meshes;
    vector<Texture> textures;
    string directory;

private:
    void LoadModel(string path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    GLuint TextureFromFile(const char *path, const string &directory);

};

#endif // MODEL_H
