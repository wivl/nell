#include "camera.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glad/glad.h>

#include <glm/gtc/constants.hpp>

nell::Camera::Camera(vec3 lookfrom,
                     vec3 lookat,
                     vec3 vup,
                     float vfov,
                     float aspect,
                     float aperture,
                     float focus_dist) { // length from origin to thing you look at

    this->origin = lookfrom;
    this->lookat = lookat;
    this->vup = vup;
    this->vfov = vfov;
    this->aspect = aspect;
    this->aperture = aperture;
    this->focus_dist = focus_dist;

    this->loop = 0.0f;

    // calculate
    this->lens_radius = aperture / 2;

    float theta = radians(vfov);
    float half_height = tan(theta/2);
    float half_width = aspect * half_height;

    this->w = normalize(lookfrom - lookat); // in camera space, camera looks at -z(w) direction
    // w, v, vup is in the same plane
    this->u = normalize(cross(vup, w));     // so u can be calculated as the normal vector of the plane
    this->v = cross(w, u); // up

    this->lower_left_corner = origin
                              - half_width * focus_dist * u
                              - half_height * focus_dist * v
                              - focus_dist * w;

    this->horizontal = 2 * half_width*focus_dist * u;
    this->vertical = 2 * half_height*focus_dist * v;
}


void nell::Camera::update() {
    // calculate
    this->lens_radius = aperture / 2;

    float theta = radians(vfov);
    float half_height = tan(theta/2);
    float half_width = aspect * half_height;

    this->w = normalize(origin - lookat); // in camera space, camera looks at -z(w) direction
    // w, v, vup is in the same plane
    this->u = normalize(cross(vup, w));     // so u can be calculated as the normal vector of the plane
    this->v = cross(w, u); // up

    this->lower_left_corner = origin
                              - half_width * focus_dist * u
                              - half_height * focus_dist * v
                              - focus_dist * w;

    this->horizontal = 2 * half_width*focus_dist * u;
    this->vertical = 2 * half_height*focus_dist * v;
}

void nell::Camera::sync(unsigned int shader_id) {
    glUniform3f(glGetUniformLocation(shader_id, "camera.lower_left_corner"),
                this->lower_left_corner.x,
                this->lower_left_corner.y,
                this->lower_left_corner.z);
    glUniform3f(glGetUniformLocation(shader_id, "camera.horizontal"),
                this->horizontal.x,
                this->horizontal.y,
                this->horizontal.z);
    glUniform3f(glGetUniformLocation(shader_id, "camera.vertical"),
                this->vertical.x,
                this->vertical.y,
                this->vertical.z);
    glUniform3f(glGetUniformLocation(shader_id, "camera.origin"),
                this->origin.x,
                this->origin.y,
                this->origin.z);
    glUniform3f(glGetUniformLocation(shader_id, "camera.u"),
                this->u.x,
                this->u.y,
                this->u.z);
    glUniform3f(glGetUniformLocation(shader_id, "camera.v"),
                this->v.x,
                this->v.y,
                this->v.z);
    glUniform3f(glGetUniformLocation(shader_id, "camera.w"),
                this->w.x,
                this->w.y,
                this->w.z);
    glUniform1f(glGetUniformLocation(shader_id, "camera.horizontal"),
                this->lens_radius);
    this->loop = 0;

}

void nell::Camera::update_and_sync(unsigned int shader_id) {
    update();
    sync(shader_id);
}`