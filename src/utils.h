#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class ShaderProgram
{
public:
    ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
    {
        int successCompile;

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successCompile);
        if (!successCompile)
            std::cout << "Failed compile vertex shader." << std::endl;

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successCompile);
        if (!successCompile)
            std::cout << "Failed compile fragment shader." << std::endl;

        m_id = glCreateProgram();
        glAttachShader(m_id, vertexShader);
        glAttachShader(m_id, fragmentShader);

        glLinkProgram(m_id);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    unsigned int id() const { return m_id; }

    void use() const
    {
        glUseProgram(m_id);
    }
private:
    unsigned int m_id;
};

struct Texture
{
    unsigned int id;
    int width;
    int height;
    int channels;
};

class IDrawable
{
public:
    virtual void draw();
};

class Plane
{
public:
    Plane(float width = 1.0f, float height = 1.0f) : m_width(width), m_height(height)
    {
        float halfWidth = width / 2;
        float halfHeight = height / 2;

        float vertices[] = {
            -halfWidth, -halfHeight, 0,  0.0f, 0.0f,
             halfWidth, -halfHeight, 0,  1.0f, 0.0f,
             halfWidth,  halfHeight, 0,  1.0f, 1.0f,
            -halfWidth,  halfHeight, 0,  0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(0)
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float))
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    ~Plane()
    {
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        glDeleteVertexArrays(1, &m_VAO);
    }

    Plane(const Plane&) = delete;

    float width() const { return m_width; }
    float height() const { return m_height; }

    void draw()
    {
        glBindVertexArray(m_VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    Plane& operator=(const Plane&) = delete;

private:
    float m_width;
    float m_height;
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
};

class Cube
{
public:
    Cube()
    {
        static float vertices[] = {
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,

             0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
        };

        static unsigned int indices[] = {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(0)
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float))
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    Cube(const Cube&) = delete;

    ~Cube()
    {
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        glDeleteVertexArrays(1, &m_VAO);
    }

    void draw()
    {
        glBindVertexArray(m_VAO);

        glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, nullptr);
    }

    Cube& operator=(const Cube&) = delete;
private:
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
};
