#pragma once

#include "mesh.hpp"
#include "camera.hpp"

namespace nell {
    // 1. mesh
    // 2. camera
    // 3. skybox
    // 4. width, height
    class Scene {
    public:
        int fnum, vnum;
        MeshData *mesh;
        Camera *camera;

        bool containSkybox;
        unsigned int skybox;

        int width, height;

        Scene();
        Scene(int vnum, int fnum, MeshData *mesh, Camera *camera, bool containSkybox, unsigned int skybox, int width, int height);
        void sync(unsigned int shaderid);

    public:
        static Scene CornellBoxChessScene();
        static Scene CornellBox();

    };


}