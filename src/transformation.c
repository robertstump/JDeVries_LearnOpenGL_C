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
#include "linmath.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define STRING_BUFFER 512
#define ARENA_SIZE 1024 * 1024

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
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
    fprintf(stderr, "ERROR: Error Callback %s\n", description);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "INFO: Winodw Size Callback: w: %d, h: %d\n", width, height);
}

int main() {
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    
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

    GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL Transformations", NULL, NULL);
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
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // top left     
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3 
    };
    
    Shader shader = loadGlShaders(&arena, "shaders/transformVertex.vs", "shaders/textureFragment.fs");

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

    GLuint texture1, texture2;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(1);

    unsigned char *data = stbi_load("resources/textures/container.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if(!data) { 
        fprintf(stderr, "ERROR: Failed to load image with stb library\n");
    } else { 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    
    data = stbi_load("resources/textures/awesomeface.png", &texWidth, &texHeight, &nrChannels, 0);
    if(!data) {
        fprintf(stderr, "ERROR: Failed to load image with stb library\n");
    } else { 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

    mat4x4 trans, secondTrans; 

    while(!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        //render loop:
        glClearColor(0.2f, 0.4f, 0.6f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        mat4x4_identity(trans);
        mat4x4_translate_in_place(trans, .5f, -.5f, 0.f);
        mat4x4_rotate(trans, trans, 0.f, 0.f, 1.f, (float)glfwGetTime());

        glUseProgram(shader.ID);
        GLuint transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (void*)trans);
 
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        mat4x4_identity(secondTrans);
        mat4x4_rotate(secondTrans, secondTrans, 0.f, 0.f, 1.f, -(float)glfwGetTime());
        mat4x4_translate_in_place(secondTrans, .5f, .5f, 0.f);
        float timeNow = (float)sinf(glfwGetTime());
        float timeNow2 = (float)cosf(glfwGetTime());
        mat4x4_scale_aniso(secondTrans, secondTrans, timeNow, timeNow2, 0.f);

        glUseProgram(shader.ID);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (void*)secondTrans);

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
