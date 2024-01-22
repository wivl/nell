#pragma once

#include <glm/glm.hpp>
// #include <glm/ext.hpp>
using namespace glm;

// FIX: update
namespace nell {
    class Camera {
        private:
            vec3 lower_left_corner;
            vec3 origin, lookat;
            vec3 horizontal, vertical;
            vec3 u, v, w; // vectors that point to 3 axises
            float lens_radius; // 镜头半径, aperture: 透镜直径
            float vfov; // vertical fov
            float aspect; // viewport width / height
        public:
            Camera(vec3 lookfrom, vec3 lookat, vec3 vup,
                    float vfov, // top to bottom, in degrees
                    float aspect, float aperture, float focus_dist);

            // TODO: complete setters
            void position(vec3 p) { origin = p; }
            vec3 position() const { return origin; }

            void set_lens_radius(float lr) { lens_radius = lr; }

            void at(vec3 dir);
            vec3 at();




    };
}
