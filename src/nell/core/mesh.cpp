#include "mesh.hpp"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

//void nell::Mesh::setup() {
//    glGenVertexArrays(1, &vao);
//    glGenBuffers(1, &vbo);
//    glGenBuffers(1, &ebo);
//
//    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//
//    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
//                 &indices[0], GL_STATIC_DRAW);
//
//    // 顶点位置
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//    // 顶点法线
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//    // 顶点纹理坐标
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
//
//    glBindVertexArray(0);
//}

nell::Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // setup();
}

void nell::Model::load_model(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    process_node(scene->mRootNode, scene);
}

void nell::Model::process_node(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

nell::Mesh nell::Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // load vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        vertex.position = v;

        if (mesh->HasNormals()) {
            v.x = mesh->mNormals[i].x;
            v.y = mesh->mNormals[i].y;
            v.z = mesh->mNormals[i].z;
            vertex.normal = v;
        }

        if (mesh->mTextureCoords[0]) {
            glm::vec2 v2;
            v2.x = mesh->mTextureCoords[0][i].x;
            v2.y = mesh->mTextureCoords[0][i].y;
            vertex.texcoord = v2;
            // TODO: ignore tangent space now since I don't need it in path tracing
        } else {
            vertex.texcoord = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // load indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    std::vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    // or aiTextureType_HEIGHT
    std::vector<Texture> normal_maps = load_material_textures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

    return Mesh(vertices, indices, textures);
}

unsigned int texture_from_file(const char *path, const std::string &directory) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    int width, height, nr_components;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nr_components, 0);
    if (data) {
        int format;
        if (nr_components == 1) {
            format = GL_RED;
        } else if (nr_components == 3) {
            format = GL_RGB;
        } else if (nr_components == 4) {
            format = GL_RGBA;
        } else {
            assert(false);
        }
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    stbi_image_free(data);
    return texture_id;
}

std::vector<nell::Texture> nell::Model::load_material_textures(aiMaterial *mat, aiTextureType type, std::string type_name) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = texture_from_file(str.C_Str(), this->directory);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}



void nell::GLMeshTexture::model_to_texture(const nell::Model &model, nell::Shader &shader) {
    int data_size_f = 0;
    int data_size_v = 0;

    std::cout << "[Nell][Debug]" << "GLMeshTexture::model_to_texture" << ": "
            << "mesh count: " << model.meshes.size() << std::endl;

    for (int i = 0; i < model.meshes.size(); i++) {
        data_size_f += model.meshes[i].indices.size();
        data_size_v += model.meshes[i].vertices.size();
    }
    std::cout << "[Nell][Debug]" << "GLMeshTexture::model_to_texture" << ": "
            << "data_size_f: " << data_size_f << " "
            << "data_size_v: " << data_size_v << std::endl;

    float vert_x_f = sqrtf(data_size_v);
    int vert_x = ceilf(vert_x_f);
    int vert_y = ceilf((float)data_size_v / (float)vert_x);
    float face_x_f = sqrtf(data_size_f);
    int face_x = ceilf(face_x_f);
    int face_y = ceilf((float)data_size_f / (float)face_x);

    float *vertex_array = new float[(3+3+2) * (vert_x*vert_y)];
    float *face_array = new float[1 * (face_x*face_y)];

    assert(vertex_array != nullptr);
    assert(face_array != nullptr);

    int v_index = 0;
    int f_index = 0;

    for (int i = 0; i < model.meshes.size(); i++) {
        for (int j = 0; j < model.meshes[i].vertices.size(); j++) {

            vertex_array[v_index * (8) + 0] = (float)model.meshes[i].vertices[j].position.x;
            vertex_array[v_index * (8) + 1] = (float)model.meshes[i].vertices[j].position.y;
            vertex_array[v_index * (8) + 2] = (float)model.meshes[i].vertices[j].position.z;

            vertex_array[v_index * (8) + 3] = (float)model.meshes[i].vertices[j].normal.x;
            vertex_array[v_index * (8) + 4] = (float)model.meshes[i].vertices[j].normal.y;
            vertex_array[v_index * (8) + 5] = (float)model.meshes[i].vertices[j].normal.z;

            vertex_array[v_index * (8) + 6] = (float)model.meshes[i].vertices[j].texcoord.x;
            vertex_array[v_index * (8) + 7] = (float)model.meshes[i].vertices[j].texcoord.y;

            v_index++;
        }

        for (int j = 0; j < model.meshes[i].indices.size(); j++) {
            face_array[f_index] = (float)model.meshes[i].indices[j];
            f_index++;
        }

        this->indices_size = data_size_f;
        this->vertices_size = data_size_v;

        shader.use();

        glGenTextures(1, &vertices_id);
        glBindTexture(GL_TEXTURE_2D, vertices_id);
        glTexImage2D(GL_TEXTURE, 0, GL_R32F, vert_x * 8, vert_y, 0, GL_RED, GL_FLOAT, vertex_array);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

//        glUniform1i(glGetUniformLocation(shader.id, "vertices"), 2);
//        glUniform1i(glGetUniformLocation(shader.id, "vertices_num"), vertices_size);

        glGenTextures(1, &indices_id);
        glBindTexture(GL_TEXTURE_2D, indices_id);
        glTexImage2D(GL_TEXTURE, 0, GL_R32F, face_x, face_y, 0, GL_RED, GL_FLOAT, face_array);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

//        glUniform1i(glGetUniformLocation(shader.id, "indices"), 3);
//        glUniform1i(glGetUniformLocation(shader.id, "indices_num"), indices_size);

        delete[] vertex_array;
        delete[] face_array;
    }


}

void nell::GLMeshTexture::setup(const nell::Model &model, nell::Shader &shader) {
    model_to_texture(model, shader);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->vertices_id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->indices_id);

    glUniform1i(glGetUniformLocation(shader.id, "vertices"), 2);
    glUniform1i(glGetUniformLocation(shader.id, "vertices_num"), vertices_size);

    glUniform1i(glGetUniformLocation(shader.id, "indices"), 3);
    glUniform1i(glGetUniformLocation(shader.id, "indices_num"), indices_size);
}
