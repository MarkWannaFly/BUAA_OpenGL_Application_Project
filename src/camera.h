/**
  ******************************************************************************
  * @file           : camera.h
  * @author         : Xiang Guo
  * @date           : 2023/06/09
  * @brief          : 
  *     抽象的相机类，用于记录相机（观察者）的位置、方向、上向量、右向量、世界向量等信息
  * 参考：https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/
  ******************************************************************************
  * @attention
  *             
  *             
  ******************************************************************************
  */

#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

// 移动方向枚举量
typedef enum
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
} CameraMovement_t;

class Camera
{
public:
    // 相机坐标描述向量
    QVector3D position_vec;
    QVector3D front_vec;
    QVector3D up_vec;
    QVector3D right_vec;
    QVector3D world_up_vec;
    
    // 相机姿态的欧拉角
    float yaw_angle_degree;   // 偏航角
    float pitch_angle_degree; // 俯仰角

    // 相机的近平面和远平面
    float near_clip;
    float far_clip;

    // 相机可视角度
    float field_of_view_degree;

    // 改变相机属性的鼠标灵敏度
    float sensitivity_trans;
    float sensitivity_rot;
    float sensitivity_zoom;

public:
    /**
      * @brief  构造函数，初始化相机属性，包括位置、方向、上向量、右向量、世界向量等信息
      * @author Xiang Guo
      * @param  position_vec: 相机位置向量
      * @param  up_vec: 相机上向量
      * @param  yaw: 偏航角，单位：度
      * @param  pitch: 俯仰角，单位：度
      * @retval none
      */
    Camera(float nearclip, float farclip, float fov, QVector3D position_vec = QVector3D(0.0f, 0.0f, 0.0f), QVector3D world_up_vec = QVector3D(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

    /**
      * @brief  获取当前相机观察矩阵
      * @author Xiang Guo
      * @param  none
      * @retval 相机观察矩阵
      */
    QMatrix4x4 GetViewMatrix(void);

    /**
      * @brief  移动相机位置，根据输入的方向和偏移量，依据移动灵敏度更新相机位置
      * @author Xiang Guo
      * @param  direction: 移动方向
      * @param  increment: 移动偏移量，单位：无
      * @param  constrain_dist: 是否限制相机位置到原点的距离
      * @retval none
      */
    void Move(CameraMovement_t direction, float increment, bool constrain_dist = true);

    /**
      * @brief  旋转相机姿态，根据输入的俯仰角和偏航角增量，依据旋转灵敏度更新相机姿态，可选是否限制俯仰角的范围在[-89, 89]度之间，以防止万向锁问题的发生
      * @author Xiang Guo
      * @param  pitch_increment: 俯仰角增量，单位：无
      * @param  yaw_increment: 偏航角增量，单位：无
      * @param  constrain_pitch: 是否限制俯仰角的范围在[-89, 89]度之间
      * @retval none
      */
    void Rotate(float pitch_increment, float yaw_increment, bool constrain_pitch = true);

    /**
      * @brief  绕原点旋转相机姿态，根据输入的俯仰角和偏航角增量，依据旋转灵敏度更新相机姿态，可选是否限制俯仰角的范围在[-89, 89]度之间，以防止万向锁问题的发生
      * @author Xiang Guo
      * @param  pitch_increment: 俯仰角增量，单位：无
      * @param  yaw_increment: 偏航角增量，单位：无
      * @param  constrain_pitch: 是否限制俯仰角的范围在[-89, 89]度之间
      * @retval none
      */
    void RotateArroundOrigin(float pitch_increment, float yaw_increment, bool constrain_pitch = true);

    /**
      * @brief  对相机视角缩放
      * @author Xiang Guo
      * @param  fov_increment: 视角增量，单位：无
      * @retval none
      */
    void Zoom(float fov_increment);

private:
    /**
      * @brief  根据相机的（更新的）Euler角度计算前矢量
      * @author Xiang Guo
      * @param  none
      * @retval none
      */
    void UpdateCameraVectors(void);
};
#endif
