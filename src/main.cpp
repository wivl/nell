#include <iostream>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "nell/core/shader_utils/shader.hpp"
#include "nell/core/camera.hpp"
#include "nell/core/random.hpp"
#include "nell/core/framebuffer.hpp"

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    float vertices[] = {
            1.0f, 1.0f, 1.0, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3,
    };

    unsigned int vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    nell::Shader pathtracing_shader = nell::Shader("../src/shader/default.vert", "../src/shader/pathtrace.frag");
    nell::Shader acc_shader = nell::Shader("../src/shader/default.vert", "../src/shader/accumulate.frag");
    nell::Shader preview_shader = nell::Shader("../src/shader/default.vert", "../src/shader/preview.frag");

    vec2 screen_size = vec2(WIDTH, HEIGHT);

    float aspect_ratio = screen_size.x / screen_size.y;
    vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    float dist_to_focus = length(lookat-lookfrom);
    float aperture = 10.0;

    nell::Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    camera.update_and_sync(pathtracing_shader.id);



    unsigned int pt_texture, pt_framebuffer;
    glGenTextures(1, &pt_texture);
    glBindTexture(GL_TEXTURE_2D, pt_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &pt_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pt_framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pt_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
    }


    unsigned int acc_texture, acc_framebuffer;
    glGenTextures(1, &acc_texture);
    glBindTexture(GL_TEXTURE_2D, acc_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &acc_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, acc_framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           acc_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pt_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, acc_texture);

    glUniform1i(glGetUniformLocation(acc_shader.id, "currentFrame"), 0);
    glUniform1i(glGetUniformLocation(acc_shader.id, "accFrame"), 1);
    glUniform1i(glGetUniformLocation(preview_shader.id, "frame"), 1);



    nell::CPURandomInit();

    int loop = 0;
    while (!glfwWindowShouldClose(window)) {
//        std::this_thread::sleep_for(std::chrono::seconds(2));
        process_input(window);
        float rand_origin = 674764.0f * (nell::GetCPURandom() + 1.0f);
        auto time = static_cast<float>(glfwGetTime());
        // std::cout << time << " " << rand_origin << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, pt_framebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        pathtracing_shader.use();
        camera.sync(pathtracing_shader.id);

        glUniform1f(glGetUniformLocation(pathtracing_shader.id, "time"), time);
        glUniform1f(glGetUniformLocation(pathtracing_shader.id, "rand_origin"), rand_origin);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        glBindFramebuffer(GL_FRAMEBUFFER, acc_framebuffer);
        acc_shader.use();
        glUniform1i(glGetUniformLocation(acc_shader.id, "accFrame"), 1);
        glUniform1i(glGetUniformLocation(acc_shader.id, "currentFrame"), 0);
        glUniform1i(glGetUniformLocation(acc_shader.id, "loop"), loop);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        preview_shader.use();
        glUniform1i(glGetUniformLocation(preview_shader.id, "frame"), 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        loop ++;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

