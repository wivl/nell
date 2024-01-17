#version 330 core

in vec2 screenCoord;

out vec4 FragColor;

// raytracing

// ray
struct Ray {
    vec3 origin;
    vec3 direction;
};


Ray createRay(vec3 o, vec3 d) {
    Ray ray;
    ray.origin = o;
    ray.direction = d;
    return ray;
}


vec3 getRayPosition(Ray ray, float t) {
    return ray.origin + t*ray.direction;
}


// camera
struct Camera {
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
};


Camera makeCamera(vec3 position, vec3 vertical, vec3 horizontal, vec3 lowerleft) {
    Camera camera;
    camera.origin = position;
    camera.vertical = vertical;
    camera.horizontal = horizontal;
    camera.lower_left_corner = lowerleft;
    return camera;
}




vec3 rayTrace(Ray ray) {
    vec3 normalizeDir = normalize(ray.direction);
    float t = (normalizeDir.y + 1.0) * 0.5;

    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void main() {
    float u = screenCoord.x; // 0 ~ 1
    float v = screenCoord.y; // 0 ~ 1

    Camera camera = makeCamera(
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 2.0, 0.0),
            vec3(4.0, 0.0, 0.0),
            vec3(-2.0, -1.0, -1.0)
            );

    Ray ray = createRay(
            camera.origin,
            camera.lower_left_corner + u*camera.horizontal + v*camera.vertical - camera.origin
            );

    FragColor = vec4(rayTrace(ray), 1.0);
}
