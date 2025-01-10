// look dude, i raised this from the ground with my own two bare hands ahahhaha
//
//Z:\ok yes\projects\tutorial\Libs\glfw3.lib linker entry for linux
//C:\Users\zain\Desktop\opengl\projects\tutorial\Libs\glfw3.lib linker entry for windows

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <shaderClass/shader.h>
#include <iostream>
#include <math.h>
#include <filesystem>
#define  STB_IMAGE_IMPLEMENTATION
#include <shaderClass/stb_image.h>



//const char* vertexShaderSource =
//    "#version 330 core\n"
//    "layout (location = 0) in vec3 aPos;\n"
//    "layout (location = 1) in vec3 aColor;\n"
//    "out vec3 ourColor;\n" // output to fragment shaders
//    "void main()\n"
//    "{\n"
//    "   gl_Position = vec4(aPos, 1.0);\n"
//    "   ourColor = aColor;\n"
//    "}\0";
//
//const char* fragmentShaderSource = 
//    "#version 330 core\n"
//    "out vec4 FragColor;\n"
//    "in vec3 ourColor;\n"
//    "void main()\n"
//    "{\n"
//    "   FragColor = vec4(ourColor, 1.0f);\n"
//    "}\0";
//
//const char* t2fragmentShaderSource =
//    "#version 330 core\n"
//    "out vec4 FragColor;\n"
//    "in vec3 ourColor;\n"
//    // "uniform vec4 ourColour2;\n"
//    "void main()\n"
//    "{\n"
//    "    FragColor = vec4(ourColor, 1.0f);\n"
//    // "    FragColor = ourColour2;\n"
//    "}\0"
//;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // need to put external functions before the main one for some reason
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
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

    // could use element buffer objects to eliminate overlap when drawing more than 1 triangle
    float t1vertices[] = { // vertices
        // t1
            //position          //colour    // texture coords
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right 1
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right 1
       -0.25f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right 1
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left 1
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

    //unsigned int indices[] = {
    //    0, 1, 3, // 1st triangle
    //    1, 2, 3  // 2nd triangle
    //};

    //compile vertex shader

    shader ourShader("./shaders/vShader.glsl", "./shaders/fShader.glsl");
    float offset = 0.5f;

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
    //t2
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(t2vertices), t2vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //colour
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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


    int width, height, noChannels;
    unsigned char* data = stbi_load("./container.jpeg", &width, &height, &noChannels, 0); // last arg is ?

    unsigned int texture; // texture object referenced as num id
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    //texture options see middle argument for what each option pertains to
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "couldnt load texture" << endl;
    }

    stbi_image_free(data);


    // main render loop
    while (!glfwWindowShouldClose(window)) { // will keep drawing window on the display until the user closes it
        processInput(window);

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f); //state-setting function
        glClear(GL_COLOR_BUFFER_BIT); // state-using 

        /*float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
        int vertexColourLocation = glGetUniformLocation(shaderProgram, "ourColour");
        int vertexColourLocation2 = glGetUniformLocation(t2shaderProgram, "ourColour2");*/

        // draw the triangle(s)
        //glUniform4f(vertexColourLocation, 0.0f, greenValue, 0.0f, 1.0f);
        ourShader.use();
        ourShader.setFloat("xOffset", offset);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO[0]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        //glUseProgram(t2shaderProgram);
        //glUniform4f(vertexColourLocation2, 0.0f, greenValue, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}