#include <iostream>
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

GLFWwindow* InitWindow(int width, int height, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("ERROR::GLFW::FAILED_INITIALIZATION\n");
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("ERROR::GLAD::FAILED_LOADING\n");
    }

    glViewport(0, 0, width, height);
    return window;
}

void InitVBO(unsigned int* ID, const float* vertices)
{
    glGenBuffers(1, ID);
    glBindBuffer(GL_ARRAY_BUFFER, *ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InitVAO(unsigned int* ID, unsigned int* VBO, const float* vertices, int n)
{
    glGenVertexArrays(1, ID);
    glBindVertexArray(*ID);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    
}

unsigned int HardcodedShader()
{
    int success = 1;
    char infoLog[256];

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "out vec3 PosColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   PosColor = aPos;\n"
    "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 PosColor;\n"
    "void main()\n"
    "{\n"
    "   //FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "   FragColor = vec4(PosColor, 1.0f);\n"
    "}\0";

    unsigned int vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vShader);
    
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vShader, 256, NULL, infoLog);
        std::cout << infoLog << '\n';
        throw std::runtime_error("ERROR::SHADER::VERTEX::COMPILATION_FAILED"); //just to be extra safe ahh
    }

    unsigned int fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fShader);

    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fShader, 256, NULL, infoLog);
        std::cout << infoLog << '\n';
        throw std::runtime_error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 256, NULL, infoLog);
        std::cout << infoLog << '\n';
        throw std::runtime_error("ERROR::SHADER:::PROGRAM::LINKING_FAILED: ");
    }

    return shaderProgram;
}

int main()
{
    int screen = 800;
    GLFWwindow* window; 
    try {window = InitWindow(screen, screen, "Hey");}
    catch(const std::exception& e) {std::cerr << e.what() << '\n';}
    
    unsigned int shaderProgram;
    try {shaderProgram = HardcodedShader();}
    catch(const std::exception& e) {std::cerr << e.what() << '\n';}

    const float vertices[]={
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };

    unsigned int indices[]={
        0, 1, 2,
        0, 2, 3
    };

    //Actively putting VBO and EBO in VAO

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0*sizeof(float)));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); //unbinding, now everthing is saved inside VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        glLineWidth(10); //Damn GLAD I didn't know you were chill like that!
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}