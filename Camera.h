#ifndef CAMERA_H
#define CAMERA_H

#include "BasicDataStructure.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum CameraPara {FOV,NEAR};
//Orbit Camera
class Camera
{
public:
    Camera(float _aspect, float far):aspect(_aspect),zFar(far){}
    float aspect;
    Vector3D position;
    Vector3D target;
    float zNear;
    float zFar;
    float fov;
    void RotateAroundTarget(Vector2D motion);
    void MoveTarget(Vector2D motion);
    void CloseToTarget(int ratio);
    void SetModel(Coord3D modelCentre, float yRange);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
};

#endif // CAMERA_H
