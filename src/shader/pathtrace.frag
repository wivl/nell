#version 460 core


in vec2 TexCoords;
out vec4 FragColor;

uniform float randOrigin;
uniform float time;

uniform int width;
uniform int height;

uniform sampler2D skybox;

#define MAX_VERTEX_NUM 40000
#define MAX_FACE_NUM 40000
#define MAX_MATERIAL_NUM 50

#define DEBUG_TRIANGLE_ONLY

uniform int faceCount;

layout(std430, binding = 0) buffer VertexBuffer {
    vec3 vertices[MAX_VERTEX_NUM];
    vec3 normals[MAX_VERTEX_NUM];

    ivec3 faces[MAX_FACE_NUM];

    float materials[MAX_MATERIAL_NUM];
    int materialPtrs[MAX_FACE_NUM];
};

#define PI 3.1415926535

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

vec2 rand2() {
    return vec2(rand(), rand());
}

vec3 rand3() {
    return vec3(rand(), rand(), rand());
}

vec4 rand4() {
    return vec4(rand(), rand(), rand(), rand());
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


struct Camera {
    vec3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;
    vec3 position;
    vec3 u, v, w;
    int loop;
};

uniform Camera camera;

Ray Camera_getRay(Camera camera, vec2 uv){
    Ray ray = Ray_make(camera.position,
                       camera.lowerLeftCorner +
                       uv.x * camera.horizontal +
                       uv.y * camera.vertical - camera.position);
    return ray;
}


const float Material_Lambertian = 0.0;
const float Material_Metal = 1.0;
const float Material_Dielectric = 2.0;

//uniform int materialArraySize;
//uniform float materials[20];

struct HitRecord {
    float t; // time
    vec3 position;
    vec3 normal;
    int materialPtr; // 材质开始的下标
    float materialType; // 材质的类型
};


struct Sphere {
    vec3 center;
    float radius;
    int material; // 既是下标，也是类型
};

Sphere Sphere_make(vec3 center, float radius, int materialptr) {
    Sphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    sphere.material = materialptr;
    return sphere;
}

bool Sphere_hit(Ray ray, Sphere sphere, float t_min, float t_max, inout HitRecord hitRecord) {
    vec3 oc = ray.origin - sphere.center;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;

    // 0, 1 or 2 roots
    float delta = b * b - 4 * a * c;

    if (delta > 0.0) { // 2 roots
        float temp = (-b - sqrt(delta)) / (2.0 * a); // nearest hit
        if (temp < t_max && temp > t_min) { // in the zone
            // set the record
            hitRecord.t = temp;
            hitRecord.position = Ray_getPosition(ray, hitRecord.t);
            // hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
            hitRecord.normal = normalize(hitRecord.position - sphere.center);

            hitRecord.materialPtr = sphere.material; // 材质数组的下标
            hitRecord.materialType = materials[sphere.material]; // 记录材质的类型

            return true;
        }

        temp = (-b + sqrt(delta)) / (2.0 * a); // further hit
        if (temp < t_max && temp > t_min) {
            hitRecord.t = temp;
            hitRecord.position = Ray_getPosition(ray, hitRecord.t);
            // hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
            hitRecord.normal = normalize(hitRecord.position - sphere.center);


            hitRecord.materialPtr = sphere.material; // 材质数组的下标
            hitRecord.materialType = materials[sphere.material]; // 记录材质的类型

            return true;
        }
    }
    return false;
}




struct Triangle {
    vec3 position[3];
    vec3 normal[3];
    int materialPtr;
    float materialType;
};

Triangle Triangle_get(int faceptr) {
    ivec3 face = faces[faceptr];
    Triangle triangle;
    triangle.position[0] = vertices[face.x];
    triangle.position[1] = vertices[face.y];
    triangle.position[2] = vertices[face.z];

    triangle.normal[0] = normals[face.x];
    triangle.normal[1] = normals[face.y];
    triangle.normal[2] = normals[face.z];

    triangle.materialPtr = materialPtrs[faceptr];
    triangle.materialType = materials[triangle.materialPtr];

    return triangle;
}

bool Triangle_hit(Ray ray, Triangle triangle, float t_min, float t_max, inout HitRecord hitRecord) {
    vec3 e1 = triangle.position[1] - triangle.position[0];
    vec3 e2 = triangle.position[2] - triangle.position[0];
    vec3 s = ray.origin - triangle.position[0];
    vec3 s1 = cross(ray.direction, e2);
    vec3 s2 = cross(s, e1);

    vec3 temp = vec3(
        dot(s2, e2),
        dot(s1, s),
        dot(s2, ray.direction)
    );

    vec3 hitResult = (1.0/dot(s1, e1)) *  temp;
    float t = hitResult.x;
    float b1 = hitResult.y;
    float b2 = hitResult.z;
    float b0 = 1.0 - b1 - b2;

    // test if hit result is available
    if (t < t_min) {
        return false;
    }
    if (t > t_max) {
        return false;
    }
    if (b0 < 0 || b1 < 0 || b2 < 0) {
        return false;
    }


    hitRecord.t = t;
    hitRecord.position = b0 * triangle.position[0] +
    b1 * triangle.position[1] +
    b2 * triangle.position[2];

    hitRecord.normal = b0 * triangle.normal[0] +
    b1 * triangle.normal[1] +
    b2 * triangle.normal[2];

    hitRecord.materialPtr = triangle.materialPtr;
    hitRecord.materialType = triangle.materialType;
    return true;
}



void Scatter_lambertian(int materialOffset, in Ray incident, in HitRecord hitRecord,
                        out Ray scattered, out vec3 attenuation) {
    vec3 albedo = vec3(materials[materialOffset+1], materials[materialOffset+2], materials[materialOffset+3]);

    attenuation = albedo;

    scattered.origin = hitRecord.position;
    scattered.direction = hitRecord.normal + randomInUnitSphere();
}

void Scatter_Metal(int materialOffset, in Ray incident, in HitRecord hitRecord,
                    out Ray scattered, out vec3 attenuation) {
    vec3 specular = vec3(materials[materialOffset+1], materials[materialOffset+2], materials[materialOffset+3]);
    float fuzz = materials[materialOffset+4];

    attenuation = specular;

    scattered.origin = hitRecord.position;
    scattered.direction = reflect(incident.direction, hitRecord.normal) + fuzz * randomInUnitSphere();

}

bool refract(vec3 v, vec3 n, float ni_over_nt, out vec3 refracted){
    vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
    if (discriminant > 0.0){
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }

    return false;
}

float schlick(float cosine, float ior){
    float r0 = (1 - ior) / (1 + ior);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}


void Scatter_Dielectric(int materialOffset, in Ray incident, in HitRecord hitRecord,
                    out Ray scattered, out vec3 attenuation) {
    vec3 color = vec3(materials[materialOffset+1], materials[materialOffset+2], materials[materialOffset+3]);
    float refractIndex = materials[materialOffset+4];

    vec3 reflected = reflect(incident.direction, hitRecord.normal);
    attenuation = color;


    vec3 outward_normal;
    float ni_over_nt;
    float cosine;
    if (dot(incident.direction, hitRecord.normal) > 0.0) { // 从内击中
                                                            outward_normal = -hitRecord.normal;
                                                            ni_over_nt = refractIndex;
                                                            cosine = dot(incident.direction, hitRecord.normal) / length(incident.direction);//入射光线角度
    } else { // 从外击中
             outward_normal = hitRecord.normal;
             ni_over_nt = 1.0 / refractIndex;
             cosine = -dot(incident.direction, hitRecord.normal) / length(incident.direction);//入射光线角度
    }

    float reflect_prob;
    vec3 refracted;
    if (refract(incident.direction, outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine, refractIndex);
    } else {
        reflect_prob = 1.0;
    }

    if (rand() < reflect_prob) {
        scattered = Ray(hitRecord.position, refracted);
    } else {
        scattered = Ray(hitRecord.position, refracted);
    }
}

vec3 getEnvironmentColor(Ray ray) {
//    vec3 normalizeDir = normalize(ray.direction);
//    float t = (normalizeDir.y + 1.0) * 0.5;
//    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

    vec3 direction = normalize(ray.direction);
    float phi = atan(direction.z, direction.x);
    float theta = acos(direction.y);

    float u = (phi + PI) / (2.0 * PI);
    float v = 1.0 - theta / PI;

    return texture(skybox, vec2(u, v)).rgb;
}

struct Scene {
    int sphereCount;
    Sphere spheres[10];
};

Scene Scene_demo_1() {
    Scene scene;
    scene.spheres[0] = Sphere_make(
        vec3(0, 0, -2),
        0.5,
        0
    );
    scene.spheres[1] = Sphere_make(
        vec3(1, 0, -2),
        0.5,
        4
    );
    scene.spheres[2] = Sphere_make(
        vec3(-1, 0, -2),
        0.5,
        9
    );
    scene.sphereCount = 3;
    return scene;
}


#ifdef DEBUG_TRIANGLE_ONLY
bool Scene_hit(Scene scene, Ray ray, float t_min, float t_max, inout HitRecord hitRecord) {
    HitRecord tempRecord;
    bool hitAnything = false;
    float t_close = t_max;

    for (int i = 0; i < faceCount; i++) {
        Triangle triangle = Triangle_get(i);
        if (Triangle_hit(ray, triangle, t_min, t_close, tempRecord)) {
            hitRecord = tempRecord;
            hitAnything = true;
            t_close = hitRecord.t;
        }
    }

    return hitAnything;
}
#else
bool Scene_hit(Scene scene, Ray ray, float t_min, float t_max, inout HitRecord hitRecord) {
    HitRecord tempRecord;
    bool hitAnything = false;
    float t_close = t_max;

    for (int i = 0; i < scene.sphereCount; i++) {
        if (Sphere_hit(ray, scene.spheres[i], t_min, t_close, tempRecord)) {
            hitRecord = tempRecord;
            hitAnything = true;
            t_close = hitRecord.t;
        }
    }

    return hitAnything;
}
#endif



vec3 trace(Ray ray, int depth) {
    Scene scene = Scene_demo_1();

    HitRecord hitRecord;
    vec3 bgColor = vec3(0);
    vec3 objColor = vec3(1.0);

    while (depth > 0) {
        depth--;
        if (Scene_hit(scene, ray, 0.001, 100000.0, hitRecord)) {
            vec3 attenuation;
            Ray scatterRay;

            // ray intersect
            if (hitRecord.materialType == Material_Lambertian) {
                Scatter_lambertian(hitRecord.materialPtr, ray, hitRecord,
                        scatterRay, attenuation);
            } else if (hitRecord.materialType == Material_Metal) {
                Scatter_Metal(hitRecord.materialPtr, ray, hitRecord,
                    scatterRay, attenuation);
            } else if (hitRecord.materialType == Material_Dielectric) {
                Scatter_Dielectric(hitRecord.materialPtr, ray, hitRecord,
                              scatterRay, attenuation);
            }

            ray = scatterRay;
            // shading
            objColor *= attenuation;
        } else {
            bgColor = getEnvironmentColor(ray);
            break;
        }
    }
    return objColor * bgColor;
}

vec3 gammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}



void main() {
    float u = TexCoords.x; // 0 ~ 1
    float v = TexCoords.y; // 0 ~ 1

    wseed = uint(randOrigin * float(6.95857) * (TexCoords.x * TexCoords.y) + fract(time));
    vec2 screenSize = vec2(width, height);

    vec3 color = vec3(0.0, 0.0, 0.0);
    int spp = 1;
    for (int i = 0; i < spp; i++) {
        Ray ray = Camera_getRay(camera, TexCoords + rand2() / screenSize);
        color += trace(ray, 50);
    }
    color /= spp;
    color = gammaCorrection(color);

    FragColor = vec4(color, 1.0);



}

