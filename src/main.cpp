#include <iostream>
#include <thread>

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
#include "image.hpp"
#include "scene.hpp"


void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, nell::Camera *camera, unsigned int shaderid);


#define WIDTH 800
#define HEIGHT 800




#define SPEED 0.5 // speed of camera move
float firstClick = true;
float sensitivity = 40.0;



bool cameraMove = false;

int main() {

    // Create OpenGL and glfw app
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
            1.0f, 1.0f, 1.0, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,
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
    nell::Shader accShader("../src/shader/default.vert", "../src/shader/accumulate.frag");
    nell::Shader previewShader("../src/shader/default.vert", "../src/shader/preview.frag");


    nell::Scene scene = nell::Scene::CornellBoxChessScene();

    scene.camera->updateAndSync(ptShader.id);







    GLuint meshSSBO;
    glGenBuffers(1, &meshSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(nell::MeshData), scene.mesh, GL_STATIC_DRAW);

    GLuint bindingPoint = 0;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, meshSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    unsigned int ptTexture, ptFramebuffer;
    glGenTextures(1, &ptTexture);
    glBindTexture(GL_TEXTURE_2D, ptTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &ptFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ptFramebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           ptTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
    }


    unsigned int accTexture, accFramebuffer;
    glGenTextures(1, &accTexture);
    glBindTexture(GL_TEXTURE_2D, accTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &accFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, accFramebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           accTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ptTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, accTexture);

    glUniform1i(glGetUniformLocation(accShader.id, "currentFrame"), 0);
    glUniform1i(glGetUniformLocation(accShader.id, "accFrame"), 1);
    glUniform1i(glGetUniformLocation(previewShader.id, "frame"), 1);


    // Render loop

    nell::CPURandomInit();

//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO &io = ImGui::GetIO(); (void) io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 460");

    int loop = 0;
    while (!glfwWindowShouldClose(window)) {

        glBindFramebuffer(GL_FRAMEBUFFER, ptFramebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//        if (!io.WantCaptureMouse) {
            processInput(window, scene.camera, ptShader.id);
//        }
        if (cameraMove) {
            loop = 0;
            cameraMove = false;
        }

        float randOrigin = 674764.0f * (nell::GetCPURandom() + 1.0f);
        auto time = static_cast<float>(glfwGetTime());

        ptShader.use();

        scene.camera->updateAndSync(ptShader.id);
        scene.sync(ptShader.id);
        glUniform1f(glGetUniformLocation(ptShader.id, "randOrigin"), randOrigin);
        glUniform1f(glGetUniformLocation(ptShader.id, "time"), time);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        glBindFramebuffer(GL_FRAMEBUFFER, accFramebuffer);
        accShader.use();
        glUniform1i(glGetUniformLocation(accShader.id, "accFrame"), 1);
        glUniform1i(glGetUniformLocation(accShader.id, "currentFrame"), 0);
        glUniform1i(glGetUniformLocation(accShader.id, "loop"), loop);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        previewShader.use();
        glUniform1i(glGetUniformLocation(previewShader.id, "frame"), 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


//        ImGui::Begin("Info: ");
//        ImGui::Text("%s", (std::string("Frame: ") +
//                           std::to_string(loop+1)
//        ).c_str());
//        ImGui::Text("%s", (std::string("Camera Position: ") +
//            std::to_string(scene.camera->position.x) + std::string(" ") +
//            std::to_string(scene.camera->position.y) + std::string(" ") +
//            std::to_string(scene.camera->position.z)
//                    ).c_str());
//        ImGui::Text("%s", (std::string("Camera Direction: ") +
//                     std::to_string(scene.camera->direction.x) + std::string(" ") +
//                     std::to_string(scene.camera->direction.y) + std::string(" ") +
//                     std::to_string(scene.camera->direction.z)
//                    ).c_str());
//        ImGui::Text("%s", (std::string("Vertex Number: ") +
//                           std::to_string(scene.vnum)
//                ).c_str());
//        ImGui::Text("%s", (std::string("Triangle Number: ") +
//                           std::to_string(scene.fnum)
//        ).c_str());
//        ImGui::End();
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        loop++;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();

}


void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, nell::Camera *camera, unsigned int shaderid) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->moveForward(static_cast<float>(SPEED));
        camera->update();
        cameraMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->moveBackward(static_cast<float>(SPEED));
        camera->update();
        cameraMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->moveLeft(static_cast<float>(SPEED));
        camera->update();
        cameraMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->moveRight(static_cast<float>(SPEED));
        camera->update();
        cameraMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->moveUp(static_cast<float>(SPEED));
        camera->update();
        cameraMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->moveDown(static_cast<float>(SPEED));
        camera->update();
        cameraMove = true;
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
        glm::vec3 newOrientation = glm::rotate(camera->direction, glm::radians(-rotX), glm::normalize(glm::cross(camera->direction, vec3(0, 1, 0))));

        // Decides whether or not the next vertical Orientation is legal or not
        if (abs(glm::angle(newOrientation, vec3(0, 1, 0)) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            camera->direction = newOrientation;
        }

        // Rotates the Orientation left and right
        camera->direction = glm::rotate(camera->direction, glm::radians(-rotY), vec3(0, 1, 0));

        // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
        glfwSetCursorPos(window, (WIDTH / 2), (HEIGHT / 2));
        cameraMove = true;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
    // camera->updateAndSync(shaderid);
}



