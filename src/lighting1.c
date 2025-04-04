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
#include "camera.h"

#define ARENA_SIZE 1024 * 1024
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define TRUE 1
#define FALSE 0

float deltaTime;
float lastFrame;
float currentFrame;
int mouseFirst;
Camera *cam;

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
    ProcessMouseScroll(cam, yOffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ProcessKeyboard(cam, FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ProcessKeyboard(cam, BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ProcessKeyboard(cam, LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ProcessKeyboard(cam, RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if(mouseFirst) {
        cam->lastX = xpos;
        cam->lastY = ypos;
        mouseFirst = FALSE;
    }

    float xOffset = xpos - cam->lastX;
    float yOffset = cam->lastY - ypos;
    cam->lastX = xpos;
    cam->lastY = ypos;
    ProcessMouseMovement(cam, xOffset, yOffset, TRUE);
}

void error_callback(int error, const char *description) {
    fprintf(stderr, "ERROR: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    mouseFirst = TRUE;
    vec3 lightPos = {1.2f, 1.f, 2.f};

    ScratchArena arena = createArena(ARENA_SIZE);
    arenaPush(&arena);
    cam = arenaAlloc(&arena, sizeof(Camera), sizeof(float));
    
    CameraInit(cam, (vec3){0.f, 0.f, 3.f}, (vec3){0.f, 1.f, 0.f}, YAW, PITCH);

    if(!cam) {
        fprintf(stderr, "ERROR: Failed to initialize camera.\n");
        glfwTerminate();
        destroyArena(&arena);
        exit(EXIT_FAILURE);
    }

    if(!glfwInit()) { 
        fprintf(stderr, "ERROR: Failed to initialize GLFW. \n");
        glfwTerminate();
        destroyArena(&arena);
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL Lighting 1", NULL, NULL);
    if(!window) {
        fprintf(stderr, "ERROR: Failed to initialize window.\n");
        glfwTerminate();
        destroyArena(&arena);
        exit(EXIT_FAILURE);
}

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };
   
    Shader shader = loadGlShaders(&arena, "shaders/lighting1Vertex.vs", "shaders/lighting1Fragment.fs");
    Shader lightCubeShader = loadGlShaders(&arena, "shaders/lightCubeVertex.vs", "shaders/lightCubeFragment.fs");

    GLuint cubeVAO, VBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    GLuint lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    mat4x4 projection;
    mat4x4_identity(projection);
    float fovRad;
    mat4x4 model;
    mat4x4_identity(model);
    mat4x4 view;

    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    GLuint projLoc = glGetUniformLocation(shader.ID, "projection");
    GLuint viewLoc = glGetUniformLocation(shader.ID, "view");

    while(!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &width);
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        fovRad = DEG2RAD(cam->Zoom);

        processInput(window);

        mat4x4_perspective(projection, fovRad, (float)width / height, 01.f, 100.f);

        vec3 tmp;
        vec3_add(tmp, cam->Position, cam->Front);
        mat4x4_look_at(view, cam->Position, tmp, cam->Up);

        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.ID);
        glUniform3f(glGetUniformLocation(shader.ID, "objectColor"), 1.f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shader.ID, "lightColor"), 1.f, 1.f, 1.f);

        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (void*)projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (void*)view);

        mat4x4_translate(model, 2.f, 2.f, 2.f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (void*)model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUseProgram(lightCubeShader.ID);
        mat4x4_identity(model);
        mat4x4_translate(model, lightPos[0], lightPos[1], lightPos[2]);
        mat4x4_scale(model, model, .2f);
        
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (void*)projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (void*)view);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (void*)model);
       
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    destroyArena(&arena);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
