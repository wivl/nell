#include "camera.hpp"
#include <glad/glad.h>


nell::Camera::Camera(vec3 position,
                     vec3 direction,
                     float vfov,
                     float aspect,
                     float focusLength) { // length from position to thing you look at

    this->position = position;
    this->direction = normalize(direction);
    this->vfov = vfov;
    this->aspect = aspect;
    this->focusDist = focusLength;

    this->loop = 0;

    update();
}


void nell::Camera::update() {
    // calculate

    float theta = radians(vfov);
    float halfHeight = tan(theta / 2);
    float halfWidth = aspect * halfHeight;

    this->w = normalize(-direction); // in camera space, camera looks at -z(w) direction
    // w, v, vup is in the same plane
    this->u = normalize(cross(vec3(0, 1, 0), w));     // so u can be calculated as the normal vector of the plane
    this->v = normalize(cross(w, u)); // up

    this->lowerLeftCorner = position
                            - halfWidth * focusDist * u
                            - halfHeight * focusDist * v
                            - focusDist * w;

    this->horizontal = 2 * halfWidth * focusDist * u;
    this->vertical = 2 * halfHeight * focusDist * v;
}

void nell::Camera::sync(unsigned int shaderId) {
    glUniform3f(glGetUniformLocation(shaderId, "camera.lowerLeftCorner"),
                this->lowerLeftCorner.x,
                this->lowerLeftCorner.y,
                this->lowerLeftCorner.z);
    glUniform3f(glGetUniformLocation(shaderId, "camera.horizontal"),
                this->horizontal.x,
                this->horizontal.y,
                this->horizontal.z);
    glUniform3f(glGetUniformLocation(shaderId, "camera.vertical"),
                this->vertical.x,
                this->vertical.y,
                this->vertical.z);
    glUniform3f(glGetUniformLocation(shaderId, "camera.position"),
                this->position.x,
                this->position.y,
                this->position.z);
    glUniform3f(glGetUniformLocation(shaderId, "camera.u"),
                this->u.x,
                this->u.y,
                this->u.z);
    glUniform3f(glGetUniformLocation(shaderId, "camera.v"),
                this->v.x,
                this->v.y,
                this->v.z);
    glUniform3f(glGetUniformLocation(shaderId, "camera.w"),
                this->w.x,
                this->w.y,
                this->w.z);

    this->loop = 0;
    glUniform1i(glGetUniformLocation(shaderId, "camera.loop"),
                this->loop);

}

void nell::Camera::updateAndSync(unsigned int shaderId) {
    update();
    sync(shaderId);
}

void nell::Camera::moveForward(float speed) {
    this->position -= this->w * speed;
}

void nell::Camera::moveBackward(float speed) {
    this->position += this->w * speed;
}

void nell::Camera::moveLeft(float speed) {
    this->position -= this->u * speed;
}

void nell::Camera::moveRight(float speed) {
    this->position += this->u * speed;
}

void nell::Camera::moveUp(float speed) {
    this->position += vec3(0, 1, 0) * speed;
}

void nell::Camera::moveDown(float speed) {
    this->position -= vec3(0, 1, 0) * speed;
}

