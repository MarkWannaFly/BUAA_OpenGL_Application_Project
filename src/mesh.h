/**
  ******************************************************************************
  * @file           : mesh.h
  * @author         : Xiang Guo
  * @date           : 2023/06/08
  * @brief          : 
  *     这个文件包含了网格类的定义，包括网格的顶点、索引和纹理，以及网格的绘制函数
  * 网格类的定义参考了LearnOpenGL的网格类定义，网址：https://learnopengl.com/Model-Loading/Mesh
  * 使用这个网格类，可以方便得管理复杂的模型网格，可以直接从Assimp的模型加载器中加载网格数据
  ******************************************************************************
  * @attention
  *             
  *             
  ******************************************************************************
  */

#ifndef MESH_H
#define MESH_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <string>
#include <vector>

using std::string;
using std::vector;

// 顶点结构体，包括位置、法向量和纹理坐标
struct Vertex {
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
};

// 纹理结构体，包括纹理ID、纹理类型和纹理路径
struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    // 网格数据，包括Vertex、索引以及Texture，眼红vector容器存储
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // 绘制用到的OpenGL对象
    unsigned int VAO, VBO, EBO;

public:
    /**
      * @brief  构造函数，初始化网格数据，包括顶点、索引和纹理，以及OpenGL函数指针，用于绘制网格
      * @author Xiang Guo
      * @param  gl_funs: OpenGL函数指针，将初始化好的OpenGL函数指针传入网格对象，用于绘制网格
      * @param  vertices: 网格顶点数据
      * @param  indices: 网格索引数据
      * @param  textures: 网格纹理数据
      * @retval none
      */
    Mesh(QOpenGLFunctions_4_5_Core *gl_funs, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    /**
      * @brief  绘制网格，使用传入的着色器程序进行绘制，绘制时会绑定网格的纹理，以及设置网格的材质属性，包括漫反射、镜面反射、高光反射和折射率等属性
      * @author Xiang Guo
      * @param  shader: 着色器程序，用于绘制网格
      * @retval none
      */
    void Draw(QOpenGLShaderProgram &shader);

    /**
      * @brief  构建绘制网格所需要的OpenGL对象，包括VAO、VBO和EBO，以及绑定顶点属性指针，用于绘制网格
      * @author Xiang Guo
      * @param  none
      * @retval none
      */
    void Setup(void);

private:
    QOpenGLFunctions_4_5_Core *p_gl_funs;
};

#endif // MESH_H
