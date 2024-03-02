//
// Created by weevil on 2024/2/27.
//

#include "texture_manager.hpp"


namespace nell {
    int TextureManager::generate_texture() {
        return current_index++;
    }

    TextureManager::TextureManager() {
        current_index = MAIN_USED_INDEX;
    }
} // nell