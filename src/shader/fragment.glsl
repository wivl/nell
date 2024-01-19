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
};


Camera makeCamera(vec3 position, vec3 vertical, vec3 horizontal, vec3 lowerleft) {
    Camera camera;
    camera.origin = position;
    camera.vertical = vertical;
    camera.horizontal = horizontal;
    camera.lower_left_corner = lowerleft;
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

// 漫反射
struct Lambertian{
	vec3 albedo; // color
};

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

// 金属
struct Metallic{
	vec3 albedo; // color
	float fuzz;  // TODO:
};

Metallic makeMetallic(vec3 albedo, float fuzz){
	Metallic metallic;

	metallic.albedo = albedo;
	metallic.fuzz = fuzz;

	return metallic;
}

// 电介质
struct Dielectric{
	vec3 albedo;
	float ior;
};

Dielectric makeDielectric(vec3 albedo, float ior){
	Dielectric dielectric;

	dielectric.albedo = albedo;
	dielectric.ior = ior;

	return dielectric;
}

Lambertian lambertMaterials[4];
Metallic metallicMaterials[4];
Dielectric dielectricMaterials[4];


// ---world
struct World {
    int object_count;
    Sphere objects[10];
};

World makeWorld() {
    lambertMaterials[0] = makeLambertian(vec3(0.1, 0.7, 0.7));
    lambertMaterials[1] = makeLambertian(vec3(0.5, 0.5, 0.5));

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

    world.object_count = 2;

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

    Camera camera = makeCamera(
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 2.0, 0.0),
            vec3(4.0, 0.0, 0.0),
            vec3(-2.0, -1.0, -1.0)
            );

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
