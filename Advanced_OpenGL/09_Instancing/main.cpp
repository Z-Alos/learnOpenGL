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
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };    

    // Calculate Traslation
    // glm::vec2 translations[100];
    // int index = 0;
    // float offset = 0.1f;
    // for(int y  = -10; y < 10; y+=2){
    //     for(int x  = -10; x < 10; x+=2){
    //         glm::vec2 translation;
    //         translation.x = (float)x / 10.0f + offset;
    //         translation.y = (float)y / 10.0f + offset;
    //         translations[index++] = translation;
    //         // std::cout<< translation.x << translation.y << std::endl;
    //     }
    // }

    // instanced VBO
    // unsigned int instancedVBO;
    // glGenBuffers(1, &instancedVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, instancedVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // instanced vbo
    // glBindBuffer(GL_ARRAY_BUFFER, instancedVBO);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
    // glEnableVertexAttribArray(2);
    // glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
    glBindVertexArray(0);

    // Build & Compile Shaders
    // Shader shader("../shader.vert", "../shader.frag");
    Shader marsShader("../mars.vert", "../mars.frag");
    Shader asteroidShader("../asteroid.vert", "../asteroid.frag");

    // Model
    // Model* mars = nullptr;
    // try {
    //     mars = new Model("../../../res/models/mars/planet.obj");
    //     std::cout << "Model loaded successfully!" << std::endl;
    // } catch (const std::exception& e) {
    //     std::cout << "Model loading failed: " << e.what() << std::endl;
    //     return -1;
    // }
    
    Model mars("../../../res/models/mars/planet.obj");
    Model asteroid("../../../res/models/asteroid/rock.obj");

    // shader.use();
    // shader.setInt("cubeTexture", 0);


    // shader.use();
    // for(unsigned int i=0; i<100; i++){
    //     shader.setVec2(("offsets[" + std::to_string(i) + "]"), translations[i]);
    // }
    
    unsigned int amount = 40000;
    glm::mat4 * modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // seed
    float radius = 70.0f;
    float offset = 16.5f;
    for(unsigned int i=0; i<amount; i++){
        glm::mat4 model = glm::mat4(1.0f);
        // translate: random displacement in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // scale
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // rotation
        float rotAngle= (rand() % 20) / 100.0f + 0.05;
        model = glm::rotate(model, rotAngle, glm::vec3(0.4, 0.6, 0.8));

        modelMatrices[i] = model;
    }

    // instanced buffer for asteroids
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for(unsigned int i=0; i<asteroid.meshes.size(); i++){
        unsigned int VAO = asteroid.meshes[i].VAO;
        glBindVertexArray(VAO);

        // vertex attributes
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    glEnable(GL_DEPTH_TEST);
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
        
        marsShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        marsShader.setMat4("view", view);
        marsShader.setMat4("projection", projection);
        // marsShader.setVec3("cameraPos", camera.Position);

        // draw mars 
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        marsShader.setMat4("model", model);

        mars.Draw(marsShader);

        // not instanced
        // for(unsigned int i=0; i<amount; i++){
        //     marsShader.setMat4("model", modelMatrices[i]);
        //     asteroid.Draw(marsShader);
        // }
        
        // instanced
        asteroidShader.use();
        asteroidShader.setMat4("view", view);
        asteroidShader.setMat4("projection", projection);
        asteroidShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, asteroid.textures_loaded[0].id);
        for(unsigned int i=0; i<asteroid.meshes.size(); i++){
            glBindVertexArray(asteroid.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(asteroid.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        // glBindVertexArray(quadVAO);
        // // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
        // glBindVertexArray(0);
                              
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // // De-allocate all resources once they've outlived their purpose;
    // glDeleteVertexArrays(1, &cubeVAO);
    // glDeleteBuffers(1, &cubeVBO);

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












