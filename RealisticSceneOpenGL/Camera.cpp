#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
       
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }

 
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD) {
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
        }
        else if (direction == MOVE_BACKWARD) {
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
        }
        else if (direction == MOVE_RIGHT) {
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
        }
        else if (direction == MOVE_LEFT) {
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        cameraTarget.x = cameraPosition.x + cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraTarget.y = cameraPosition.y + sin(glm::radians(pitch));
        cameraTarget.z = cameraPosition.z + sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }
}  