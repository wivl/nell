#pragma once

#include <glm/glm.hpp>
using namespace glm;

namespace nell {
    // TODO: 重写 camera，定义 direction 取代 lookat
    class Camera {
    public:
        vec3 position;
        vec3 direction;
        float vfov;
        float aspect;
        float focusDist;

        int loop;
    private:
        vec3 lowerLeftCorner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w; // back, right, up(not vertically up)

    public:
        Camera(vec3 position,
               vec3 direction,
               float vfov,
               float aspect,
               float focusLength);

        void update();
        void sync(unsigned int shaderId);
        void updateAndSync(unsigned int shaderId);

        void moveForward(float speed);
        void moveBackward(float speed);
        void moveLeft(float speed);
        void moveRight(float speed);
        void moveUp(float speed);
        void moveDown(float speed);
    };
}