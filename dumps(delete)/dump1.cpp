#include <iostream>
#include <../include/glad/glad.h>
#include <../include/GLFW/glfw3.h>

//should turn this into a class
//private member: GLFWwindow* window
//cleanup(){glfwDestroyWindow(window); glfwTerminate();}
GLFWwindow* WindowInit(int width, int height, const char* string)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, string, NULL, NULL);
    if(window == NULL)
    {
        std::cout <<"Failed to initialize GLFW\n";
        glfwTerminate();
        throw std::runtime_error("ERROR::GLFW::FAILED_INITIALIZATION\n");
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout <<"Failed to initialize GLAD\n";
        throw std::runtime_error("ERROR::GLAD::FAILED_LOADING\n");
    }

    glViewport(0, 0, width, height);
    //glClearColor(1.0f, 0.5f, 0.3f, 1.0f);
    return window;
}

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n\tFragColor = vec4(1.0, 0.0, 0.0, 1.0);\n}\0";

void InitVBO(unsigned int* ID, const float* vertices)
{
    glGenBuffers(1, ID);
    glBindBuffer(GL_ARRAY_BUFFER, *ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// InitVAO(unsigned int* ID, unsigned int &VBO, const float* vertices)
// {

// }

int main()
{
    int success = 1;
    char infoLog[256];

    int screen = 800;
    GLFWwindow* window = WindowInit(screen, screen, "Heck yeah!");

    // const float vertices[] = {
    //     0.5f, 0.5f, 0.0f,
    //     -0.5f, 0.5f, 0.0f,
    //     0.0f, -0.5f, 0.0f
    // };

    const float vertices[] ={
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    //Vertex Buffer Object
    unsigned int VBO;
    InitVBO(&VBO, vertices);

    //vertex Shader
    unsigned int vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vShader);
    //error check
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vShader, 256, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog << '\n';
    }

    //fragment Shader
    unsigned int fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fShader);
    //error check
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fShader, 256, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog << '\n';
    }

    //Shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    //error check
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 256, NULL, infoLog);
        std::cout << "ERROR::SHADER:::PROGRAM::LINKING_FAILED: " << infoLog << '\n';
    }

    //Cleaning up shaders (we already have the program)
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    //Vertex Attributes
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    //VAO: Combining Vertex Atributes and Vertex Buffer Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //here we 'link' VAO with VBO vvv
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glClearColor(1.0f, 0.5f, 0.3f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//Element buffer object:

// float vertices[] = {
//      0.5f,  0.5f, 0.0f,  // top right
//      0.5f, -0.5f, 0.0f,  // bottom right
//     -0.5f, -0.5f, 0.0f,  // bottom left
//     -0.5f,  0.5f, 0.0f   // top left 
// };
// unsigned int indices[] = {  // note that we start from 0!
//     0, 1, 3,   // first triangle
//     1, 2, 3    // second triangle
// };

// unsigned int EBO;
// glGenBuffers(1, &EBO);
// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// glUseProgram(shaderProgram);
// glBindVertexArray(VAO);
// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
// glBindVertexArray(0);