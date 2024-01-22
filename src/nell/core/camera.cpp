#include "camera.hpp"
#include "glm/ext/quaternion_geometric.hpp"

#include <glm/gtc/constants.hpp>

nell::Camera::Camera(vec3 lookfrom, vec3 lookat, vec3 vup,
        float vfov, // top to bottom, in degrees
        float aspect, float aperture,
        float focus_dist) { // length from origin to thing you look at

    this->lookat = lookat;
    origin = lookfrom;
    lens_radius = aperture / 2;

    float theta = radians(vfov);
    float half_height = tan(theta/2);
    float half_width = aspect * half_height;

    w = normalize(lookfrom - lookat); // in camera space, camera looks at -z(w) direction
                                      // w, v, vup in in the same plane
    u = normalize(cross(vup, w));     // so u can be calculated as the normal vector of the plane
    v = cross(w, u); // up

    lower_left_corner = origin
        - half_width * focus_dist * u
        - half_height * focus_dist * v
        - focus_dist * w;

    horizontal = 2 * half_width*focus_dist*u;
    vertical = 2 * half_height*focus_dist*v;
}


void nell::Camera::at(vec3 dir) {
    
}

vec3 nell::Camera::at() {
    return lookat;
}
