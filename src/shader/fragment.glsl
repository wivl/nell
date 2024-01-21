#version 330 core

#define WIDTH 1600
#define HEIGHT 800

#define PI 3.1415926535

in vec2 screenCoord;

out vec4 FragColor;


// ---ramdom
uint m_u = uint(521288629);
uint m_v = uint(362436069);

uint GetUintCore(inout uint u, inout uint v){
    v = uint(36969) * (v & uint(65535)) + (v >> 16);
    u = uint(18000) * (u & uint(65535)) + (u >> 16);
    return (v << 16) + u;
}

float GetUniformCore(inout uint u, inout uint v){
    uint z = GetUintCore(u, v);

    return float(z) / uint(4294967295);
}

float GetUniform(){
    return GetUniformCore(m_u, m_v);
}

uint GetUint(){
    return GetUintCore(m_u, m_v);
}

float rand(){
    return GetUniform();
}

vec2 rand2(){
    return vec2(rand(), rand());
}

vec3 rand3(){
    return vec3(rand(), rand(), rand());
}

vec4 rand4(){
    return vec4(rand(), rand(), rand(), rand());
}

// --material
#define MAT_LAMBERTIAN 0
#define MAT_METALLIC 1
#define MAT_DIELECTRIC 2


// ---ray
struct Ray {
    vec3 origin;
    vec3 direction;
};


Ray makeRay(vec3 o, vec3 d) {
    Ray ray;
    ray.origin = o;
    ray.direction = d;
    return ray;
}


vec3 getRayPosition(Ray ray, float t) {
    return ray.origin + t*ray.direction;
}


// ---camera
struct Camera {
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    vec3 u, v, w;
    float lens_radius;
};


// TODO:
Camera makeCamera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
    Camera camera;

    camera.origin = lookfrom;
    camera.lens_radius = aperture / 2;

    float theta = radians(vfov);
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    camera.w = normalize(lookfrom - lookat);
    camera.u = normalize(cross(vup, camera.w));
    camera.v = cross(camera.w, camera.u);

    camera.lower_left_corner = camera.origin
    - half_width * focus_dist * camera.u
    - half_height * focus_dist * camera.v
    - focus_dist * camera.w;

    camera.horizontal = 2 * half_width * focus_dist * camera.u;
    camera.vertical = 2 * half_height * focus_dist * camera.v;

    return camera;
}

// ---hit record
struct HitRecord {
    float t;
    vec3 position;
    vec3 normal;
    int material_ptr; // 材质数组的下标
    int material_type; // 记录材质的类型
};

// ---sphere
struct Sphere {
    vec3 center;
    float radius;
    int material_ptr; // 材质数组的下标
    int material_type; // 记录材质的类型
};

Sphere makeSphere(vec3 c, float r, int type, int ptr) {
    Sphere sphere;
    sphere.center = c;
    sphere.radius = r;
    sphere.material_type = type;
    sphere.material_ptr = ptr;
    return sphere;
}

bool hitSphere(Ray ray, Sphere sphere, float t_min, float t_max, inout HitRecord hitRecord) {
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
            hitRecord.position = getRayPosition(ray, hitRecord.t);
            // hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
            hitRecord.normal = normalize(hitRecord.position - sphere.center);

            hitRecord.material_ptr = sphere.material_ptr; // 材质数组的下标
            hitRecord.material_type = sphere.material_type; // 记录材质的类型

            return true;
        }

        temp = (-b + sqrt(delta)) / (2.0 * a); // further hit
        if (temp < t_max && temp > t_min) {
            hitRecord.t = temp;
            hitRecord.position = getRayPosition(ray, hitRecord.t);
            // hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
            hitRecord.normal = normalize(hitRecord.position - sphere.center);

            hitRecord.material_ptr = sphere.material_ptr; // 材质数组的下标
            hitRecord.material_type = sphere.material_type; // 记录材质的类型

            return true;
        }
    }
    return false;
}


vec3 random_in_unit_sphere() {
    vec3 p;

    float theta = rand() * 2.0 * PI;
    float phi   = rand() * PI;
    p.y = cos(phi);
    p.x = sin(phi) * cos(theta);
    p.z = sin(phi) * sin(theta);

    return p;
}

// ---material
// 漫反射
struct Lambertian{
    vec3 albedo; // color
};

// 金属
struct Metallic{
    vec3 albedo; // color
    float fuzz;  // TODO:
};

// 电介质
struct Dielectric{
    vec3 albedo;
    float ior;
};

Lambertian lambertMaterials[4];
Metallic metallicMaterials[4];
Dielectric dielectricMaterials[4];

Lambertian makeLambertian(vec3 albedo){
    Lambertian lambertian;

    lambertian.albedo = albedo;

    return lambertian;
}

void lambertianScatter(in Lambertian lambertian,
in Ray incident,
in HitRecord hitRecord,
out Ray scattered,
out vec3 attenuation){
    // return a color value
    attenuation = lambertian.albedo;

    // scattered ray
    scattered.origin = hitRecord.position;
    scattered.direction = hitRecord.normal + random_in_unit_sphere();
}



Metallic makeMetallic(vec3 albedo, float fuzz){
    Metallic metallic;

    metallic.albedo = albedo;
    metallic.fuzz = fuzz;

    return metallic;
}

// return scattered vector
vec3 reflect(in vec3 v, in vec3 n) {
    return v - 2 * dot(n, v) * n;
}

void metallicScatter(in Metallic metallic,
in Ray incident,
in HitRecord hitRecord,
out Ray scattered,
out vec3 attenuation) {
    attenuation = metallic.albedo;

    scattered.origin = hitRecord.position;
    scattered.direction = reflect(incident.direction, hitRecord.normal) + metallicMaterials[hitRecord.material_ptr].fuzz * random_in_unit_sphere();
}


Dielectric makeDielectric(vec3 albedo, float ior){
    Dielectric dielectric;

    dielectric.albedo = albedo;
    dielectric.ior = ior;

    return dielectric;
}

// TODO:
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

void dielectricScatter(in Dielectric dielectric,
in Ray incident,
in HitRecord hitRecord,
out Ray scattered,
out vec3 attenuation) {
    attenuation = dielectric.albedo;
    vec3 reflected = reflect(incident.direction, hitRecord.normal);

    vec3 outward_normal;
    float ni_over_nt;
    float cosine;
    if (dot(incident.direction, hitRecord.normal) > 0.0) {//从内击中
        outward_normal = -hitRecord.normal;
        ni_over_nt = dielectric.ior;
        cosine = dot(incident.direction, hitRecord.normal) / length(incident.direction);//入射光线角度
    } else {// 从外击中
        outward_normal = hitRecord.normal;
        ni_over_nt = 1.0 / dielectric.ior;
        cosine = -dot(incident.direction, hitRecord.normal) / length(incident.direction);//入射光线角度
    }

    float reflect_prob;
    vec3 refracted;
    if (refract(incident.direction, outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine, dielectric.ior);
    } else {
        reflect_prob = 1.0;
    }

    if (rand() < reflect_prob) {
        scattered = Ray(hitRecord.position, refracted);
    } else {
        scattered = Ray(hitRecord.position, refracted);
    }
}


// ---world
struct World {
    int object_count;
    Sphere objects[100];
};

World makeWorld() {
    lambertMaterials[0] = makeLambertian(vec3(0.1, 0.7, 0.7));
    lambertMaterials[1] = makeLambertian(vec3(0.5, 0.5, 0.5));
    metallicMaterials[0] = makeMetallic(vec3(0.8, 0.8, 0.8), 0.3);
    dielectricMaterials[0] = makeDielectric(vec3(1.0, 1.0, 1.0), 1.5);

    World world;
    world.objects[0] = makeSphere(
    vec3(0, 0, -1),
    0.5,
    MAT_LAMBERTIAN,
    0);
    world.objects[1] = makeSphere(
    vec3(0, -100.5, -1.0),
    100.0,
    MAT_LAMBERTIAN,
    1);
    world.objects[2] = makeSphere(
    vec3(1.0, 0.0, -1.0),
    0.5,
    MAT_METALLIC,
    0);
    world.objects[3] = makeSphere(
    vec3(-1.0, 0.0, -1.0),
    0.5,
    MAT_DIELECTRIC,
    0);

    world.object_count = 4;

    return world;
}



bool hitWorld(World world, Ray ray, float t_min, float t_max, inout HitRecord hitRecord) {
    HitRecord tempRecord;
    bool hitAnyThing = false;
    float t_close = t_max; // closest do far

    for (int i = 0; i < world.object_count; i++) {
        if (hitSphere(ray, world.objects[i], t_min, t_close, tempRecord)) {
            hitRecord = tempRecord;
            hitAnyThing = true;
            t_close = hitRecord.t;
        }
    }

    return hitAnyThing;

}



// ---raytracing

vec3 getEnvironemntColor(Ray ray){
    vec3 normalizeDir = normalize(ray.direction);
    float t = (normalizeDir.y + 1.0) * 0.5;
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

vec3 rayTrace(Ray ray, int depth) {

    World world = makeWorld();

    HitRecord hitRecord;

    vec3 bgColor = vec3(0);
    vec3 objColor = vec3(1.0);

    while (depth > 0) {
        depth --;
        if (hitWorld(world, ray, 0.001, 100000.0, hitRecord)) {
            vec3 attenuation; // 衰减值，物体颜色
            Ray scatterRay;

            if (hitRecord.material_type == MAT_LAMBERTIAN) {
                lambertianScatter(lambertMaterials[hitRecord.material_ptr],
                ray,
                hitRecord,
                scatterRay,
                attenuation);
            } else if (hitRecord.material_type == MAT_METALLIC) {
                metallicScatter(metallicMaterials[hitRecord.material_ptr], ray, hitRecord, scatterRay, attenuation);
            } else if (hitRecord.material_type == MAT_DIELECTRIC) {
                dielectricScatter(dielectricMaterials[hitRecord.material_ptr], ray, hitRecord, scatterRay, attenuation);
            }

            ray = scatterRay;
            objColor *= attenuation;
        } else {
            bgColor = getEnvironemntColor(ray);
            break;
        }
    }

    return objColor * bgColor;
}

Ray cameraGetRay(Camera camera, vec2 uv){
    Ray ray = makeRay(camera.origin,
    camera.lower_left_corner +
    uv.x * camera.horizontal +
    uv.y * camera.vertical - camera.origin);

    return ray;
}

vec3 gammaCorrection(vec3 c){
    return pow(c, vec3(1.0 / 2.2));
}



void main() {
    float u = screenCoord.x; // 0 ~ 1
    float v = screenCoord.y; // 0 ~ 1

    vec2 screenSize = vec2(WIDTH, HEIGHT);

    float aspect_ratio = screenSize.x / screenSize.y;
    vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    float dist_to_focus = 1000.0;
    float aperture = 0.1;

    Camera camera = makeCamera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);


    vec3 color = vec3(0.0, 0.0, 0.0);
    int spp = 100;
    for (int i = 0; i < spp; i++) {
        Ray ray = cameraGetRay(camera, screenCoord + rand2() / screenSize);
        color += rayTrace(ray, 50);
    }
    color /= spp;

    color = gammaCorrection(color);


    FragColor = vec4(color, 1.0);
}