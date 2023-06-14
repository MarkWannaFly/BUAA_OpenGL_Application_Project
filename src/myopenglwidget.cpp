#include "myopenglwidget.h"
#include <iostream>
#include <QtMath>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    // 初始化刷新绘图定时器
    refresh_timer = new QTimer(this);
    connect(refresh_timer, SIGNAL(timeout()), this, SLOT(OnRefreshTimeout()));
    setFocusPolicy(Qt::StrongFocus);
}

void MyOpenGLWidget::initializeGL()
{
    // 初始化OpenGL函数指针
    initializeOpenGLFunctions();

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 初始化操作
    InitProgram();
    InitTexture("./resources/terrain.png");
    InitTerrain("./resources/grid.dem");
    InitPhoto("./resources/photo.png", QVector2D(0.6f, -0.6f), QVector2D(1.0f, -1.0f));

    p_camera = new Camera(nearclip, farclip, 30.0, QVector3D(0, 0, farclip / 5));
    m_model = new Model(QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>(), "./resources/plane.stl");
    QMatrix4x4 plane_pose_offset_matrix;
    plane_pose_offset_matrix.rotate(-90.0f, QVector3D(1.0f, 0.0f, 0.0f));
    p_plane_pose_0 = new ObjectPose(plane_pose_offset_matrix, QVector3D(0.0f, 10000.0f, 0.0f));
    plane_pose_offset_matrix.setToIdentity();
    plane_pose_offset_matrix.rotate(-90.0f, QVector3D(1.0f, 0.0f, 0.0f));
    p_plane_pose_1 = new ObjectPose(plane_pose_offset_matrix, QVector3D(0.0f, 10000.0f, 10000.0f));
    p_plane_pose_array[0] = p_plane_pose_0;
    p_plane_pose_array[1] = p_plane_pose_1;
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void MyOpenGLWidget::paintGL()
{

    // 绘制地形
    QMatrix4x4 projection;
    projection.perspective(p_camera->field_of_view_degree, (float)width()/height(), nearclip, farclip);
    QMatrix4x4 view = p_camera->GetViewMatrix();
    QMatrix4x4 terrain_model;
    terrain_model.rotate(-90.0f, QVector3D(1.0f, 0.0f, 0.0f));
    terrain_model.translate(QVector3D(-rx, -ry, -rz));

    shader_program_terrain.bind();
    shader_program_terrain.setUniformValue("projection", projection);
    shader_program_terrain.setUniformValue("view", view);
    shader_program_terrain.setUniformValue("model", terrain_model);

    p_texture_terrain->bind(0);
    DrawTerrain();
    p_texture_terrain->release();
    shader_program_terrain.release();

    // 绘制飞机
    QMatrix4x4 plane_model;
    plane_model.scale(100.0f);
    plane_model = p_plane_pose_0->GetModelMatrix() * plane_model;

    shader_program_plane.bind();

    shader_program_plane.setUniformValue("projection", projection);
    shader_program_plane.setUniformValue("view", view);
    shader_program_plane.setUniformValue("model", plane_model);

    shader_program_plane.setUniformValue("material.ambient", QVector3D(0.1f, 0.1f, 0.1f));
    shader_program_plane.setUniformValue("material.diffuse", QVector3D(0.6f, 0.6f, 0.6f));
    shader_program_plane.setUniformValue("material.specular", QVector3D(1.0f, 1.0f, 1.0f));
    shader_program_plane.setUniformValue("material.color", QVector3D(0.5f, 0.5f, 0.5f));
    
    shader_program_plane.setUniformValue("light.position", QVector3D(farclip, farclip, 0));
    shader_program_plane.setUniformValue("light.color", QVector3D(1.0f, 1.0f, 1.0f));

    shader_program_plane.setUniformValue("view_pos", p_camera->position_vec);
    
    m_model->Draw(shader_program_plane);

    plane_model.setToIdentity();
    plane_model.scale(100.0f);
    plane_model = p_plane_pose_1->GetModelMatrix() * plane_model;
    shader_program_plane.setUniformValue("model", plane_model);
    shader_program_plane.setUniformValue("material.color", QVector3D(0.1f, 0.2f, 0.6f));
    shader_program_plane.setUniformValue("material.diffuse", QVector3D(0.3f, 0.3f, 0.3f));
    m_model->Draw(shader_program_plane);

    shader_program_plane.release();

    // 绘制照片
    QMatrix4x4 photo_projection;
    photo_projection.ortho(0.0f, 1.0f, 0.0f, height() / width(), -1.0f, 1.0f);
    QMatrix4x4 photo_view;
    QMatrix4x4 photo_model;

    shader_program_terrain.bind();
    shader_program_terrain.setUniformValue("projection", photo_projection);
    shader_program_terrain.setUniformValue("view", photo_view);
    shader_program_terrain.setUniformValue("model", photo_model);

    p_my_photo->bind(0);
    DrawPhoto();
    p_my_photo->release();
    shader_program_terrain.release();

    refresh_timer->start(1000.0f / 60.0f);
}

void MyOpenGLWidget::InitProgram(void)
{
    shader_program_terrain.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrain.vert");
    shader_program_terrain.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrain.frag");
    bool success = shader_program_terrain.link();
    if (!success)
    {
        qDebug() << "ERR: " << shader_program_terrain.log();
        exit(-1);
    }

    shader_program_plane.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/plane.vert");
    shader_program_plane.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/plane.frag");
    success = shader_program_plane.link();
    if (!success)
    {
        qDebug() << "ERR: " << shader_program_plane.log();
        exit(-1);
    }
}

void MyOpenGLWidget::InitTexture(const char *pic_file)
{
    p_texture_terrain = new QOpenGLTexture(QImage(pic_file).mirrored());
}

void MyOpenGLWidget::InitTerrain(const char *dem_file)
{
    float x_origin, y_origin; // the origin of terrain
    float angle;              // azimuth angle of terrain
    float dx, dy;             // the size of grid
    int nx, ny;               // the resolution of terrain
    float *p_hight;           // data of terrain

    // read terrain
    FILE *fp = fopen(dem_file, "r");
    fscanf(fp, "%f %f %f %f %f %d %d", &x_origin, &y_origin, &angle, &dx, &dy, &nx, &ny);
    // pHight = (float *)calloc(nx*ny, sizeof(float));
    p_hight = new float[nx * ny];
    for (int j = 0; j < ny; j++)
    {
        for (int i = 0; i < nx; i++)
        {
            fscanf(fp, "%f", &p_hight[(ny - 1 - j) * nx + i]);
        }
    }
    fclose(fp);

    // 初始化坐标位置参数
    rx = (nx - 1) * dx / 2;
    ry = (ny - 1) * dy / 2;
    rz = 0.0;
    nearclip = 0.1f * (rx + ry);
    farclip = 20.0f * (rx + ry);

    // 初始化地形和纹理定点参数
    float *p_vercoord = new float[nx * ny * 3];
    float *p_texcoord = new float[nx * ny * 2];
    GLuint *p_index = new GLuint[(nx - 1) * (ny - 1) * 6];

    for (int j = 0; j < ny; j++)
        for (int i = 0; i < nx; i++)
        {
            p_vercoord[(j * nx + i) * 3 + 0] = i * dx;              // x coord
            p_vercoord[(j * nx + i) * 3 + 1] = j * dy;              // y coord
            p_vercoord[(j * nx + i) * 3 + 2] = p_hight[j * nx + i]; // z coord

            p_texcoord[(j * nx + i) * 2 + 0] = i * 1.0f / (nx - 1); // s coord
            p_texcoord[(j * nx + i) * 2 + 1] = j * 1.0f / (ny - 1); // t coord
        }
    for (int j = 0; j < ny - 1; j++)
        for (int i = 0; i < nx - 1; i++)
        {
            p_index[(j * (nx - 1) + i) * 6 + 0] = j * nx + i;           // 0
            p_index[(j * (nx - 1) + i) * 6 + 1] = j * nx + i + 1;       // 1
            p_index[(j * (nx - 1) + i) * 6 + 2] = (j + 1) * nx + i + 1; // 2

            p_index[(j * (nx - 1) + i) * 6 + 3] = j * nx + i;           // 0
            p_index[(j * (nx - 1) + i) * 6 + 4] = (j + 1) * nx + i + 1; // 2
            p_index[(j * (nx - 1) + i) * 6 + 5] = (j + 1) * nx + i;     // 3
        }

    // 生成VAO
    glGenVertexArrays(1, &vao_terrain);
    glBindVertexArray(vao_terrain);

    // 生成VBO
    glGenBuffers(1, &vbo_vercoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vercoord);
    glBufferData(GL_ARRAY_BUFFER, nx * ny * 3 * sizeof(float), p_vercoord, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
    glBufferData(GL_ARRAY_BUFFER, nx * ny * 2 * sizeof(float), p_texcoord, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (nx - 1) * (ny - 1) * 6 * sizeof(GLuint), p_index, GL_STATIC_DRAW);

    // 释放内存
    delete[] p_hight;
    delete[] p_vercoord;
    delete[] p_texcoord;
    delete[] p_index;

    // 赋值
    nx_terrain = nx;
    ny_terrain = ny;

    // 解绑VAO
    glBindVertexArray(0);
}

void MyOpenGLWidget::DrawTerrain(void)
{
    // 绑定VAO
    glBindVertexArray(vao_terrain);

    // 绘制地形
    glDrawElements(GL_TRIANGLES, (nx_terrain - 1) * (ny_terrain - 1) * 6, GL_UNSIGNED_INT, nullptr);

    // 解绑VAO
    glBindVertexArray(0);
}

void MyOpenGLWidget::InitPhoto(const char *pic_file, QVector2D left_top, QVector2D right_bottom)
{
    p_my_photo = new QOpenGLTexture(QImage(pic_file));
    float x_left_top = left_top.x();
    float y_left_top = left_top.y();
    float x_right_bottom = right_bottom.x();
    float y_right_bottom = right_bottom.y();

    float ver_coord[] = {
        x_left_top, y_left_top, 0.0f,
        x_right_bottom, y_left_top, 0.0f,
        x_right_bottom, y_right_bottom, 0.0f,
        x_left_top, y_right_bottom, 0.0f
    };

    float tex_coord[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.f, 1.0f,
        0.0f, 1.0f
    };

    GLuint index[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &vao_photo);
    glBindVertexArray(vao_photo);

    glGenBuffers(1, &vbo_vercoord_photo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vercoord_photo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ver_coord), ver_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo_texcoord_photo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_photo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo_index_photo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_index_photo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void MyOpenGLWidget::DrawPhoto(void)
{
    glBindVertexArray(vao_photo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void MyOpenGLWidget::OnRefreshTimeout(void)
{
    ObjectPose *temp_p_plane_pose = p_plane_pose_array[plane_select];
    // 移动飞机
    if (w_pressed)
    {
        temp_p_plane_pose->Move(300, 0, 0);
    }
    if (s_pressed)
    {
        temp_p_plane_pose->Move(-300, 0, 0);
    }
    if (a_pressed)
    {
        temp_p_plane_pose->Move(0, -100, 0);
    }
    if (d_pressed)
    {
        temp_p_plane_pose->Move(0, 100, 0);
    }
    if (q_pressed)
    {
        temp_p_plane_pose->Move(0, 0, 100);
    }
    if (e_pressed)
    {
        temp_p_plane_pose->Move(0, 0, -100);
    }
    // 转动飞机
    if (up_pressed)
    {
        temp_p_plane_pose->Rotate(-1, 0, 0);
    }
    if (down_pressed)
    {
        temp_p_plane_pose->Rotate(1, 0, 0);
    }
    if (left_pressed)
    {
        temp_p_plane_pose->Rotate(0, 1, 0);
    }
    if (right_pressed)
    {
        temp_p_plane_pose->Rotate(0, -1, 0);
    }
    if (z_pressed)
    {
        temp_p_plane_pose->Rotate(0, 0, -1);
    }
    if (c_pressed)
    {
        temp_p_plane_pose->Rotate(0, 0, 1);
    }
    // qDebug() << p_plane_pose_0->position_vec;
    update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    mouse_x = event->x();
    mouse_y = event->y();
    QWidget::mousePressEvent(event);
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    GLint dx, dy;
    dx = event->x() - mouse_x;
    dy = event->y() - mouse_y;
    mouse_x = event->x();
    mouse_y = event->y();

    if (event->buttons() & Qt::LeftButton)
    {
        p_camera->RotateArroundOrigin(-dy, -dx);
    }
    else if (event->buttons() & Qt::RightButton)
    {
        p_camera->Move(FORWARD, dx + dy);
    }

    update();
    QWidget::mouseMoveEvent(event);
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    p_camera->Zoom(event->angleDelta().y());
    
    update();
    QWidget::wheelEvent(event);
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
    {
        w_pressed = true;
    }
    else if (event->key() == Qt::Key_A)
    {
        a_pressed = true;
    }
    else if (event->key() == Qt::Key_S)
    {
        s_pressed = true;
    }
    else if (event->key() == Qt::Key_D)
    {
        d_pressed = true;
    }
    else if (event->key() == Qt::Key_Q)
    {
        q_pressed = true;
    }
    else if (event->key() == Qt::Key_E)
    {
        e_pressed = true;
    }
    else if (event->key() == Qt::Key_Up)
    {
        up_pressed = true;
    }
    else if (event->key() == Qt::Key_Down)
    {
        down_pressed = true;
    }
    else if (event->key() == Qt::Key_Left)
    {
        left_pressed = true;
    }
    else if (event->key() == Qt::Key_Right)
    {
        right_pressed = true;
    }
    else if (event->key() == Qt::Key_Z)
    {
        z_pressed = true;
    }
    else if (event->key() == Qt::Key_C)
    {
        c_pressed = true;
    }
    else if (event->key() == Qt::Key_1)
    {
        plane_select = 0;
    }
    else if (event->key() == Qt::Key_2)
    {
        plane_select = 1;
    }
    
    QWidget::keyPressEvent(event);
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
    {
        w_pressed = false;
    }
    else if (event->key() == Qt::Key_A)
    {
        a_pressed = false;
    }
    else if (event->key() == Qt::Key_S)
    {
        s_pressed = false;
    }
    else if (event->key() == Qt::Key_D)
    {
        d_pressed = false;
    }
    else if (event->key() == Qt::Key_Q)
    {
        q_pressed = false;
    }
    else if (event->key() == Qt::Key_E)
    {
        e_pressed = false;
    }
    else if (event->key() == Qt::Key_Up)
    {
        up_pressed = false;
    }
    else if (event->key() == Qt::Key_Down)
    {
        down_pressed = false;
    }
    else if (event->key() == Qt::Key_Left)
    {
        left_pressed = false;
    }
    else if (event->key() == Qt::Key_Right)
    {
        right_pressed = false;
    }
    else if (event->key() == Qt::Key_Up)
    {
        up_pressed = false;
    }
    else if (event->key() == Qt::Key_Down)
    {
        down_pressed = false;
    }
    else if (event->key() == Qt::Key_Left)
    {
        left_pressed = false;
    }
    else if (event->key() == Qt::Key_Right)
    {
        right_pressed = false;
    }
    else if (event->key() == Qt::Key_Z)
    {
        z_pressed = false;
    }
    else if (event->key() == Qt::Key_C)
    {
        c_pressed = false;
    }
    QWidget::keyPressEvent(event);
}
