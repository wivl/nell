//
// Created by weevil on 2024/2/27.
//

#pragma once

#define MAIN_USED_INDEX 2

namespace nell {

    class TextureManager {
    public:
        int current_index;

        TextureManager();

        int generate_texture();

    };

} // nell

