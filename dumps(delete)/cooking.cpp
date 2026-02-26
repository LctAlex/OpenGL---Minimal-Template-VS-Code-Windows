#include <iostream>
#include <vector>
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

//these will be classes eventually

class VBO
{
    private:
    unsigned int ID;
    public:
    VBO(const std::vector<float>& vertices) //could make them constant
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

class BO //Buffer-Object (VBO or EBO) ...
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

class VAO
{
    private:
    unsigned int ID;
    public:
    VAO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    {
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
        VBO vbo(vertices);
        EBO ebo(indices);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0*sizeof(float))); //figure this out
        glEnableVertexAttribArray(0);

        Unbind();
        vbo.Unbind();
        ebo.Unbind();
    }

    VAO(VBO vbo, EBO ebo)
    {
        vbo.Unbind(); //this approach should ensure Unbinding VBO & EBO at initialization
        ebo.Unbind(); //to avoi uncesesarry calls (like these)
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
};

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

    const std::vector<float> vertices ={ //upgrade
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };

    const std::vector<unsigned int> indices ={
        0, 1, 2,
        0, 2, 3
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    VBO vbo(vertices);
    EBO ebo(indices);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0*sizeof(float)));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    vbo.Unbind();
    ebo.Unbind();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glUseProgram(shaderProgram);
        glLineWidth(10); //Damn GLAD I didn't know you were chill like that!
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    vbo.Delete();
    ebo.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}