//You can delete this but DON'T delete glad.c !
#include <iostream>
#include <vector>
#include <math.h>

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#include "../include/glm/glm.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define TINIYOBJLOADER_IMPLEMENTATION
#include "../include/tiny_obj_loader.h"

//EVERYTHING should to go inside a class (ex. GLFWwindow window, float deltaTime(time between frames) as members)
//class will handle everything

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

class VBO
{
    private:
    unsigned int ID;
    public:
    VBO(const std::vector<float>& vertices)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID); //Bind();
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices.data()[0]), vertices.data(), GL_STATIC_DRAW); //need to return size of DATA, not pointer
    }
    void Bind(){glBindBuffer(GL_ARRAY_BUFFER, ID);}
    void Unbind(){glBindBuffer(GL_ARRAY_BUFFER, 0);}
    void Delete(){glDeleteBuffers(1, &ID);}
    ~VBO(){glDeleteBuffers(1, &ID);}
};

class EBO
{
    private:
    unsigned int ID;
    public:
    EBO(const std::vector<unsigned int>& indices)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices.data()[0]), indices.data(), GL_STATIC_DRAW);
    }
    void Bind(){glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);}
    void Unbind(){glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);}
    void Delete(){glDeleteBuffers(1, &ID);}
    ~EBO(){glDeleteBuffers(1, &ID);}
};

class BO //General Buffer-Object (VBO or EBO) ...
{
    private:
    unsigned int ID;
    GLenum target;
    public:
    template <typename T> //float => VBO, uint =>EBO
    BO(const std::vector<T>& data, GLenum target)
    :target(target)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(target, ID);
        glBufferData(target, data.size() * sizeof(data.data()[0]), data.data(), GL_STATIC_DRAW);
    }
    void Bind(){glBindBuffer(target, ID);}
    void Unbind(){glBindBuffer(target, 0);}
    void Delete(){glDeleteBuffers(1, &ID);}
    ~BO(){glDeleteBuffers(1, &ID);}
};

class VAO //'offsetof' macro would work here for (void*)!!!
{
    private:
    unsigned int ID;
    public:

    VAO(const std::vector<float>& vertices, int size, int stride, const std::vector<unsigned int>& indices) //size = per group \n stride = per line
    {
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
        VBO vbo(vertices);
        EBO ebo(indices);
        for(int i = 0; i < stride/size; i++) //going through each possible data inside VBO (size is equal for all, but what about TexCoords?)
        {
            glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(i*size*sizeof(float)));
            glEnableVertexAttribArray(i);
        }

        Unbind();
        vbo.Unbind();
        ebo.Unbind();
        vbo.Delete(); //allowed since 3.3 OpenGL
        ebo.Delete();
    }

    VAO(std::vector<float>& vertices, int size, int stride, std::vector<unsigned int>& indices) //NON-CONSTANTS version
    {
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
        VBO vbo(vertices);
        EBO ebo(indices);
        for(int i = 0; i < stride/size; i++)
        {
            glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(i*size*sizeof(float)));
            glEnableVertexAttribArray(i);
        }

        Unbind();
        vbo.Unbind();
        ebo.Unbind();
        vbo.Delete(); //allowed since 3.3 OpenGL
        ebo.Delete();
    }

    VAO(VBO vbo, EBO ebo)
    {
        vbo.Unbind(); //this approach should ensure Unbinding VBO & EBO at initialization
        ebo.Unbind(); //to avoid uncesesarry calls (like these)
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
        vbo.Bind();
        ebo.Bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0*sizeof(float))); //figure this out
        glEnableVertexAttribArray(0);
        Unbind();
    }

    void Bind(){glBindVertexArray(ID);}
    void Unbind(){glBindVertexArray(0);}
    void Delete(){glDeleteVertexArrays(1, &ID);}
    ~VAO(){glDeleteVertexArrays(1, &ID);}
};

#define PI 3.14159f
#define DEG2RAD (PI/180.f)

void CircleData(std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    const float center[] = {0.0f, 0.0f, 0.0f};
    float radius = 0.75f;
    int sectors = 50;

    //these will always be pushed first (all triangles have in common)
    vertices.push_back(center[0]);
    vertices.push_back(center[1]);
    vertices.push_back(center[2]);
    //indices.push_back(0);

    for(int i = 0; i < sectors; i++)
    {
        float angle = i*(360.f/sectors);
        float xAxis = center[0] + radius * cos(angle*DEG2RAD); //cos and sin expect RADIANS...
        float yAxis = center[1] + radius * sin(angle*DEG2RAD); //draw relative to center

        vertices.push_back(xAxis);
        vertices.push_back(yAxis);
        vertices.push_back(0.0f); //Z axis
    }
    for(int i = 1; i <= sectors; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back((i % sectors) + 1);
    }
}

unsigned int HardcodedShader()
{
    int success = 1;
    char infoLog[256];

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "out vec2 PosColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   PosColor = aPos.xy;\n"
    "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 PosColor;\n"
    "uniform float time;\n"
    "void main()\n"
    "{\n"
    "   //FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "   FragColor = vec4(PosColor.x+sin(time/2.0)+0.5, PosColor.y+cos(time/2.0)+0.5, 0.5, 1.0f);\n"
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

typedef struct
{
    float lastFrameTime = 0.f;
    //float lastUpdateTime = 0.f; //but I'll update stuff AND frames at the same time
    float fpsLimit = 1.f/60.f;
    double deltaTime = 0.f;
    double now = 0.f;
}Frames;

Frames frames;

int main()
{
    int screen = 800;
    GLFWwindow* window; 
    try {window = InitWindow(screen, screen, "Hey");}
    catch(const std::exception& e) {std::cerr << e.what() << '\n';}
    
    unsigned int shaderProgram;
    try {shaderProgram = HardcodedShader();}
    catch(const std::exception& e) {std::cerr << e.what() << '\n';}

    // const std::vector<float> vertices ={ //upgrade
    //     0.5f, 0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     -0.5f, -0.5f, 0.0f,
    //     -0.5f, 0.5f, 0.0f
    // };

    // const std::vector<unsigned int> indices ={
    //     0, 1, 2,
    //     0, 2, 3
    // };

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    CircleData(vertices, indices);

    VAO vao(vertices, 3, 3, indices);

    int timeLoc = glGetUniformLocation(shaderProgram, "time");
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        frames.now = glfwGetTime();
        frames.deltaTime = frames.now - frames.lastFrameTime;
        if(frames.deltaTime >= frames.fpsLimit)
        {
            glClear(GL_COLOR_BUFFER_BIT);

            vao.Bind();
            glUseProgram(shaderProgram);
            glUniform1f(timeLoc, glfwGetTime());
            glLineWidth(5); //Damn GLAD I didn't know you were chill like that!
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            vao.Unbind();

            glfwSwapBuffers(window);
            glfwPollEvents();
            frames.lastFrameTime = frames.now;
        }
    }

    vao.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}