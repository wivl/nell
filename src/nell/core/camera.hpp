#pragma once

#include <glm/glm.hpp>
// #include <glm/ext.hpp>
using namespace glm;

// FIX: update
// vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist
namespace nell {
    class Camera {
    private:
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 origin;
        vec3 u, v, w;
        float lens_radius;


    public:
        vec3 lookat;
        vec3 vup;
        float vfov;
        float aspect;
        float aperture;
        float focus_dist;

        void update();
        void sync(unsigned int shader_id);
        void update_and_sync(unsigned int shader_id);

        float loop;

    public:
        Camera(vec3 lookfrom,
               vec3 lookat,
               vec3 vup,
               float vfov,
               float aspect,
               float aperture,
               float focus_dist);

        // TODO: complete setters
        void position(vec3 p) { origin = p; }

        vec3 position() const { return origin; }

        void set_lens_radius(float lr) { lens_radius = lr; }

//        void at(vec3 dir);
//
//        vec3 at();


    };
}
