#version 330 core

#define WIDTH 1600
#define HEIGHT 800

#define PI 3.1415926535

in vec2 screenCoord;

out vec4 FragColor;


// ---random
// random algorithm by Thomas Wang
uint wseed;
float randcore(uint seed) {
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

vec2 rand2(){
    return vec2(rand(), rand());
}

vec3 rand3(){
    return vec3(rand(), rand(), rand());
}

vec4 rand4(){
    return vec4(rand(), rand(), rand(), rand());
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

vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3(rand()*2.0-1.0, rand()*2.0-1.0, 0.0);
        if (dot(p, p) < 1.0)
        return p;
    }
}




// ---ray
struct Ray {
    vec3 origin;
    vec3 direction;
};


Ray Ray_make(vec3 o, vec3 d) {
    Ray ray;
    ray.origin = o;
    ray.direction = d;
    return ray;
}


vec3 Ray_get_position(Ray ray, float t) {
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


// TODO: 临时编写的构造函数，后期使用 uniform 赋值
Camera Camera_make(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
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

Ray Camera_get_ray(Camera camera, vec2 uv){
//    vec3 rd = camera.lens_radius * random_in_unit_disk();
//    vec3 offset = camera.u * rd.x + camera.v * rd.y;
//    Ray ray = Ray_make(camera.origin + offset,
//                       camera.lower_left_corner +
//                       uv.x * camera.horizontal +
//                       uv.y * camera.vertical - camera.origin);
    Ray ray = Ray_make(camera.origin,
                   camera.lower_left_corner +
                   uv.x * camera.horizontal +
                   uv.y * camera.vertical - camera.origin);

    return ray;
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

Sphere Sphere_make(vec3 c, float r, int type, int ptr) {
    Sphere sphere;
    sphere.center = c;
    sphere.radius = r;
    sphere.material_type = type;
    sphere.material_ptr = ptr;
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
            hitRecord.position = Ray_get_position(ray, hitRecord.t);
            // hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
            hitRecord.normal = normalize(hitRecord.position - sphere.center);

            hitRecord.material_ptr = sphere.material_ptr; // 材质数组的下标
            hitRecord.material_type = sphere.material_type; // 记录材质的类型

            return true;
        }

        temp = (-b + sqrt(delta)) / (2.0 * a); // further hit
        if (temp < t_max && temp > t_min) {
            hitRecord.t = temp;
            hitRecord.position = Ray_get_position(ray, hitRecord.t);
            // hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
            hitRecord.normal = normalize(hitRecord.position - sphere.center);

            hitRecord.material_ptr = sphere.material_ptr; // 材质数组的下标
            hitRecord.material_type = sphere.material_type; // 记录材质的类型

            return true;
        }
    }
    return false;
}



// --material
#define MAT_LAMBERTIAN 0
#define MAT_METALLIC 1
#define MAT_DIELECTRIC 2

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

Lambertian lambert_materials[4];
Metallic metallic_materials[4];
Dielectric dielectric_materials[4];

Lambertian Material_lambertian_make(vec3 albedo){
    Lambertian lambertian;

    lambertian.albedo = albedo;

    return lambertian;
}

void Material_lambertian_scatter(in Lambertian lambertian,
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



Metallic Material_metallic_make(vec3 albedo, float fuzz){
    Metallic metallic;

    metallic.albedo = albedo;
    metallic.fuzz = fuzz;

    return metallic;
}

// return scattered vector
vec3 reflect(in vec3 v, in vec3 n) {
    return v - 2 * dot(n, v) * n;
}

void Material_metallic_scatter(in Metallic metallic,
in Ray incident,
in HitRecord hitRecord,
out Ray scattered,
out vec3 attenuation) {
    attenuation = metallic.albedo;

    scattered.origin = hitRecord.position;
    scattered.direction = reflect(incident.direction, hitRecord.normal) + metallic_materials[hitRecord.material_ptr].fuzz * random_in_unit_sphere();
}


Dielectric Material_dielectric_make(vec3 albedo, float ior){
    Dielectric dielectric;

    dielectric.albedo = albedo;
    dielectric.ior = ior;

    return dielectric;
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

void Material_dielectric_scatter(in Dielectric dielectric,
in Ray incident,
in HitRecord hit_record,
out Ray scattered,
out vec3 attenuation) {
    attenuation = dielectric.albedo;
    vec3 reflected = reflect(incident.direction, hit_record.normal);

    vec3 outward_normal;
    float ni_over_nt;
    float cosine;
    if (dot(incident.direction, hit_record.normal) > 0.0) { // 从内击中
        outward_normal = -hit_record.normal;
        ni_over_nt = dielectric.ior;
        cosine = dot(incident.direction, hit_record.normal) / length(incident.direction);//入射光线角度
    } else { // 从外击中
        outward_normal = hit_record.normal;
        ni_over_nt = 1.0 / dielectric.ior;
        cosine = -dot(incident.direction, hit_record.normal) / length(incident.direction);//入射光线角度
    }

    float reflect_prob;
    vec3 refracted;
    if (refract(incident.direction, outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine, dielectric.ior);
    } else {
        reflect_prob = 1.0;
    }

    if (rand() < reflect_prob) {
        scattered = Ray(hit_record.position, refracted);
    } else {
        scattered = Ray(hit_record.position, refracted);
    }
}


// ---world
struct World {
    int object_count;
    Sphere objects[100];
};

World World_make() {
    lambert_materials[0] = Material_lambertian_make(vec3(0.1, 0.7, 0.7));
    lambert_materials[1] = Material_lambertian_make(vec3(0.5, 0.5, 0.5));
    metallic_materials[0] = Material_metallic_make(vec3(0.8, 0.8, 0.8), 0.3);
    dielectric_materials[0] = Material_dielectric_make(vec3(1.0, 1.0, 1.0), 1.5);

    World world;
    world.objects[0] = Sphere_make(
    vec3(0, 0, -1),
    0.5,
    MAT_LAMBERTIAN,
    0);
    world.objects[1] = Sphere_make(
    vec3(0, -100.5, -1.0),
    100.0,
    MAT_LAMBERTIAN,
    1);
    world.objects[2] = Sphere_make(
    vec3(1.0, 0.0, -1.0),
    0.5,
    MAT_METALLIC,
    0);
    world.objects[3] = Sphere_make(
    vec3(-1.0, 0.0, -1.0),
    0.5,
    MAT_DIELECTRIC,
    0);

    world.object_count = 4;

    return world;
}



bool World_hit(World world, Ray ray, float t_min, float t_max, inout HitRecord hit_record) {
    HitRecord temp_record;
    bool hit_anything = false;
    float t_close = t_max; // closest do far

    for (int i = 0; i < world.object_count; i++) {
        if (Sphere_hit(ray, world.objects[i], t_min, t_close, temp_record)) {
            hit_record = temp_record;
            hit_anything = true;
            t_close = hit_record.t;
        }
    }

    return hit_anything;

}



// ---raytracing
vec3 get_environemnt_color(Ray ray){
    vec3 normalizeDir = normalize(ray.direction);
    float t = (normalizeDir.y + 1.0) * 0.5;
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

vec3 ray_trace(Ray ray, int depth) {

    World world = World_make();

    HitRecord hit_record;

    vec3 bg_color = vec3(0);
    vec3 obj_color = vec3(1.0);

    while (depth > 0) {
        depth --;
        if (World_hit(world, ray, 0.001, 100000.0, hit_record)) {
            vec3 attenuation; // 衰减值，物体颜色
            Ray scatter_ray;

            if (hit_record.material_type == MAT_LAMBERTIAN) {
                Material_lambertian_scatter(lambert_materials[hit_record.material_ptr],
                                            ray,
                                            hit_record,
                                            scatter_ray,
                                            attenuation);
            } else if (hit_record.material_type == MAT_METALLIC) {
                Material_metallic_scatter(metallic_materials[hit_record.material_ptr], ray, hit_record, scatter_ray, attenuation);
            } else if (hit_record.material_type == MAT_DIELECTRIC) {
                Material_dielectric_scatter(dielectric_materials[hit_record.material_ptr], ray, hit_record, scatter_ray, attenuation);
            }

            ray = scatter_ray;
            obj_color *= attenuation;
        } else {
            bg_color = get_environemnt_color(ray);
            break;
        }
    }

    return obj_color * bg_color;
}



vec3 gamma_correction(vec3 c){
    return pow(c, vec3(1.0 / 2.2));
}



void main() {
    float u = screenCoord.x; // 0 ~ 1
    float v = screenCoord.y; // 0 ~ 1

    wseed = uint(float(69557857) * (screenCoord.x * screenCoord.y));
    vec2 screen_size = vec2(WIDTH, HEIGHT);

    float aspect_ratio = screen_size.x / screen_size.y;
    vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    float dist_to_focus = length(lookat-lookfrom);
    float aperture = 0.1;

    Camera camera = Camera_make(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);


    vec3 color = vec3(0.0, 0.0, 0.0);
    int spp = 100;
    for (int i = 0; i < spp; i++) {
        Ray ray = Camera_get_ray(camera, screenCoord + rand2() / screen_size);
        color += ray_trace(ray, 50);
    }
    color /= spp;

    color = gamma_correction(color);


    FragColor = vec4(color, 1.0);
}
