#ifndef OBJECTPOSE_H
#define OBJECTPOSE_H

#include <QMatrix4x4>

class ObjectPose
{
public:
    // 坐标描述向量
    QVector3D position_vec;
    QVector3D front_vec;
    QVector3D up_vec;
    QVector3D right_vec;
    QVector3D world_up_vec;

    // 初始姿态偏置
    QMatrix4x4 pose_offset_matrix;

public:
    /**
      * @brief  构造函数，初始化位姿属性，包括位置、方向、上向量、右向量、世界向量等信息
      * @author Xiang Guo
      * @param  position_vec: 位姿位置向量
      * @param  world_up_vec: 世界上向量
      * @param  up_vec: 位姿上向量
      * @param  front_vec: 位姿前向量
      * @retval none
      */
    ObjectPose(QMatrix4x4 pose_offset_matrix, QVector3D position_vec = QVector3D(0.0f, 0.0f, 0.0f), QVector3D world_up_vec = QVector3D(0.0f, 1.0f, 0.0f), QVector3D up_vec = QVector3D(0.0f, 1.0f, 0.0f), QVector3D front_vec = QVector3D(1.0f, 0.0f, 0.0f));

    /**
      * @brief  获得物体位姿变换矩阵
      * @author Xiang Guo
      * @param  none
      * @retval none
      */
    QMatrix4x4 GetModelMatrix(void);

    /**
      * @brief  移动物体位置
      * @author Xiang Guo
      * @param  forward_increment: 前向移动增量
      * @param  right_increment: 右向移动增量
      * @param  up_increment: 上向移动增量
      * @retval none
      */
    void Move(float forward_increment, float right_increment, float up_increment);

    /**
      * @brief  旋转物体姿态，注意：三个角度必须是微小变动，才能根据微转动的可交换性连乘
      * @author Xiang Guo
      * @param  pitch_increment: 俯仰角增量
      * @param  yaw_increment: 偏航角增量
      * @param  roll_increment: 翻滚角增量
      * @retval none
      */
    void Rotate(float pitch_increment, float yaw_increment, float roll_increment);
    
};

#endif // OBJECTPOSE_H
