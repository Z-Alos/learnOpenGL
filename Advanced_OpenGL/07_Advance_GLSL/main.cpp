#include "../../include/glad/include/glad/glad.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <vector>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../include/shader.h"
#include "../../include/camera.h"
#include "../../include/model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(vector<std::string> faces);

// GLOBAL VARIABLES
// Screen Settings
const unsigned int SCR_WIDTH=1080;
const unsigned int SCR_HEIGHT=720;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // arg: cameraPos
float lastX=SCR_WIDTH/2.0f; 
float lastY=SCR_HEIGHT/2.0f;
float firstMouse=true;

// Timing
float deltaTime=0.0f;
float lastFrame=0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 0.0f, 2.0f);

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "opengl", NULL, NULL);

    if(window==NULL){
        std::cout << "Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // MOUSE
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Vertices
    float cubeVertices[] = {
        // positions         
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


    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // Build & Compile Shaders
    Shader redShader("../shader.vert", "../red.frag");
    Shader greenShader("../shader.vert", "../green.frag");
    Shader blueShader("../shader.vert", "../blue.frag");
    Shader yellowShader("../shader.vert", "../yellow.frag");

    // Model
    // Model* sponza = nullptr;
    // try {
    //     sponza = new Model("../../../res/models/sponza/scene.gltf");
    //     std::cout << "Model loaded successfully!" << std::endl;
    // } catch (const std::exception& e) {
    //     std::cout << "Model loading failed: " << e.what() << std::endl;
    //     return -1;
    // }

    unsigned int cubeTexture = loadTexture("../../../res/textures/container.jpg");

    // Uniform Block
    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(redShader.ID, "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(greenShader.ID, "Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(blueShader.ID, "Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(yellowShader.ID, "Matrices");

    glUniformBlockBinding(redShader.ID, uniformBlockIndexRed, 0);
    glUniformBlockBinding(greenShader.ID, uniformBlockIndexGreen, 0);
    glUniformBlockBinding(blueShader.ID, uniformBlockIndexBlue, 0);
    glUniformBlockBinding(yellowShader.ID, uniformBlockIndexYellow, 0);

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2*sizeof(glm::mat4));

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // RENDER LOOP
    while(!glfwWindowShouldClose(window)){
        // Delta Time
        float currentFrame=glfwGetTime();
        deltaTime=currentFrame-lastFrame;
        lastFrame=currentFrame;

        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // cubes
        glBindVertexArray(cubeVAO);
        // RED
        redShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
        redShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // GREEN
        greenShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
        greenShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // YELLOW
        yellowShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
        yellowShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // BLUE
        blueShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
        blueShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose;
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
       glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse){
        lastX=xpos;
        lastY=ypos;
        firstMouse=false;
    }

    float xoffset=xpos-lastX;
    float yoffset=lastY-ypos; // reversed: y-corrd -> bottom to top
    lastX=xpos;
    lastY=ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if(data){
        GLenum format;
        if(nrComponents==1)
            format=GL_RED;
        else if(nrComponents==3)
            format=GL_RGB;
        else if(nrComponents==4)
            format=GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else{
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for(unsigned int i=0; i<faces.size(); i++){
        unsigned char *texture_data  = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(texture_data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
            stbi_image_free(texture_data);
        }
        else {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(texture_data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}












