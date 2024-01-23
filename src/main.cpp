#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "nell/core/shader_utils/shader.hpp"
#include "nell/core/camera.hpp"

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
            1.0f, 1.0f, 1.0, 1.0,
            1.0f, -1.0f, 1.0f, 0.0,
            -1.0f, -1.0f, 0.0f, 0.0,
            -1.0f, 1.0f, 0.0f, 1.0,
    };

    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3,
    };



    // 1) raytrace image to texture1
    // 2) blend and store image to texture2
    // 3) show texture to screen

    // ---Path tracing part---
    nell::Shader pathtracing_shader = nell::Shader("../src/shader/pathtrace.vert", "../src/shader/pathtrace.frag");
    // Set up a camera
    vec2 screen_size = vec2(WIDTH, HEIGHT);

    float aspect_ratio = screen_size.x / screen_size.y;
    vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    float dist_to_focus = length(lookat-lookfrom);
    float aperture = 0.0;

    nell::Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    camera.sync(pathtracing_shader.id);

    // Raytracer vao
    unsigned int rt_vao, rt_vbo, rt_ebo;

    glGenVertexArrays(1, &rt_vao);
    glBindVertexArray(rt_vao);

    glGenBuffers(1, &rt_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rt_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &rt_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rt_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)(2*sizeof(float)));

    // TODO: Framebuffer for raytracing: the raytracing shader render image to this buffer as texture,
    // and then this buffer is used in accumulate shader to blend to another frame buffer,
    // then the preview shader draw the blended rt_result_buffer to screen.
    // rt_result_buffer -> accumulate shader
    // acc_buffer -> preview shader
    unsigned int rt_result_buffer;
    glGenFramebuffers(1, &rt_result_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, rt_result_buffer); // bind to bind texture
    // texture
    unsigned int rt_texture;
    glGenTextures(1, &rt_texture);
    glBindTexture(GL_TEXTURE_2D, rt_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_texture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ---accumulate part---
    nell::Shader acc_shader = nell::Shader("../src/shader/accumulate.vert", "../src/shader/accumulate.frag");
    // accumulate vao
    unsigned int acc_vao, acc_vbo, acc_ebo;

    glGenVertexArrays(1, &acc_vao);
    glBindVertexArray(acc_vao);

    glGenBuffers(1, &acc_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, acc_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &acc_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, acc_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));



    // Framebuffer for accumulate result
    unsigned int acc_framebuffer;
    glGenFramebuffers(1, &acc_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, acc_framebuffer); // bind to bind texture
    // texture
    unsigned int acc_texture;
    glGenTextures(1, &acc_texture);
    glBindTexture(GL_TEXTURE_2D, acc_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // bind texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, acc_texture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ---Preview part---
    nell::Shader preview_shader = nell::Shader("../src/shader/preview.vert", "../src/shader/preview.frag");



    unsigned int preview_vao, preview_vbo, preview_ebo;

    glGenVertexArrays(1, &preview_vao);
    glBindVertexArray(preview_vao);

    glGenBuffers(1, &preview_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, preview_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &preview_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, preview_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    // Framebuffer for accumulate result
    unsigned int preview_framebuffer;
    glGenFramebuffers(1, &preview_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, preview_framebuffer); // bind to bind texture
    // texture
    unsigned int preview_texture;
    glGenTextures(1, &preview_texture);
    glBindTexture(GL_TEXTURE_2D, preview_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // bind texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, preview_texture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    preview_shader.use();
    glUniform1i(glGetUniformLocation(preview_shader.id, "screenFrame"), 0);
    acc_shader.use();
    glUniform1i(glGetUniformLocation(acc_shader.id, "currentFrame"), 0);
    glUniform1i(glGetUniformLocation(acc_shader.id, "accFrame"), 1);

    int loop = 0;
    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        loop ++;
        float mix_part = (float(loop)-1.0f) / float(loop);
//        printf("%d: %f\n", &loop, &mix_part);
        std::cout << loop << ": " << mix_part << std::endl;

        // raytracing pass
        glBindFramebuffer(GL_FRAMEBUFFER, rt_result_buffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        pathtracing_shader.use();
        camera.sync(pathtracing_shader.id);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //TODO: 按照图表绘制

        // acc
        glBindFramebuffer(GL_FRAMEBUFFER, acc_framebuffer); // 设置输出为 acc_framebuffer
        acc_shader.use();
        glUniform1f(glGetUniformLocation(acc_shader.id, "mix_part"), mix_part);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rt_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, acc_texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         preview
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        preview_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, acc_texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

