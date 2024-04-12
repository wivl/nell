#pragma once

#include "glad/glad.h"
#include <string>

namespace nell {
    class Shader {
    public:
        unsigned int id;

        Shader(const char *vertex_path, const char *fragment_path);
        void use();
        void set_bool(const std::string &name, bool value) const;
        void set_int(const std::string &name, int value) const;
        void set_float(const std::string &name, float value) const;

        void set_vec4(const std::string &name, float x, float y, float z, float w) const;
    };
}