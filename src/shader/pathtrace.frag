#version 410 core


in vec2 TexCoords;
out vec4 FragColor;

uniform float randOrigin;
uniform float time;

uniform int width;
uniform int height;

uint wseed;
// returns a [0, 1) value
float randcore(inout uint seed) {
    seed = (seed ^ uint(61)) ^ (seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> uint(4));
    seed *= uint(0x27d4eb2d);
    wseed = seed ^ (seed >> uint(15));
    return float(wseed) * (1.0 / 4294967296.0);
}

float rand() {
    return randcore(wseed);
}

vec3 randomInUnitSphere() {
    vec3 p;
    float theta = rand() * 2.0 * PI;
    float phi   = rand() * PI;
    p.y = cos(phi);
    p.x = sin(phi) * cos(theta);
    p.z = sin(phi) * sin(theta);

    return p;
}


struct Ray {
    vec3 origin;
    vec3 direction;
};


Ray Ray_make(vec3 o, vec3 d) {
    Ray ray;
    ray.origin = o;
    ray.direction = normalize(d);
    return ray;
}

vec3 Ray_getPosition(Ray ray, float t) {
    return ray.origin + t*ray.direction;
}



void main() {
    float u = TexCoords.x; // 0 ~ 1
    float v = TexCoords.y; // 0 ~ 1

    wseed = uint(randOrigin * float(6.95857) * (TexCoords.x * TexCoords.y) + fract(time));
    vec2 screenSize = vec2(width, height);

    FragColor = vec4(rand(), rand(), rand(), 1);



}

