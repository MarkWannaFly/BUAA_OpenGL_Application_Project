#include "objectpose.h"

ObjectPose::ObjectPose(QMatrix4x4 pose_offset_matrix, QVector3D position_vec, QVector3D world_up_vec, QVector3D up_vec, QVector3D front_vec)
{
    this->pose_offset_matrix = pose_offset_matrix;
    this->position_vec = position_vec;
    this->world_up_vec = world_up_vec;
    this->up_vec = up_vec;
    this->front_vec = front_vec;
    this->right_vec = QVector3D::crossProduct(this->front_vec, this->up_vec);
}

QMatrix4x4 ObjectPose::GetModelMatrix()
{
    QMatrix4x4 model_matrix;
    QQuaternion quaternion1 = QQuaternion::fromDirection(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    QQuaternion quaternion2 = QQuaternion::fromDirection(front_vec, up_vec);
    QQuaternion rotation_quaternion = quaternion2 * quaternion1.conjugated();
    model_matrix.translate(position_vec);
    model_matrix.rotate(rotation_quaternion);
    model_matrix = model_matrix * pose_offset_matrix;
    
    return model_matrix;
}

void ObjectPose::Move(float forward_increment, float right_increment, float up_increment)
{
    position_vec += front_vec.normalized() * forward_increment;
    position_vec += right_vec.normalized() * right_increment;
    position_vec += up_vec.normalized() * up_increment;
}

void ObjectPose::Rotate(float pitch_increment, float yaw_increment, float roll_increment)
{
    QMatrix4x4 rotation_matrix;
    rotation_matrix.rotate(pitch_increment, right_vec);
    rotation_matrix.rotate(yaw_increment, up_vec);
    rotation_matrix.rotate(roll_increment, front_vec);
    
    up_vec = rotation_matrix * up_vec;
    front_vec = rotation_matrix * front_vec;
    right_vec = QVector3D::crossProduct(front_vec, up_vec);
}

