cmake_minimum_required(VERSION 3.10)

project(nellpt)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        src/nell/core/*.cpp
        src/nell/core/shader_utils/*.cpp
        # stb
        ${CMAKE_SOURCE_DIR}/dependencies/stb/*.cpp
        )

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        src/*.hpp
        src/nell/core/*.hpp
        src/nell/core/shader_utils/*.hpp
        # stb
        ${CMAKE_SOURCE_DIR}/dependencies/stb/*.h
        )


# DEPENDENCIES
include_directories(${CMAKE_SOURCE_DIR}/dependencies/stb)
#add_library(stb STATIC dependencies/stb/stb_image.cpp)
#target_include_directories(stb PUBLIC dependencies/stb)

add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/assimp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})


target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glm)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glad)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC assimp)




# if you are using neovim with coc-clangd, than you should generate a file using 
# ```
# cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# ```
# in your build directery to avoid error message by coc-clangd.
