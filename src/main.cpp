#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>


#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "shader.hpp"
#include "utils/random.hpp"
#include "camera.hpp"


void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, nell::Camera &camera, unsigned int shaderid);


#define WIDTH 1920
#define HEIGHT 1080


const float Material_Lambertian = 0.0;
const float Material_Metal = 1.0;
const float Material_Dielectric = 2.0;

#define SPEED 0.05 // speed of camera move

float firstClick = true;

float sensitivity = 5.0;




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
    vec3 position = vec3(0, 0, 1);
    vec3 direction = vec3(0, 0, -1);
    float focusLength = 1.0;

    nell::Camera camera(position, direction, 20, aspect_ratio, focusLength);
    camera.updateAndSync(ptShader.id);

#define materialArraySize 14

    const float materials[materialArraySize] {
        Material_Lambertian, 0.8, 0.8, 0.0,
        Material_Metal, 0.1, 0.2, 0.5, 0.0,
        Material_Dielectric, 1.0, 1.0, 1.0, 1.5
    };




    // Render loop

    nell::CPURandomInit();

    while (!glfwWindowShouldClose(window)) {
        processInput(window, camera, ptShader.id);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float randOrigin = 674764.0f * (nell::GetCPURandom() + 1.0f);
        auto time = static_cast<float>(glfwGetTime());


        glUniform1i(glGetUniformLocation(ptShader.id, "width"), WIDTH);
        glUniform1i(glGetUniformLocation(ptShader.id, "height"), HEIGHT);
        glUniform1f(glGetUniformLocation(ptShader.id, "randOrigin"), randOrigin);
        glUniform1f(glGetUniformLocation(ptShader.id, "time"), time);

        glUniform1i(glGetUniformLocation(ptShader.id, "materialArraySize"), materialArraySize);
        glUniform1fv(glGetUniformLocation(ptShader.id, "materials"), materialArraySize, materials);

        ptShader.use();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




        glfwSwapBuffers(window);
        glfwPollEvents();
    }
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
