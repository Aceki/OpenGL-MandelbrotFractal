#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cfloat>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"

int windowWidth = 1280;
int windowHeight = 720;

int mandelbrotIterationsCount = 200;
double mandelbrotScale = 0.5f;
float mandelbrotScaleCoef = 0.02f;
float mandelbrotMoveSpeed = 0.01f;
glm::dvec2 mandelbrotTranslate = glm::dvec2(0.0f);
float mandelbrotConditionValue = 4.0f;

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model = mat4(1.0f);
uniform mat4 view = mat4(1.0f);
uniform mat4 projection = mat4(1.0f);

void main()
{
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0f);
}
)";

const char* fragmentShaderSource = R"(
#version 440 core

uniform vec2 resolution;
uniform int mandelbrotIterationsCount = 200;
uniform double mandelbrotScale = 0.5f;
uniform dvec2 mandelbrotTranslate = dvec2(0.0f);
uniform float mandelbrotConditionValue = 4.0f;

out vec4 fragmentColor;

const vec3 color1 = vec3(1.0f, 1.0f, 1.0f);
const vec3 color2 = vec3(0.0f, 0.0f, 0.0f);
const vec3 color3 = vec3(0.0f, 0.0f, 0.0f);

float mandelbrot(dvec2 c)
{
    dvec2 z = dvec2(0.0f);

    for (int i = 0; i < mandelbrotIterationsCount; i++)
    {
        if (dot(z, z) >= mandelbrotConditionValue)
            return float(i) / mandelbrotIterationsCount;
        else
            z = dvec2((z.x * z.x) - (z.y * z.y) + c.x, 2.0f * z.x * z.y + c.y);
    }

    return -1.0f;
}

void main()
{
    float aspect = resolution.x / resolution.y;

    dvec2 c = (dvec2(gl_FragCoord.xy) / dvec2(resolution.xy)) - 0.5f;
    c /= mandelbrotScale;
    c += mandelbrotTranslate;
    c.x *= aspect;

    vec3 color = vec3(0.0f);
    float intensity = mandelbrot(c);

    if (intensity < 0)
        color = color3;
    else
        color = color1 * (1.0f - intensity) + color2 * intensity;

    fragmentColor = vec4(color, 1.0f);
}
)";

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() 
{
    glfwInit(); // Инициализация библиотеки GLFW

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Lab4", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Сделать контекст окна главным в текущем потоке

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    ShaderProgram shaderProgram(vertexShaderSource, fragmentShaderSource);

    Plane plane(1.0f);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        shaderProgram.use();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int modelLocation = glGetUniformLocation(shaderProgram.id(), "model");
        int viewLocation = glGetUniformLocation(shaderProgram.id(), "view");
        int projectionLocation = glGetUniformLocation(shaderProgram.id(), "projection");
        int resolutionLocation = glGetUniformLocation(shaderProgram.id(), "resolution");
        int mandelbrotIterationsCountLocaiton = glGetUniformLocation(shaderProgram.id(), "mandelbrotIterationsCount");
        int mandelbrotScaleLocation = glGetUniformLocation(shaderProgram.id(), "mandelbrotScale");
        int mandelbrotTranslateLocation = glGetUniformLocation(shaderProgram.id(), "mandelbrotTranslate");
        int mandelbrotConditionValueLocation = glGetUniformLocation(shaderProgram.id(), "mandelbrotConditionValue");

        glm::mat4 model(1.0f);
        model = glm::translate(model, { 0.0f, 0.0f, -99.0f });
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 view = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, 0.1f, 100.0f);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection(1.0f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::vec2 resolution = glm::vec2(windowWidth, windowHeight);
        glUniform2fv(resolutionLocation, 1, glm::value_ptr(resolution));

        glUniform1i(mandelbrotIterationsCountLocaiton, mandelbrotIterationsCount);

        glUniform1d(mandelbrotScaleLocation, mandelbrotScale);

        glUniform2dv(mandelbrotTranslateLocation, 1, glm::value_ptr(glm::dvec2(mandelbrotTranslate)));

        glUniform1f(mandelbrotConditionValueLocation, mandelbrotConditionValue);

        plane.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

void processInput(GLFWwindow* window)
{
    float translate = mandelbrotMoveSpeed * (1.0f / mandelbrotScale);
    if (translate <= 0)
        translate = 0.0001f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mandelbrotScale += mandelbrotScaleCoef * mandelbrotScale;
        if (mandelbrotScale >= 1e+15)
        {
            mandelbrotScale = 0.1f;
            mandelbrotTranslate = glm::dvec2(0.0f);
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && mandelbrotScale > (0.0f + 0.01f))
        mandelbrotScale -= mandelbrotScaleCoef * mandelbrotScale;
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mandelbrotTranslate.y += translate;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mandelbrotTranslate.x -= mandelbrotMoveSpeed * (1.0f / mandelbrotScale);
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mandelbrotTranslate.y -= translate;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mandelbrotTranslate.x += mandelbrotMoveSpeed * (1.0f / mandelbrotScale);
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        mandelbrotIterationsCount += 1;
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && mandelbrotIterationsCount > 0)
        mandelbrotIterationsCount -= 1;
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && mandelbrotConditionValue > 1e-1)
        mandelbrotConditionValue -= 0.01f;
    else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        mandelbrotConditionValue += 0.01f;
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        mandelbrotScale = 0.1f;
        mandelbrotTranslate = glm::dvec2(0.0f);
    }
}
