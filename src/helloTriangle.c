#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

//#include "linmath.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800
#define STRING_BUFFER 512

const char *vertexShaderSource = "#version 330 core\n"
       "layout (location = 0) in vec3 aPos;\n"
       "void main()\n"
       "{\n"
       " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
       "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
       "FragColor = vec4(1.0f, 0.4f, 0.2f, 1.0f);\n"
       "}\0";

const char *fragmentShaderSource2 = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
       "FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
       "}\0";

/*static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}*/
       
void ProcessInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "ERROR: Callback %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;

    float vertices[] = {
         0.375f,   0.5f, 0.0f,
         0.f, -0.5f, 0.0f, 
         0.75f, -0.5f, 0.0f,
    };

    float vertices2[] = {
        -0.75f,  0.5f, 0.0f,
        -0.f,  0.5f, 0.0f,
        -0.375f,  -0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
    };

    glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialized GLFW....exiting.");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Testing", NULL, NULL);
    if(!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: Failed to initialize window....exiting.");
        exit(EXIT_FAILURE);
    }

    //glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[STRING_BUFFER];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) { 
        glGetShaderInfoLog(vertexShader, STRING_BUFFER, NULL, infoLog);
        fprintf(stderr, "SHADER ERROR: %s\n", infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, STRING_BUFFER, NULL, infoLog);
        fprintf(stderr, "SHADER ERROR: %s\n", infoLog);
    }

    unsigned int fragmentShader2;
    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);

    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader2, STRING_BUFFER, NULL, infoLog);
        fprintf(stderr, "SHADER ERROR: %s\n", infoLog);
    }

    unsigned int shaderProgram, shaderProgram2;
    shaderProgram = glCreateProgram();
    shaderProgram2 = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, STRING_BUFFER, NULL, infoLog);
        fprintf(stderr, "SHADER PROG ERROR: %s\n", infoLog);
    }
 
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);

    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram2, STRING_BUFFER, NULL, infoLog);
        fprintf(stderr, "SHADER ERROR: %s\n", infoLog);
    }

    glDeleteShader(fragmentShader2);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VBO, VAO, EBO, VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    while(!glfwWindowShouldClose(window)) {
        //glfwGetFramebufferSize(window, &width, &height);
        ProcessInput(window);
          
        glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
       
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return(EXIT_SUCCESS);
}
