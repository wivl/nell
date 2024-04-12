cmake_minimum_required(VERSION 3.10)

project(nellpt)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        src/utils/*.cpp
        dependencies/imgui/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        src/*.hpp
        src/utils/*.hpp
        dependencies/imgui/*.h
)




add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

include(C:/Users/Weevil/scoop/apps/vcpkg/current/scripts/buildsystems/vcpkg.cmake)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm::glm)

find_package(Stb REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE assimp::assimp)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui::imgui)



# if you are using neovim with coc-clangd, than you should generate a file using 
# ```
# cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# ```
# in your build directery to avoid error message by coc-clangd.
