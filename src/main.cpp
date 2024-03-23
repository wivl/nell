#include <iostream>

#include "../dependencies/imgui/imgui.h"
#include "../dependencies/imgui/imgui_impl_glfw.h"
#include "../dependencies/imgui/imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb_image_write.h>
#include <stb_image.h>


#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "shader.hpp"
#include "utils/random.hpp"
#include "camera.hpp"
#include "mesh.hpp"


void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, nell::Camera &camera, unsigned int shaderid);
unsigned int loadImage(char const* path);
void cornellBoxilize(nell::MeshData *mesh, int &vnum, int &fnum);


#define WIDTH 1600
#define HEIGHT 800


const float Material_Lambertian = 0.0;
const float Material_Metal = 1.0;
const float Material_Dielectric = 2.0;

#define SPEED 0.5 // speed of camera move

float firstClick = true;

float sensitivity = 40.0;




int main() {

    // Create OpenGL and glfw app
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "nellpt", NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }




    // Create window

    float vertices[] = {
            1.0f, 1.0f, 1.0, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
            0, 1, 3, 1, 2, 3,
    };

    unsigned int vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *) (2 * sizeof(float)));


    // Shader

    nell::Shader ptShader("../src/shader/default.vert", "../src/shader/pathtrace.frag");


    // Camera

    vec2 screen_size = vec2(WIDTH, HEIGHT);

    float aspect_ratio = screen_size.x / screen_size.y;
    vec3 position = vec3(0, 2, 15);
    vec3 direction = vec3(0, 0, -1);
    float focusLength = 1.0;

    nell::Camera camera(position, direction, 90, aspect_ratio, focusLength);
    camera.updateAndSync(ptShader.id);

#define materialArraySize 14

    const float materials[materialArraySize] {
        Material_Lambertian, 0.8, 0.8, 0.0,
        Material_Metal, 0.1, 0.2, 0.5, 0.0,
        Material_Dielectric, 1.0, 1.0, 1.0, 1.5
    };

    nell::Model model = nell::Model("./chess_scene.obj");
    int fnum = 0;
    int vnum = 0;

    nell::MeshData* mesh = model.generateMeshData(vnum, fnum);

    cornellBoxilize(mesh, vnum, fnum);

    unsigned int skybox = loadImage("./vestibule_8k.hdr");



    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(nell::MeshData), mesh, GL_STATIC_DRAW);

    GLuint bindingPoint = 0;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



    // Render loop

    nell::CPURandomInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (!io.WantCaptureMouse) {
            processInput(window, camera, ptShader.id);
        }

        float randOrigin = 674764.0f * (nell::GetCPURandom() + 1.0f);
        auto time = static_cast<float>(glfwGetTime());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skybox);
        glUniform1i(glGetUniformLocation(ptShader.id, "skybox"), 0);



        glUniform1i(glGetUniformLocation(ptShader.id, "width"), WIDTH);
        glUniform1i(glGetUniformLocation(ptShader.id, "height"), HEIGHT);
        glUniform1f(glGetUniformLocation(ptShader.id, "randOrigin"), randOrigin);
        glUniform1f(glGetUniformLocation(ptShader.id, "time"), time);
        glUniform1i(glGetUniformLocation(ptShader.id, "faceCount"), fnum);

        glUniform1i(glGetUniformLocation(ptShader.id, "materialArraySize"), materialArraySize);
        glUniform1fv(glGetUniformLocation(ptShader.id, "materials"), materialArraySize, materials);


        ptShader.use();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




        ImGui::Begin("Camera: ");
        ImGui::Text("%s", (std::string("Position: ") +
            std::to_string(camera.position.x) + std::string(" ") +
            std::to_string(camera.position.y) + std::string(" ") +
            std::to_string(camera.position.z)
                    ).c_str());
        ImGui::Text("%s", (std::string("Direction: ") +
                     std::to_string(camera.direction.x) + std::string(" ") +
                     std::to_string(camera.direction.y) + std::string(" ") +
                     std::to_string(camera.direction.z)
                    ).c_str());
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}


void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, nell::Camera &camera, unsigned int shaderid) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // TODO: Camera update
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.moveForward(static_cast<float>(SPEED));
        camera.update();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveBackward(static_cast<float>(SPEED));
        camera.update();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.moveLeft(static_cast<float>(SPEED));
        camera.update();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.moveRight(static_cast<float>(SPEED));
        camera.update();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.moveUp(static_cast<float>(SPEED));
        camera.update();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.moveDown(static_cast<float>(SPEED));
        camera.update();
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        // Hides mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        // Prevents camera from jumping on the first click
        if (firstClick)
        {
            glfwSetCursorPos(window, (WIDTH / 2), (HEIGHT / 2));
            firstClick = false;
        }

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;
        // Fetches the coordinates of the cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
        // and then "transforms" them into degrees
        float rotX = sensitivity * (float)(mouseY - (HEIGHT / 2)) / HEIGHT;
        float rotY = sensitivity * (float)(mouseX - (WIDTH / 2)) / WIDTH;

        // Calculates upcoming vertical change in the Orientation
        glm::vec3 newOrientation = glm::rotate(camera.direction, glm::radians(-rotX), glm::normalize(glm::cross(camera.direction, vec3(0, 1, 0))));

        // Decides whether or not the next vertical Orientation is legal or not
        if (abs(glm::angle(newOrientation, vec3(0, 1, 0)) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            camera.direction = newOrientation;
        }

        // Rotates the Orientation left and right
        camera.direction = glm::rotate(camera.direction, glm::radians(-rotY), vec3(0, 1, 0));

        // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
        glfwSetCursorPos(window, (WIDTH / 2), (HEIGHT / 2));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
    camera.updateAndSync(shaderid);
}

unsigned int loadImage(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        GLenum iformat;
        if (nrComponents == 1) {
            format = GL_RED;
            iformat = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            iformat = GL_RGB;
        }
        else {
            format = GL_RGBA;
            iformat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "[Nell][Debug]Main::loadImage: Unable to laod "<< path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void cornellBoxilize(nell::MeshData *mesh, int &vnum, int &fnum) {
    glm::vec3 vert[20] = {
            // 0
            glm::vec3(-5, 10, 5),
            glm::vec3(-5, 0, -5),
            glm::vec3(-5, 10, -5),
            // 1
            glm::vec3(-5, 0, 5),
            // 2
            glm::vec3(-5, 10, 5),
            glm::vec3(-5, 10, -5),
            glm::vec3(5, 10, -5),
            // 3
            glm::vec3(5, 10, 5),
            // 4
            glm::vec3(5, 10, 5),
            glm::vec3(5, 10, -5),
            glm::vec3(5, 0, -5),
            // 5
            glm::vec3(5, 0, 5),
            // 6
            glm::vec3(5, 0, 5),
            glm::vec3(5, 0, -5),
            glm::vec3(-5, 0, -5),
            // 7
            glm::vec3(-5, 0, 5),
            // 8
            glm::vec3(-5, 0, -5),
            glm::vec3(5, 0, -5),
            glm::vec3(-5, 10, -5),
            // 9
            glm::vec3(5, 10, -5),
    };

    glm::ivec3 face[10] = {
            glm::ivec3(vnum, vnum+1, vnum+2),
            glm::ivec3(vnum, vnum+3, vnum+1),
            glm::ivec3(vnum+4, vnum+5, vnum+6),
            glm::ivec3(vnum+4, vnum+6, vnum+7),
            glm::ivec3(vnum+8, vnum+9, vnum+10),
            glm::ivec3(vnum+8, vnum+10, vnum+11),
            glm::ivec3(vnum+12, vnum+13, vnum+14),
            glm::ivec3(vnum+12, vnum+14, vnum+15),
            glm::ivec3(vnum+16, vnum+17, vnum+18),
            glm::ivec3(vnum+17, vnum+18, vnum+19),
    };
    glm::vec3 normal[20] = {
            // 0
            glm::vec3(1, 0, 0),
            glm::vec3(1, 0, 0),
            glm::vec3(1, 0, 0),
            // 1
            glm::vec3(1, 0, 0),
            // 2
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            // 3
            glm::vec3(0, -1, 0),
            // 4
            glm::vec3(-1, 0, 0),
            glm::vec3(-1, 0, 0),
            glm::vec3(-1, 0, 0),
            // 5
            glm::vec3(-1, 0, 0),
            // 6
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            // 7
            glm::vec3(0, 1, 0),
            // 8
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 1),
            // 9
            glm::vec3(0, 0, 1),
    };

    if (vnum + 20 >= MAX_VERTEX_NUM) {
        std::cout << "[Nell][Debug]" << "cornellBoxilize: Too many vertices: " << vnum + 20 << std::endl;
        return ;
    }
    if (fnum + 10 >= MAX_FACE_NUM) {
        std::cout << "[Nell][Debug]" << "cornellBoxilize: Too many faces: " << fnum + 10 << std::endl;
        return ;
    }
    for (int i = 0; i < 20; i++) {
        mesh->vertices[vnum + i] = glm::vec4(vert[i], 0);
    }
    for (int i = 0; i < 20; i++) {
        mesh->normals[vnum + i] = glm::vec4(normal[i], 0);
    }
    for (int i = 0; i < 10; i++) {
        mesh->faces[fnum + i] = glm::ivec4(face[i], 0);
    }
    vnum += 20;
    fnum += 10;
    std::cout << "[Nell][Debug]" << "cornellBoxilize" << ": "
              << "vertex count: " << vnum << " "
              << "face count: " << fnum << std::endl;
}
