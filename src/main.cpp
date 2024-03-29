#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "renderer/ShaderProgram.h"
#include "renderer/Texture2D.h"
#include "resources/ResourceManager.h"

GLfloat point[] = {
     0.0f,  50.f, 0.0f,
     50.f, -50.f, 0.0f,
    -50.f, -50.f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

GLfloat texCoord[] = {
    0.5f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
};

glm::ivec2 windowSize(640, 480);

void glfwErrorCallback(int error, const char* description)
{
    std::cout << "Error " << error << ": " << description << std::endl;
}

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    windowSize.x = width;
    windowSize.y = height;
    glViewport(0, 0, windowSize.x, windowSize.y);
}

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char **argv)
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        std::cout << "glfwInit failed" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "Hello World", nullptr, nullptr);
    if (!window)
    {
        std::cout << "glfwCreateWindow failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);

    glfwMakeContextCurrent(window);

    if(!gladLoadGL())
    {
        std::cout << "Can't load GLAD" << std::endl;
        return -1;
    }

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL ver: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(1, 1, 1, 1);

	{
	    ResourceManager resourceManager(argv[0]);
	
	    auto pDefaultShaderProgram = resourceManager.loadShaders("DefaultShader", "res/shaders/vertex", "res/shaders/fragment");
	    if(!pDefaultShaderProgram)
	    {
	        std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
	        return -1;
	    }

        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/map_8x8.png");

	    GLuint points_vbo = 0;
	    glGenBuffers(1, &points_vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
	
	    GLuint colors_vbo = 0;
	    glGenBuffers(1, &colors_vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	
	    GLuint texCoord_vbo = 0;
	    glGenBuffers(1, &texCoord_vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);

	    GLuint vao = 0;
	    glGenVertexArrays(1, &vao);
	    glBindVertexArray(vao);
	    
	    glEnableVertexAttribArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	
	    glEnableVertexAttribArray(1);
	    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	
	    glEnableVertexAttribArray(2);
	    glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
	    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	    pDefaultShaderProgram->use(); 
	    pDefaultShaderProgram->setInt("tex", 0);

        glm::mat4 modelMatrix_1 = glm::mat4(1.f);
        modelMatrix_1 = glm::translate(modelMatrix_1, glm::vec3(100.f, 200.f, 0.f));

        glm::mat4 modelMatrix_2 = glm::mat4(1.f);
        modelMatrix_2 = glm::translate(modelMatrix_2, glm::vec3(590.f, 200.f, 0.f));

        glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(windowSize.x), 0.f, static_cast<float>(windowSize.y), -100.f, 100.f);

        pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

	    while (!glfwWindowShouldClose(window))
	    {
	        glClear(GL_COLOR_BUFFER_BIT);
	
	        pDefaultShaderProgram->use();
	        glBindVertexArray(vao);
	        tex->bind();

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix_1);   
	        glDrawArrays(GL_TRIANGLES, 0, 3);

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix_2);   
            glDrawArrays(GL_TRIANGLES, 0, 3);
	
	        glfwSwapBuffers(window);
	
	        glfwPollEvents();
	    }
    }

    glfwTerminate();
    return 0;
}
