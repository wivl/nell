#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "shader.hpp"

namespace nell {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };


#define MAX_VERTEX_NUM 500000
#define MAX_FACE_NUM 100000
#define MAX_MATERIAL_NUM 50

    struct MeshData {
        glm::vec4 vertices[MAX_VERTEX_NUM];
        glm::vec4 normals[MAX_VERTEX_NUM];

        glm::ivec4 faces[MAX_FACE_NUM];

        // assigned in scene
        float materials[MAX_MATERIAL_NUM];
        int materialPtrs[MAX_FACE_NUM];
    };

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<int> indices, std::vector<Texture> textures);
    };

    // one model can contain many meshes
    class Model {
    public:
        std::vector<Mesh> meshes;

        // all the shared textures needed in this model
        std::vector<Texture> textures_loaded;

        // where the model file is located
        std::string directory;

        // load model from file, and
        void load(const std::string& path);

        // transform a aiScene to std::vector<Mesh> in Model class
        void processNode(aiNode *node, const aiScene *scene);

        // transform a aiMesh to Mesh class
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string type_name);

    public:
        explicit Model() {}
        explicit Model(const char *path) {
            load(path);
        }

        MeshData* generateMeshData(int &vnum, int &fnum);

    };



}