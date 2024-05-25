#include "scene.hpp"

#include "material.hpp"
#include "image.hpp"


void cornellBoxilize(nell::MeshData *mesh, int &vnum, int &fnum) {
    glm::vec3 vert[24] = {
            // 0
            glm::vec3(-5, 10, 5),
            glm::vec3(-5, 0, -5),
            glm::vec3(-5, 10, -5),
            // 1
            glm::vec3(-5, 0, 5),
            // 2
            glm::vec3(-5, 10, 5),
            glm::vec3(-5, 10, -5),
            glm::vec3(5, 10, -5),
            // 3
            glm::vec3(5, 10, 5),
            // 4
            glm::vec3(5, 10, 5),
            glm::vec3(5, 10, -5),
            glm::vec3(5, 0, -5),
            // 5
            glm::vec3(5, 0, 5),
            // 6
            glm::vec3(5, 0, 5),
            glm::vec3(5, 0, -5),
            glm::vec3(-5, 0, -5),
            // 7
            glm::vec3(-5, 0, 5),
            // 8
            glm::vec3(-5, 0, -5),
            glm::vec3(5, 0, -5),
            glm::vec3(-5, 10, -5),
            // 9
            glm::vec3(5, 10, -5),
            // 10, light
            glm::vec3(-3, 9.8, 3),
            glm::vec3(-3, 9.8, -3),
            glm::vec3(3, 9.8, -3),
            // 11, light
            glm::vec3(3, 9.8, 3),

    };

    glm::ivec3 face[12] = {
            glm::ivec3(vnum, vnum+1, vnum+2),
            glm::ivec3(vnum, vnum+3, vnum+1),
            glm::ivec3(vnum+4, vnum+5, vnum+6),
            glm::ivec3(vnum+4, vnum+6, vnum+7),
            glm::ivec3(vnum+8, vnum+9, vnum+10),
            glm::ivec3(vnum+8, vnum+10, vnum+11),
            glm::ivec3(vnum+12, vnum+13, vnum+14),
            glm::ivec3(vnum+12, vnum+14, vnum+15),
            glm::ivec3(vnum+16, vnum+17, vnum+18),
            glm::ivec3(vnum+17, vnum+18, vnum+19),
            // light
            glm::ivec3(vnum+20, vnum+21, vnum+22),
            glm::ivec3(vnum+20, vnum+22, vnum+23),

    };
    glm::vec3 normal[24] = {
            // 0
            glm::vec3(1, 0, 0),
            glm::vec3(1, 0, 0),
            glm::vec3(1, 0, 0),
            // 1
            glm::vec3(1, 0, 0),
            // 2
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            // 3
            glm::vec3(0, -1, 0),
            // 4
            glm::vec3(-1, 0, 0),
            glm::vec3(-1, 0, 0),
            glm::vec3(-1, 0, 0),
            // 5
            glm::vec3(-1, 0, 0),
            // 6
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            // 7
            glm::vec3(0, 1, 0),
            // 8
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 1),
            // 9
            glm::vec3(0, 0, 1),
            // 10, light
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            // 11, light
            glm::vec3(0, -1, 0),
    };


    if (vnum + 24 >= MAX_VERTEX_NUM) {
        std::cout << "[Nell][Debug]" << "cornellBoxilize: Too many vertices: " << vnum + 24 << std::endl;
        return ;
    }
    if (fnum + 12 >= MAX_FACE_NUM) {
        std::cout << "[Nell][Debug]" << "cornellBoxilize: Too many faces: " << fnum + 12 << std::endl;
        return ;
    }
    for (int i = 0; i < 24; i++) {
        mesh->vertices[vnum + i] = glm::vec4(vert[i], 0);
    }
    for (int i = 0; i < 24; i++) {
        mesh->normals[vnum + i] = glm::vec4(normal[i], 0);
    }
    for (int i = 0; i < 12; i++) {
        mesh->faces[fnum + i] = glm::ivec4(face[i], 0);
    }
    vnum += 24;
    fnum += 12;
    std::cout << "[Nell][Debug]" << "cornellBoxilize" << ": "
              << "vertex count: " << vnum << " "
              << "face count: " << fnum << std::endl;
}


void flat(nell::MeshData *mesh, int &vnum, int &fnum) {
    glm::vec3 vert[4] = {
            glm::vec3(50, 0, 50),
            glm::vec3(50, 0, -50),
            glm::vec3(-50, 0, -50),
            glm::vec3(-50, 0, 50),
    };

    glm::ivec3 face[2] = {
            glm::ivec3(vnum, vnum+1, vnum+2),
            glm::ivec3(vnum, vnum+2, vnum+3),

    };
    glm::vec3 normal[4] = {
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
    };


    if (vnum + 4 >= MAX_VERTEX_NUM) {
        std::cout << "[Nell][Debug]" << "cornellBoxilize: Too many vertices: " << vnum + 24 << std::endl;
        return ;
    }
    if (fnum + 2 >= MAX_FACE_NUM) {
        std::cout << "[Nell][Debug]" << "cornellBoxilize: Too many faces: " << fnum + 12 << std::endl;
        return ;
    }
    for (int i = 0; i < 4; i++) {
        mesh->vertices[vnum + i] = glm::vec4(vert[i], 0);
    }
    for (int i = 0; i < 4; i++) {
        mesh->normals[vnum + i] = glm::vec4(normal[i], 0);
    }
    for (int i = 0; i < 2; i++) {
        mesh->faces[fnum + i] = glm::ivec4(face[i], 0);
    }
    vnum += 4;
    fnum += 2;
    std::cout << "[Nell][Debug]" << "cornellBoxilize" << ": "
              << "vertex count: " << vnum << " "
              << "face count: " << fnum << std::endl;
}


nell::Scene::Scene() {
    fnum = 0;
    vnum = 0;
    mesh = nullptr;
    camera = nullptr;
    containSkybox = false;
    skybox = -1;
    width = 0;
    height = 0;
}

nell::Scene::Scene(int vnum, int fnum, nell::MeshData *mesh, nell::Camera *camera, bool containSkybox, unsigned int skybox, int width,
                   int height) {
    this->vnum = vnum;
    this->fnum = fnum;
    this->mesh = mesh;
    this->camera = camera;
    this->containSkybox = containSkybox;
    this->skybox = skybox;
    this->width = width;
    this->height = height;
}

void nell::Scene::sync(unsigned int shaderid) {
    if (containSkybox) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, skybox);
        glUniform1i(glGetUniformLocation(shaderid, "skybox"), 2);
    }

    glUniform1i(glGetUniformLocation(shaderid, "containSkybox"), containSkybox ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderid, "width"), width);
    glUniform1i(glGetUniformLocation(shaderid, "height"), height);
    glUniform1i(glGetUniformLocation(shaderid, "faceCount"), fnum);
}
#define materialArraySize 39

const float materials[materialArraySize] {
        Material_Lambertian, 0.8, 0.8, 0.0,
        Material_Metal, 0.73, 0.73, 0.73, 0.0,
        Material_Dielectric, 1.0, 1.0, 1.0, 1.5,
        Material_Lambertian, 0.570068, 0.0430135, 0.0443706, // red
        Material_Lambertian, 0.885809, 0.698859, 0.666422, // white
        Material_Lambertian, 0.105421, 0.37798, 0.076425, // green
        Material_Emit, 14, 14, 14, // TODO:
        Material_Emit, 18.387, 13.9873, 6.75357, // TODO:
        Material_Lambertian, 1, 1, 1, // white
        Material_Chessboard,

};

nell::Scene nell::Scene::CornellBoxChessScene() {

    nell::Model model = nell::Model("../assets/chess_scene.obj");
    int vnum = 0, fnum = 0;
    nell::MeshData *mesh = model.generateMeshData(vnum, fnum);
    cornellBoxilize(mesh, vnum, fnum);

    bool containSkybox = false;
//    unsigned int skybox = nell::loadImage("../assets/vestibule_8k.hdr");
    // material
    for (int i = 0; i < materialArraySize; i++) {
        mesh->materials[i] = materials[i];
    }

    for (int i = 0; i < 456; i++) {
        mesh->materialPtrs[i] = 18;
    }
    for (int i = 456; i < 1038; i++) {
        mesh->materialPtrs[i] = 18;
    }
    for (int i = 1038; i < 1396; i++) {
        mesh->materialPtrs[i] = 9;
    }
    for (int i = 1396; i < 1862; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // red left wall
    for (int i = 1862; i < 1864; i++) {
        mesh->materialPtrs[i] = 14;
    }
    // white top wall
    for (int i = 1864; i < 1866; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // green right wall
    for (int i = 1866; i < 1868; i++) {
        mesh->materialPtrs[i] = 22;
    }
    // white bottom and back wall
    for (int i = 1868; i < 1872; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // light
    for (int i = 1872; i < fnum; i++) {
        mesh->materialPtrs[i] = 26;
    }
    int width = 800;
    int height = 800;

    vec2 screen_size = vec2(width, height);
    float aspect_ratio = screen_size.x / screen_size.y;
//    vec3 position = vec3(0, 5, 10.5);
    vec3 position = vec3(0, 5, 20);
    vec3 direction = vec3(0, 0, -1);
    float focusLength = 1.0;

    nell::Camera *camera = new nell::Camera(position, direction, 40, aspect_ratio, focusLength);


    return Scene(vnum, fnum, mesh, camera, containSkybox, -1, width, height);
}

nell::Scene nell::Scene::CornellBox() {

    nell::Model model = nell::Model("../assets/cornell-box.obj");
    int vnum = 0, fnum = 0;
    nell::MeshData *mesh = model.generateMeshData(vnum, fnum);

    bool containSkybox = false;
//    unsigned int skybox = nell::loadImage("../assets/vestibule_8k.hdr");
    // material
    for (int i = 0; i < materialArraySize; i++) {
        mesh->materials[i] = materials[i];
    }

    for (int i = 0; i < fnum; i++) {
        mesh->materialPtrs[i] = 0;
    }
    // back wall
    for (int i = 0; i < 2; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // tall box
    for (int i = 2; i < 12; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // short box
    for (int i = 12; i < 22; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // right green wall
    for (int i = 22; i < 24; i++) {
        mesh->materialPtrs[i] = 22;
    }
    // left red wall
    for (int i = 24; i < 26; i++) {
        mesh->materialPtrs[i] = 14;
    }
    // floor
    for (int i = 26; i < 28; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // ceiling
    for (int i = 28; i < 30; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // light
    for (int i = 30; i < 32; i++) {
        mesh->materialPtrs[i] = 30;
    }

    int width = 800;
    int height = 800;

    vec2 screen_size = vec2(width, height);
    float aspect_ratio = screen_size.x / screen_size.y;
//    vec3 position = vec3(0, 5, 10.5);
//    light center (-0.234011, -3.0429785)
    vec3 position = vec3(-0.218011, 2.5813135, 8);
    vec3 direction = vec3(0, 0, -1);
    float focusLength = 1.0;

    nell::Camera *camera = new nell::Camera(position, direction, 40, aspect_ratio, focusLength);


    return Scene(vnum, fnum, mesh, camera, containSkybox, -1, width, height);
}

nell::Scene nell::Scene::MitsubaCbox() {

    nell::Model model = nell::Model();
    int vnum = 0, fnum = 0;
    nell::MeshData *mesh = model.generateMeshData(vnum, fnum);
    cornellBoxilize(mesh, vnum, fnum);

    bool containSkybox = false;
//    unsigned int skybox = nell::loadImage("../assets/vestibule_8k.hdr");
    // material
    for (int i = 0; i < materialArraySize; i++) {
        mesh->materials[i] = materials[i];
    }


    // green left wall
    for (int i = 1862-1862; i < 1864-1862; i++) {
        mesh->materialPtrs[i] = 22;
    }
    // white top wall
    for (int i = 1864-1862; i < 1866-1862; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // red right wall
    for (int i = 1866-1862; i < 1868-1862; i++) {
        mesh->materialPtrs[i] = 14;
    }
    // white bottom and back wall
    for (int i = 1868-1862; i < 1872-1862; i++) {
        mesh->materialPtrs[i] = 18;
    }
    // light
    for (int i = 1872-1862; i < fnum; i++) {
        mesh->materialPtrs[i] = 30;
    }
    int width = 800;
    int height = 800;

    vec2 screen_size = vec2(width, height);
    float aspect_ratio = screen_size.x / screen_size.y;
//    vec3 position = vec3(0, 5, 10.5);
    vec3 position = vec3(0, 5, 19.6);
    vec3 direction = vec3(0, 0, -1);
    float focusLength = 1.0;

    nell::Camera *camera = new nell::Camera(position, direction, 40, aspect_ratio, focusLength);


    return Scene(vnum, fnum, mesh, camera, containSkybox, -1, width, height);
}

nell::Scene nell::Scene::MaterialShow() {
    nell::Model model = nell::Model("../assets/suzanne.obj");
    int vnum = 0, fnum = 0;
    nell::MeshData *mesh = model.generateMeshData(vnum, fnum);
    flat(mesh, vnum, fnum);

    bool containSkybox = true;
    unsigned int skybox = nell::loadImage("../assets/lake_pier_8k.hdr");
    // material
    for (int i = 0; i < materialArraySize; i++) {
        mesh->materials[i] = materials[i];
    }

    for (int i = 0; i < 968; i++) {
        mesh->materialPtrs[i] = 9;
    }
    for (int i = 968; i < fnum; i++) {
        mesh->materialPtrs[i] = 38;
    }

    int width = 800;
    int height = 800;

    vec2 screen_size = vec2(width, height);
    float aspect_ratio = screen_size.x / screen_size.y;
//    vec3 position = vec3(0, 5, 10.5);
    vec3 position = vec3(-0.52, 1.6, 9.79);
    vec3 direction = vec3(-0.33, -0.08, -0.93);
    float focusLength = 1.0;

    nell::Camera *camera = new nell::Camera(position, direction, 40, aspect_ratio, focusLength);


    return Scene(vnum, fnum, mesh, camera, containSkybox, skybox, width, height);
}

nell::Scene nell::Scene::Bunny() {
    nell::Model model = nell::Model("../assets/stanford-bunny.obj");
    int vnum = 0, fnum = 0;
    nell::MeshData *mesh = model.generateMeshData(vnum, fnum);
//    flat(mesh, vnum, fnum);

    bool containSkybox = true;
    unsigned int skybox = nell::loadImage("../assets/monte_scherbelino_4k.hdr");
    // material
    for (int i = 0; i < materialArraySize; i++) {
        mesh->materials[i] = materials[i];
    }

    for (int i = 0; i < fnum; i++) {
        mesh->materialPtrs[i] = 34;
    }

    int width = 800;
    int height = 800;

    vec2 screen_size = vec2(width, height);
    float aspect_ratio = screen_size.x / screen_size.y;
//    vec3 position = vec3(0, 5, 10.5);
    vec3 position = vec3(0, 0.1, 0.5);
    vec3 direction = vec3(-0.03, 0, -0.99);
    float focusLength = 1.0;

    nell::Camera *camera = new nell::Camera(position, direction, 30, aspect_ratio, focusLength);


    return Scene(vnum, fnum, mesh, camera, containSkybox, skybox, width, height);

}


nell::Scene nell::Scene::Cube() {
    nell::Model model = nell::Model("../assets/cube.obj");
    int vnum = 0, fnum = 0;
    nell::MeshData *mesh = model.generateMeshData(vnum, fnum);
//    flat(mesh, vnum, fnum);

    bool containSkybox = true;
    unsigned int skybox = nell::loadImage("../assets/lake_pier_8k.hdr");
    // material
    for (int i = 0; i < materialArraySize; i++) {
        mesh->materials[i] = materials[i];
    }

    for (int i = 0; i < fnum; i++) {
        mesh->materialPtrs[i] = 9;
    }

    int width = 800;
    int height = 800;

    vec2 screen_size = vec2(width, height);
    float aspect_ratio = screen_size.x / screen_size.y;
//    vec3 position = vec3(0, 5, 10.5);
    vec3 position = vec3(-0.52, 1.6, 9.79);
    vec3 direction = vec3(-0.33, -0.08, -0.93);
    float focusLength = 1.0;

    nell::Camera *camera = new nell::Camera(position, direction, 40, aspect_ratio, focusLength);


    return Scene(vnum, fnum, mesh, camera, containSkybox, skybox, width, height);
}
