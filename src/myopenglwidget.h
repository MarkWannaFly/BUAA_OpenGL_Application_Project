#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include "model.h"
#include "camera.h"
#include "objectpose.h"

class MyOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

protected:  // 回调函数覆写
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public:
    explicit MyOpenGLWidget(QWidget *parent = nullptr);

    // my functions

    /**
      * @brief  初始化配置OpenGL的Program，包括顶点着色器和片段着色器
      * @author Xiang Guo
      * @param  none
      * @retval none
      */
    void InitProgram(void);

    /**
      * @brief  初始化纹理，包括读取图片和配置纹理参数，将纹理绑定到OpenGL的纹理单元上
      * @author Xiang Guo
      * @param  filename: 图片路径
      * @retval none
      */
    void InitTexture(const char *pic_file);

    /**
      * @brief  初始化地形，包括读取地形数据和配置地形及纹理的顶点，将地形数据绑定到OpenGL缓冲区
      * @author Xiang Guo
      * @param  dem_file: 地形数据路径
      * @retval none
      */
    void InitTerrain(const char *dem_file);

    /**
      * @brief  执行地形绘制
      * @author Xiang Guo
      * @param  none
      * @retval none
      */
    void DrawTerrain(void);

    /**
      * @brief  初始化照片，包括读取照片数据和配置照片的顶点，将照片数据绑定到OpenGL缓冲区
      * @author Xiang Guo
      * @param  pic_file: 照片路径
      * @param  left_top: 照片左上角坐标
      * @param  right_bottom: 照片右下角坐标
      * @retval none
      */
    void InitPhoto(const char *pic_file, QVector2D left_top, QVector2D right_bottom);
    void DrawPhoto(void);

public:
    GLint mouse_x, mouse_y; // position of mouse;

    GLfloat nearclip;   // near clip distance
    GLfloat farclip;    // far clip distance

    GLfloat rx, ry, rz;  // reference point

    int nx_terrain, ny_terrain; // the resolution of terrain
    QOpenGLTexture *p_texture_terrain;     // terrain texture
    QOpenGLTexture *p_my_photo;

    GLuint vao_terrain, vbo_vercoord, vbo_texcoord, ebo_index; // VAO, VBO and EBO of terrain
    QOpenGLShaderProgram shader_program_terrain;
    QOpenGLShaderProgram shader_program_plane;
    GLuint vao_photo, vbo_vercoord_photo, vbo_texcoord_photo, ebo_index_photo; // VAO, VBO and EBO of photo

    QTimer *refresh_timer;

    Model *m_model;
    Camera *p_camera;

    ObjectPose *p_plane_pose_0;
    ObjectPose *p_plane_pose_1;
    ObjectPose *p_plane_pose_array[2];

    bool w_pressed = false, a_pressed = false, s_pressed = false, d_pressed = false, q_pressed = false, e_pressed = false;
    bool up_pressed = false, down_pressed = false, left_pressed = false, right_pressed = false, z_pressed = false, c_pressed = false;

    uint plane_select = 0;

public slots:
    void OnRefreshTimeout(void);

};

#endif // MYOPENGLWIDGET_H
