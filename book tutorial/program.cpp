// opengl demo demonstrating many different methodologies e.g. drawing cubes, textures, scaling

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <shaderClass/shader.h>
#include <cameraClass/cameraClass.h>
#include <iostream>
#include <math.h>
#include <filesystem>
#define  STB_IMAGE_IMPLEMENTATION
#include <shaderClass/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "program.h"
using namespace glm;

float transparency = 0.5f;

vec3 cameraPos(0.0f, 0.0f, 3.0f);
vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

// untie framerate to the movement speed
float deltaTime = 0.0f; // time between the last and current frame
float lastFrame = 0.0f; // time of last frame

//for mouse movement
Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = 400, lastY = 300;
float pitch1 = 0.0f, yaw1 = -90.0f;
float mouseEntered = true;

// for zoom
//float fov = 90.0f;

// the virtual sun
vec3 lightPos(1.2f, 1.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // need to put external functions before the main one for some reason
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    
    float cameraSpeed = 5.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        transparency = transparency + 0.001f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        transparency = transparency - 0.001f;
    }



    /*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    }*/

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

}


void mouse_callback(GLFWwindow* window, double x, double y) {

    //if (mouseEntered) {
    //    lastX = xpos;
    //    lastY = ypos;
    //    mouseEntered = false;
    //}

    //float xOffset = xpos - lastX;
    //float yOffset = lastY - ypos; // vice versa since y coordinate axis is reversed
    //const float sensitivity = 0.4f;

    //lastX = xpos;
    //lastY = ypos;
    //xOffset *= sensitivity;
    //yOffset *= sensitivity;
    //yaw1 += xOffset;
    //pitch1 += yOffset;
    //
    //if (pitch1 > 89.0f) {
    //    pitch1 = 89.0f;
    //}

    //if (pitch1 < -89.0f) {
    //    pitch1 = -89.0f;
    //}

    //vec3 direction;
    //direction.x = cos(radians(yaw1)) * cos(radians(pitch1));
    //direction.y = sin(radians(pitch1));
    //direction.z = sin(radians(yaw1)) * cos(radians(pitch1));

    //cameraFront = normalize(direction);

    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    if (mouseEntered)
    {
        lastX = xpos;
        lastY = ypos;
        mouseEntered = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}

void  scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

// Custom implementation of the LookAt function
mat4 calculate_lookAt_matrix(vec3 position, vec3 target, vec3 worldUp)
{
    // 1. Position = known
    // 2. Calculate cameraDirection
    vec3 zaxis = normalize(position - target);
    // 3. Get positive right axis vector
    vec3 xaxis = normalize(cross(normalize(worldUp), zaxis));
    // 4. Calculate camera up vector
    vec3 yaxis = cross(zaxis, xaxis);

    // Create translation and rotation matrix
    // In glm we access elements as mat[col][row] due to column-major layout
    mat4 translation = mat4(1.0f); // Identity matrix by default
    translation[3][0] = -position.x; // Fourth column, first row
    translation[3][1] = -position.y;
    translation[3][2] = -position.z;
    mat4 rotation = mat4(1.0f);
    rotation[0][0] = xaxis.x; // First column, first row
    rotation[1][0] = xaxis.y;
    rotation[2][0] = xaxis.z;
    rotation[0][1] = yaxis.x; // First column, second row
    rotation[1][1] = yaxis.y;
    rotation[2][1] = yaxis.z;
    rotation[0][2] = zaxis.x; // First column, third row
    rotation[1][2] = zaxis.y;
    rotation[2][2] = zaxis.z;

    // Return lookAt matrix as combination of translation and rotation matrix
    return rotation * translation; // Remember to read from right to left (first translation then rotation)
}

int main()
{
    // instantiate window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for macintosh
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

    if (!window) {
        std::cout << "couldnt make GLFW window" << std::endl;
        glfwTerminate();
        return -2543;
    }

    glfwMakeContextCurrent(window); // draws window (?)

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Couldnt initialise GLAD" << std::endl;
        return -352084;
    }

    glViewport(0, 0, 800, 600); // glad viewport same as glfw window size
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // resize viewport when resizing window

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // could use element buffer objects to eliminate overlap when drawing more than 1 triangle
    //float t1vertices[] = { // vertices
    //    // t1
    //        //position          //colour              // texture coords
    //     0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f, // top right 1
    //     0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, // bottom right 1
    //    -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f, // bottom right 1
    //    -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 0.0f,     0.0f, 1.0f  // top left 1
    //};



    // could use element buffer objects to eliminate overlap when drawing more than 1 triangle
    float t1vertices[] = { // vertices
        // t1
            //position          //colour              // texture coords
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,         1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 0.0f,         1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,         0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,         0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,         0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,         0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,         0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,         1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 1.0f, 0.0f,         1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,         0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,         1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,         1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 0.0f,         1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,         0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,         0.0f, 1.0f
    };


    float lightCubeVertices[]{
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




    float t2vertices[] = { // vertices
        // t2
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top left 2
         0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom left 2
         0.25f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f // bottom right 2
    };

    unsigned int indices[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3  // 2nd triangle
    };

    //float vertices[] = { // unique vertices
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};

    //compile vertex shader

    shader ourShader("./shaders/vShader.glsl", "./shaders/fShader.glsl");
    float offset = 0.25f;

    // vertex buffer object
    unsigned int VBO[2], VAO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    //t1
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(t1vertices), t1vertices, GL_STATIC_DRAW);
    // link vertex attributes
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    ////t2
    //glBindVertexArray(VAO[1]);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(t2vertices), t2vertices, GL_STATIC_DRAW);
    //// link vertex attributes
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    ////colour
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    // element buffer object for drawing more vertices at once
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) // wireframe

    //glUseProgram(shaderProgram);

    //texture code

    float texCoords[] = {
        0.0f, 0.0f, // lower left corner
        1.0f, 1.0f, // lower right corner
        0.5f, 1.0f  // top centre corner
    };



    unsigned int texture; // texture object referenced as num id
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //texture options see middle argument for what each option pertains to
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    int width, height, noChannels;
    unsigned char* data = stbi_load("./container.jpg", &width, &height, &noChannels, 0); // last arg is ?


    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "couldnt load box texture" << endl;
    }

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    //texture options see middle argument for what each option pertains to
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("./awesomeface.png", &width, &height, &noChannels, 0);
    if (data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "couldnt load face texture" << endl;
    }

    stbi_image_free(data);
    stbi_image_free(data2);

    ourShader.use();
    ourShader.setInt("ourTexture", 0); // assign textures to shaders
    ourShader.setInt("ourTexture2", 1);
    ourShader.setFloat("xOffset", offset);

    //---------------------------------------------------------------------------------------

    // Lighting


    unsigned int lightVBO;
    unsigned int lightVAO;

    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);



    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // only bind light vao cos the container's vao data already contains required info.
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    // set vert attr
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);


    shader lightCubeShader("./shaders/vShaderLightingContainer.glsl", "./shaders/fShaderLightingContainer.glsl");

    

    //have to make the second set of lighting shaders now.

    //---------------------------------------------------------------------------------------

    unsigned int projectionLocation = glGetUniformLocation(ourShader.id, "projection");
    
    //ourShader.setMat4("projection", mat4(1.0f));
    //ourShader.setMat4("projection", proj);

    vec3 cubePositions[] = {
        vec3(0.0f,  0.0f,  0.0f),
        vec3(2.0f,  5.0f, -15.0f),
        vec3(-1.5f, -2.2f, -2.5f),
        vec3(-3.8f, -2.0f, -12.3f),
        vec3(2.4f, -0.4f, -3.5f),
        vec3(-1.7f,  3.0f, -7.5f),
        vec3(1.3f, -2.0f, -2.5f),
        vec3(1.5f,  2.0f, -2.5f),
        vec3(1.5f,  0.2f, -1.5f),
        vec3(-1.3f,  1.0f, -1.5f)
    };



    // main render loop
    while (!glfwWindowShouldClose(window)) { // will keep drawing window on the display until the user closes it
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f); //state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using 


        ourShader.use();


        // i have all the colours tho
        /*lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);*/

        mat4 proj = perspective(radians(camera.Zoom), ((float)width) / ((float)height), 0.1f, 100.0f); // makes view frustrum
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
        mat4 view(1.0f);
        // Don't forget to replace glm::lookAt with your own version
        // view = calculate_lookAt_matrix(vec3(camx, 0.0f, camz), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); broken for some reason
        view = camera.GetViewMatrix();

        string name = "view";
        glUniformMatrix4fv(glGetUniformLocation(ourShader.id, name.c_str()), 1, GL_FALSE, &view[0][0]);
        //lightCubeShader.setMat4("view", view);

        /*int vertexColourLocation2 = glGetUniformLocation(t2shaderProgram, "ourColour2");*/

        // draw the triangle(s)
        //glUniform4f(vertexColourLocation, 0.0f, greenValue, 0.0f, 1.0f);

        //ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture); //activate texture before binding
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);


        ourShader.setFloat("tra", transparency);


        //glDrawArrays(GL_TRIANGLES, 0, 3);

        //glUseProgram(t2shaderProgram);


        vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
        mat4 trans = mat4(1.0f);
        //trans = translate(trans, vec3(1.0f, 1.0f, 0.0f));
        //vec = trans * vec;
        //cout << vec.x << vec.y << vec.z << endl;

        /*trans = rotate(trans, radians(90.0f), vec3(0.0, 0.0, 1.0));
        trans = scale(trans, vec3(0.5, 0.5, 0.5));*/

        trans = rotate(trans, /*(float)glfwGetTime() * 6.0f*/ (float)pow(glfwGetTime(), 2), vec3(0.0f, 0.0f, 1.0f));
        trans = translate(trans, vec3(0.5f, -0.5f, 0.0f));
        unsigned int transformLocation = glGetUniformLocation(ourShader.id, "transform");
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, value_ptr(trans));

        glBindVertexArray(VAO[0]);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        trans = mat4(1.0f);
        trans = translate(trans, vec3(-0.5f, 0.5f, 0.0f));
        float scaleNum = static_cast<float>(sin((float)glfwGetTime()));
        trans = scale(trans, vec3(abs(scaleNum), abs(scaleNum), abs(scaleNum)));
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &trans[0][0]);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        mat4 model(1.0f);
        model = rotate(model, (float)glfwGetTime() * radians(50.0f), vec3(0.5f, 1.0f, 0.0f));

        glBindVertexArray(VAO[0]);
        for (unsigned int i = 0; i < 10; i++)
        {
            mat4 model = mat4(1.0f);
            model = translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
            unsigned int modelLocation = glGetUniformLocation(ourShader.id, "model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // "this, now this is the light of the world" - James May c. early 21st century
        // the light of the world has broken everything
        ourShader.setVec3("lightColour", 1.0f, 1.0f, 1.0f);
        lightCubeShader.use();


        name = "lightProjection";
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.id, name.c_str()), 1, GL_FALSE, &proj[0][0]);
        //lightCubeShader.setMat4("projection", projection);

        name = "lightView";
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.id, name.c_str()), 1, GL_FALSE, &view[0][0]);
        //lightCubeShader.setMat4("view", view);


        mat4 lightModel(1.0f);
        name = "lightModel";
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.id, name.c_str()), 1, GL_FALSE, &lightModel[0][0]);
        lightModel = translate(lightModel, lightPos);
        lightModel = scale(lightModel, vec3(0.2f)); // 20% of the size of other cubes

        //lightCubeShader.setMat4("model", lightModel);

        glBindVertexArray(lightVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);



        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}
