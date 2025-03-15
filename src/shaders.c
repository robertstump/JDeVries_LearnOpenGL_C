#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include "scratch.h"
#include "shaders.h"

#define ARENA_SIZE 1024 * 1024
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960
#define STRING_BUFFER 512

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if(key == GLFW_KEY_M && action == GLFW_PRESS) {
        if(!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
            glfwMaximizeWindow(window);
        else
            glfwRestoreWindow(window);
    }
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "ERROR: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "INFO: Window Size Callback\n");
}

int main() {
    ScratchArena arena = createArena(ARENA_SIZE);

    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    
    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Shaders", NULL, NULL);
    if(!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: Failed to initialize GLFW window.\n");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    float vertices[] = {
        0.0f, 0.5f, 0.0f, 1.0f, 0.f, 0.f,
        -0.5f, -0.5f, 0.0f, 0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.0f, 0.f, 0.f, 1.f
    };

    float indices[] = {
        0, 1, 2 
    };

    Shader shader = loadGlShaders(&arena, "shaders/vertex.vs", "shaders/fragment.fs");

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(VAO);

    float rotationAngleDegree = 0.0f;

    while(!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);
        //render loop
        glClearColor(0.2f, 0.4f, 0.6f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        rotationAngleDegree += 0.5f;

        glUseProgram(shader.ID);
        GLint angleUniformLoc = glGetUniformLocation(shader.ID, "angle");
        glUniform1f(angleUniformLoc, rotationAngleDegree);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    destroyArena(&arena);
    return(EXIT_SUCCESS);
}
