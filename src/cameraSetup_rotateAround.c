#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "scratch.h"
#include "shaders.h"
#include "textures.h"
#include "linmath.h"

#define ARENA_SIZE 1024 * 1024
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define STRING_BUFFER 512

float mixOffset;

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
    
    if(key == GLFW_KEY_W && action == GLFW_PRESS) {
        if(mixOffset >= 1.f) {
            mixOffset = 1.f;
        } else {
            mixOffset += .1f;
        }
    }

    if(key == GLFW_KEY_S && action == GLFW_PRESS) {
        if(mixOffset <= 0.f) {
            mixOffset = 0.f;
        } else {
            mixOffset -= .1f;
        }
    }
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "ERROR: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
    fprintf(stderr, "INFO: Window size: W: %d, H: %d\n", width, height);
}

int main() {
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    mixOffset = .4f;

    ScratchArena arena = createArena(ARENA_SIZE);

    if(!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialzie GLFW.\n");
        glfwTerminate();        
        destroyArena(&arena);
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow * window = glfwCreateWindow(width, height, "OpenGL Camera Setup", NULL, NULL);
    if(!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: Failed to initialize window\n");
        destroyArena(&arena);
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

        float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    }; 

    vec3 cubePositions[] = {
         { 0.f, 0.f, 0.f },
         { 2.f, 5.f, -15.f },
         { -1.5f, -2.2f, -2.5f },
         { -3.8f, -2.f, -12.3f },
         { 2.4f, -0.4f, -3.5f },
         { -1.7f, 3.f, -7.5f },
         { 1.3f, 2.0f, -2.5f },
         { 1.5f, 2.f, -2.5f },
         { 1.5f, 0.2f, -1.5f },
         { -1.3f, 1.f, -1.5f }
    };
    
    Shader shader = loadGlShaders(&arena, "shaders/coordVertex.vs", "shaders/coordFragment.fs");

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint texture1, texture2;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    loadTextureRGB("resources/textures/container.jpg");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    loadTextureRGBA("resources/textures/awesomeface.png");

    glEnable(GL_DEPTH_TEST);
    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);
    
    mat4x4 projection;
    mat4x4_identity(projection);
    float fovRad = 45.f * M_PI / 180;

    mat4x4 model;
    mat4x4_identity(model);

    vec3 cameraPos = {0.f, 0.f, 3.f};
    vec3 cameraTarget = {0.f, 0.f, 0.f};
    
    vec3 cameraDirection;
    vec3_sub(cameraDirection, cameraPos, cameraTarget);
    vec3_norm(cameraDirection, cameraDirection);

    vec3 cameraRight;
    vec3 up = {0.f, 1.f, 0.f};
    vec3_mul_cross(cameraRight, up, cameraDirection);
    vec3_norm(cameraRight, cameraRight);

    vec3 cameraUp;
    vec3_mul_cross(cameraUp, cameraRight, cameraDirection);

    mat4x4 view;
    //mat4x4_look_at(view, cameraPos, cameraTarget, up);
    
    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    GLuint projLoc = glGetUniformLocation(shader.ID, "projection");
    GLuint viewLoc = glGetUniformLocation(shader.ID, "view");
   
    float radius = 10.f;
    float zRot;

    while(!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        mat4x4_look_at(view, (vec3){camX, 0.f, camZ}, cameraDirection, up);

        zRot = (float)glfwGetTime();

        mat4x4_perspective(projection, fovRad, width / height, 0.1f, 100.f); 
        
        mat4x4_rotate(model, model, 1.f, 1.f, 0.f, zRot);
        
        glClearColor(0.2f, 0.3f, 0.6f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glUseProgram(shader.ID);

        glUniform1f(glGetUniformLocation(shader.ID, "mixOffset"), mixOffset);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (void*)projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (void*)view);
        
        glBindVertexArray(VAO);
        for(int i = 0; i < 10; i++) {
            if(i % 3 == 1) {
                mat4x4_identity(model);
                mat4x4_translate(model, cubePositions[i][0], cubePositions[i][1], cubePositions[i][2]);
                mat4x4_rotate(model, model, 1.f, 0.f, 0.f, zRot);
            } else {
                mat4x4_identity(model);
                mat4x4_translate(model, cubePositions[i][0], cubePositions[i][1], cubePositions[i][2]);
                mat4x4_rotate(model, model, .4f, .6f, 1.f, 40);
            }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (void*)model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
        }
    
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    destroyArena(&arena);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
