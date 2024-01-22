#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "nell/core/shader_utils/shader.hpp"

#define WIDTH 1600
#define HEIGHT 800

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

unsigned int imageVAO, imageVBO, imageEBO;

void render_set() {
    float vertices[] = {
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    glGenVertexArrays(1, &imageVAO);
    glBindVertexArray(imageVAO);

    glGenBuffers(1, &imageVBO);
    glBindBuffer(GL_ARRAY_BUFFER, imageVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &imageEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imageEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

}

void render_image(nell::Shader shader) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "nellpt", NULL, NULL);
    if (window==NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    nell::Shader pathtracingShader = nell::Shader("../src/shader/pathtrace.vert", "../src/shader/pathtrace.frag");

    // glUniform3f(glGetUniformLocation(pathtracingShader.id, "camera.lower_left_corner"), -2.0, -1.0, -1.0);
    // glUniform3f(glGetUniformLocation(pathtracingShader.id, "camera.horizontal"), 4.0, 0.0, 0.0);
    // glUniform3f(glGetUniformLocation(pathtracingShader.id, "camera.vertical"), 0.0, 2.0, 0.0);
    // glUniform3f(glGetUniformLocation(pathtracingShader.id, "camera.origin"), 0.0, 0.0, 0.0);


    render_set();

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        pathtracingShader.use();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

