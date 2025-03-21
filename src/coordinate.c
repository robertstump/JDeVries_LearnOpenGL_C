#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "linmath.h"
#include "scratch.h"
#include "shaders.h"
#include "textures.h"

float mixOffset;
float camZ;

#define ARENA_SIZE 1024 * 1024
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define STRING_BUFFER 512

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if(key == GLFW_KEY_M && action == GLFW_PRESS) {
        if(!glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
            glfwMaximizeWindow(window);
        } else {
            glfwRestoreWindow(window);
        }
    }

    if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if(mixOffset >= 1.0f) {
            mixOffset = 1.f; 
        } else {
            mixOffset += 0.1f;
        }
    }

    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if(mixOffset <= 0.f) {
            mixOffset = 0.f;
        } else {
            mixOffset -= 0.1f;
        }
    }

    if(key == GLFW_KEY_S && action == GLFW_PRESS) {
        camZ -= .2f;
    }

    if(key == GLFW_KEY_W && action == GLFW_PRESS) {
        camZ += .2f;
    }
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "ERROR: Error Callback %s\n", description);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "INFO: Window Size Callback: w: %d, h: %d\n", width, height);
}

int main() {
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    mixOffset = 0.4;
    camZ = -3.f;

    ScratchArena arena = createArena(ARENA_SIZE);

    glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow * window = glfwCreateWindow(width, height, "OpenGL Coordinate Systems", NULL, NULL);
    if(!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: Failed to initialize GLFW window\n");
        destroyArena(&arena);
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    float vertices[] = {
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left     
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3 
    };

    Shader shader = loadGlShaders(&arena, "shaders/coordVertex.vs", "shaders/textureFragment.fs");

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    //load sets repeat in s & t, min and mag filter linear, pass filename and texture location enum
    GLuint texture1, texture2;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    loadTextureRGB("resources/textures/container.jpg");

    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    loadTextureRGBA("resources/textures/awesomeface.png");

    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

    mat4x4 projection;
    float fovRad = 45.f * M_PI / 180;
    mat4x4_perspective(projection, fovRad, 1280 / 960, 0.1f, 100.f);

    mat4x4 model;
    mat4x4_identity(model);
    float modelRad = -55.f * M_PI / 180;
    mat4x4_rotate(model, model, 1.f, 0.f, 0.f, modelRad);

    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    GLuint projLoc = glGetUniformLocation(shader.ID, "projection");
    GLuint viewLoc = glGetUniformLocation(shader.ID, "view");

    while(!glfwWindowShouldClose(window)) {
            glfwGetFramebufferSize(window, &width, &height);

            mat4x4 view;
            mat4x4_identity(view);
            mat4x4_translate(view, 0.f, 0.f, camZ);

            //render loop:
            glClearColor(0.2f, 0.3f, 0.6f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);

            glUseProgram(shader.ID);
            glUniform1f(glGetUniformLocation(shader.ID, "mixOffset"), mixOffset);

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (void*)model);
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, (void*)projection);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (void*)view);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
            }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    destroyArena(&arena);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
