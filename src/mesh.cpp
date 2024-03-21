#include "mesh.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <iostream>


nell::Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // setup();
}

void nell::Model::load(const std::string& path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate
    | aiProcess_FlipUVs
    | aiProcess_JoinIdenticalVertices
    | aiProcess_GenNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void nell::Model::processNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

nell::Mesh nell::Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
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
    std::vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<Texture> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    // or aiTextureType_HEIGHT
    std::vector<Texture> normal_maps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

    return Mesh(vertices, indices, textures);
}

unsigned int textureFromFile(const char *path, const std::string &directory) {
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

std::vector<nell::Texture> nell::Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string type_name) {
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
            texture.id = textureFromFile(str.C_Str(), this->directory);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

nell::MeshData *nell::Model::generateMeshData(int &vnum, int &fnum) {
    std::cout << "[Nell][Debug]" << "Model::generateMeshData" << ": "
              << "mesh count: " << this->meshes.size() << std::endl;

    vnum = 0;
    fnum = 0;
    for (int i = 0; i < this->meshes.size(); i++) {
        vnum += this->meshes[i].vertices.size();
        fnum += this->meshes[i].indices.size();
    }

    fnum /= 3;

    std::cout << "[Nell][Debug]" << "Model::generateMeshData" << ": "
              << "vertex count: " << vnum << " "
              << "face count: " << fnum << std::endl;

    if (vnum > MAX_VERTEX_NUM) {
        std::cout << "[Nell][Error]" << "Model::generateMeshData" << ": "
        << "Too many vertices: " << vnum;
        assert(false);
    }
    if (fnum > MAX_FACE_NUM) {
        std::cout << "[Nell][Error]" << "Model::generateMeshData" << ": "
                  << "Too many faces: " << fnum;
        assert(false);
    }

    auto *data = new MeshData;
    int findex = 0;
    int vindex = 0;

    for (int i = 0; i < this->meshes.size(); i++) {
        for (int j = 0; j < this->meshes[i].vertices.size(); j++) {
            data->vertices[vindex] = glm::vec4(this->meshes[i].vertices[j].position, 0);
            data->normals[vindex] = glm::vec4(this->meshes[i].vertices[j].normal, 0);
            vindex++;
        }

        for (int j = 0; j < this->meshes[i].indices.size(); j+=3) {
            data->faces[findex] = glm::ivec4(this->meshes[i].indices[j], this->meshes[i].indices[j+1], this->meshes[i].indices[j+2], 0);
            findex++;
        }
    }

    return data;
}
