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
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "program.h"
#include <typeinfo>
using namespace std;
using namespace glm;

float transparency = 0.5f;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


vec3 cameraPos(0.0f, 0.0f, 3.0f);
vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

// untie framerate to the movement speed
float deltaTime = 0.0f; // time between the last and current frame
float lastFrame = 0.0f; // time of last frame

//for mouse movement
Camera camera(vec3(0.0f, 0.0f, 3.0f));
//float lastX = 400, lastY = 300;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float pitch1 = 0.0f, yaw1 = -90.0f;
float mouseEntered = true;

// for zoom
//float fov = 90.0f;

// the virtual sun
vec3 lightPos(1.0f, 1.0f, 2.0f);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

}


void mouse_callback(GLFWwindow* window, double x, double y) {
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
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
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

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); // glad viewport same as glfw window size
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
            //position          //colour              // texture coords     // normal vector coordinates for lighting
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f,       1.0f, 1.0f, /*0.0f,*/        0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 0.0f,       1.0f, 1.0f, /*0.0f,*/        0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,       1.0f, 1.0f, /*0.0f,*/        0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 0.0f,       1.0f, 1.0f, /*0.0f,*/        0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 1.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,       0.0f, 1.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 1.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,       0.0f, 1.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 1.0f, /*0.0f,*/        0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,       1.0f, 0.0f, /*0.0f,*/        0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 1.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        0.0f, -1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 1.0f, /*0.0f,*/        0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,       1.0f, 0.0f, /*0.0f,*/        0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 0.0f,       1.0f, 0.0f, /*0.0f,*/        0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f, /*0.0f,*/        0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 1.0f, /*0.0f,*/        0.0f,  1.0f,  0.0f
    };

    unsigned int indices[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3  // 2nd triangle
    };

    //compile vertex shader

    shader ourShader("./shaders/vShader.glsl", "./shaders/fShader.glsl");

    glEnable(GL_DEPTH_TEST);
    
    float offset = 0.25f;

    // vertex buffer object
    unsigned int VBO[1], VAO[1];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    //t1
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(t1vertices), t1vertices, GL_STATIC_DRAW);
    // link vertex attributes
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // colour attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // lighting attribute ignore this? but no......
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

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
    unsigned char* data = stbi_load("./container.jpg", &width, &height, &noChannels, 0); // last arg is ? dont change last argument id breaks everything lmao


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

    // instead of using lightvbo, just use the OG???
    unsigned int lightVAO;

    shader lightCubeShader("./shaders/vShaderLightingContainer.glsl", "./shaders/fShaderLightingContainer.glsl");
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    //glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);

    // only bind light vao cos the container's vao data already contains required info.
    // set vert attr
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /*6*/ 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    lightCubeShader.use();
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

        //glClearColor(0.1f, 0.3f, 0.3f, 1.0f); //state-setting function
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using 


        ourShader.use();

        mat4 proj = perspective(radians(camera.Zoom), ((float)width) / ((float)height), 0.1f, 100.0f); // makes view frustrum
        //glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
        ourShader.setMat4("projection", proj);

        // Don't forget to replace glm::lookAt with your own version
        mat4 view(1.0f);
        view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

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

        //trans = rotate(trans, /*(float)glfwGetTime() * 6.0f*/ (float)pow(glfwGetTime(), 1), vec3(0.0f, 0.0f, 1.0f)); // singular rotation (not needed?)
        trans = translate(trans, vec3(0.5f, -0.5f, 0.0f));
        ourShader.setMat4("transform", trans);

        glBindVertexArray(VAO[0]);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        trans = mat4(1.0f);
        trans = translate(trans, vec3(-0.5f, 0.5f, 0.0f));
        //float scaleNum = static_cast<float>(sin((float)glfwGetTime())); // grows and shrinks with time
        //trans = scale(trans, vec3(abs(scaleNum), abs(scaleNum), abs(scaleNum)));
        //glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &trans[0][0]); // i think this another separate instance of a cube...

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        mat4 model(1.0f);
        model = rotate(model, (float)glfwGetTime() * radians(0.0f), vec3(0.5f, 1.0f, 0.0f));

        glBindVertexArray(VAO[0]);
        for (unsigned int i = 0; i < 1; i++)
        {
            mat4 model = mat4(1.0f);
            model = translate(model, cubePositions[i]);
            //float angle = 20.0f * i; // another angle rotation 
            //model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
            unsigned int modelLocation = glGetUniformLocation(ourShader.id, "model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // "this, now this is the light of the world" - James May c. early 21st century
        // the light of the world has broken everything. nvm i fixed the problems lmao
        ourShader.setVec3("lightColour", 1.0f, 1.0f, 1.0f);
        //cout << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z << endl;
        ourShader.setVec3("viewPos", camera.Position);

        lightCubeShader.use();

        lightCubeShader.setMat4("lightProjection", proj);

        lightCubeShader.setMat4("lightView", view);
        cout << glGetError() << endl;  // gives 0
        //lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        //lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        cout << typeid(lightPos).name() << endl;
        ourShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        cout << glGetError() << endl;  // gives 1282

        mat4 lightModel(1.0f);
        lightModel = translate(lightModel, lightPos);
        lightModel = scale(lightModel, vec3(0.2f)); // 20% of the size of other cubes
        cout << glGetError() << endl;  // gives 0
        lightCubeShader.setMat4("lightModel", lightModel);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); 
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        cout << "_______________________________________" << endl;
    }

    glfwTerminate();
    return 0;
}
