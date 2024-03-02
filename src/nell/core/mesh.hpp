#pragma once

#include <glm/glm.hpp>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "texture_manager.hpp"
#include "shader_utils/shader.hpp"

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

    class Mesh {
    private:
        unsigned int vao, vbo, ebo;

        // void setup();
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
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
        void load_model(std::string path);

        // transform a aiScene to std::vector<Mesh> in Model class
        void process_node(aiNode *node, const aiScene *scene);

        // transform a aiMesh to Mesh class
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string type_name);

    public:
        Model(char *path) {
            load_model(path);
        }

    };

    class GLMeshTexture {
    public:
        int indices_index; // GL_TEXTURE index
        int vertices_index; // GL_TEXTURE index
        unsigned int indices_id; // indices texture id
        unsigned int vertices_id; // vertices texture id
        int indices_size;
        int vertices_size;

        // TODO: generate opengl texture from mesh class
        GLMeshTexture(TextureManager &tm, const Model &model) {
            this->indices_index = tm.generate_texture();
            this->vertices_index = tm.generate_texture();

            std::cout << "[Nell][Debug]" << "GLMeshTexture::constructor" << ": "
                      << "vertices index: " << vertices_index << " "
                      << "indices index: " << indices_index << std::endl;
        }

        void setup(const Model &model, nell::Shader &shader);

    private:
        void model_to_texture(const Model &model, nell::Shader &shader);
    };

}