#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "shader.hpp"
#include "utils/random.hpp"
#include "camera.hpp"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window, nell::Camera &camera);


#define WIDTH 1920
#define HEIGHT 1080

#define SPEED 0.1 // speed of camera move



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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    float dist_to_focus = length(lookat - lookfrom);
    float aperture = 10.0;

    nell::Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture,
                        dist_to_focus);
    camera.update_and_sync(ptShader.id);





    // Render loop

    nell::CPURandomInit();

    while (!glfwWindowShouldClose(window)) {
        process_input(window, camera);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float randOrigin = 674764.0f * (nell::GetCPURandom() + 1.0f);
        auto time = static_cast<float>(glfwGetTime());


        glUniform1i(glGetUniformLocation(ptShader.id, "width"), WIDTH);
        glUniform1i(glGetUniformLocation(ptShader.id, "height"), HEIGHT);
        glUniform1f(glGetUniformLocation(ptShader.id, "randOrigin"), randOrigin);
        glUniform1f(glGetUniformLocation(ptShader.id, "time"), time);

        ptShader.use();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, nell::Camera &camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // TODO: Camera update
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.origin += (camera.w  * static_cast<float>(SPEED));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.origin -= (camera.w  * static_cast<float>(SPEED));
    }

}
