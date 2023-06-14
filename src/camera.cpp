#include "camera.h"

Camera::Camera(float nearclip, float farclip, float fov, QVector3D position_vec, QVector3D world_up_vec, float yaw, float pitch)
    : front_vec(QVector3D(0.0f, 0.0f, -1.0f))
{
    // 相机属性初始化
    this->position_vec = position_vec;
    this->world_up_vec = world_up_vec;
    this->yaw_angle_degree = yaw;
    this->pitch_angle_degree = pitch;
    this->near_clip = nearclip;
    this->far_clip = farclip;
    this->field_of_view_degree = fov;
    this->sensitivity_trans = 1000.0f;
    this->sensitivity_rot = 0.5f;
    this->sensitivity_zoom = 0.01f;

    // 相机属性更新
    UpdateCameraVectors();
}

QMatrix4x4 Camera::GetViewMatrix(void)
{
    QMatrix4x4 view_matrix;
    view_matrix.lookAt(position_vec, position_vec + front_vec, up_vec);
    return view_matrix;
}

void Camera::Move(CameraMovement_t direction, float increment, bool constrain_dist)
{
    increment *= sensitivity_trans;
    switch (direction)
    {
    case FORWARD:
        position_vec += front_vec.normalized() * increment;
        break;

    case BACKWARD:
        position_vec -= front_vec.normalized() * increment;
        break;

    case LEFT:
        position_vec -= right_vec.normalized() * increment;
        break;

    case RIGHT:
        position_vec += right_vec.normalized() * increment;
        break;

    case UP:
        position_vec += up_vec.normalized() * increment;
        break;

    case DOWN:
        position_vec -= up_vec.normalized() * increment;
        break;
    
    default:
        break;
    }

    if (constrain_dist)
    {
        if (this->position_vec.length() < near_clip)
            this->position_vec = this->position_vec.normalized() * near_clip;
        if (this->position_vec.length() > far_clip)
            this->position_vec = this->position_vec.normalized() * far_clip;
    }
}

void Camera::Rotate(float pitch_increment, float yaw_increment, bool constrain_pitch)
{
    pitch_increment *= sensitivity_rot;
    yaw_increment *= sensitivity_rot;

    yaw_angle_degree += yaw_increment;
    pitch_angle_degree += pitch_increment;

    // 确保当投球超出边界时，屏幕不会翻转
    if (constrain_pitch)
    {
        if (pitch_angle_degree > 89.0f)
            pitch_angle_degree = 89.0f;
        if (pitch_angle_degree < -89.0f)
            pitch_angle_degree = -89.0f;
    }

    // 使用更新的Euler角度更新前、右和上矢量
    UpdateCameraVectors();
}

void Camera::RotateArroundOrigin(float pitch_increment, float yaw_increment, bool constrain_pitch)
{
    pitch_increment *= sensitivity_rot;
    yaw_increment *= sensitivity_rot;

    QMatrix4x4 rotation_matrix;
    rotation_matrix.rotate(pitch_increment, right_vec);
    rotation_matrix.rotate(yaw_increment, up_vec);
    position_vec = rotation_matrix * position_vec;

    front_vec = -position_vec;
    right_vec = QVector3D::crossProduct(front_vec, world_up_vec);
    right_vec.normalize();
    up_vec = QVector3D::crossProduct(right_vec, front_vec);
    up_vec.normalize();
}

void Camera::Zoom(float fov_increment)
{
    fov_increment *= sensitivity_zoom;
    field_of_view_degree -= fov_increment;
    if (field_of_view_degree < 1.0f)
        field_of_view_degree = 1.0f;
    if (field_of_view_degree > 90.0f)
        field_of_view_degree = 90.0f;
}

void Camera::UpdateCameraVectors(void)
{
    // calculate the new Front vector
    float PI = 3.1415926f;
    QVector3D front;
    front.setX(cos(yaw_angle_degree * PI / 180.0f) * cos(pitch_angle_degree * PI / 180.0f));
    front.setY(sin(pitch_angle_degree * PI / 180.0f));
    front.setZ(sin(yaw_angle_degree * PI / 180.0f) * cos(pitch_angle_degree * PI / 180.0f));
    front.normalize();
    front_vec = front;
    // also re-calculate the Right and Up vector
    right_vec = QVector3D::crossProduct(front_vec, world_up_vec);
    // 标准化向量，因为向上或向下看得越多，向量的长度就越接近0，这会导致移动速度变慢。
    right_vec.normalize();
    up_vec = QVector3D::crossProduct(right_vec, front_vec);
    up_vec.normalize();
}
