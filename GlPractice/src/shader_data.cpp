#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader_data.h"

/// <summary>
/// Custom shader data, not just the points coordinates
/// </summary>
namespace shader_data {

	GLFWwindow* window;
	// hint: could just attach and detach the yellow and orange shaders
	unsigned int shaderProgram;
	unsigned int VAO;

	const char* vertexShaderSrc = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 vtxColor;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.0);
    vtxColor = vec4(aColor, 1.0);
}
)";
	const char* fragmentShaderSrc = R"(
#version 330 core

in vec4 vtxColor;

out vec4 FragColor;

void main()
{
    FragColor = vtxColor;
}
)";

	int main() {
		// 1. create a window, initialize OpenGL
		if (initContext() != 0) return -1;
		// 2. black background color
		glClearColor(.0f, .0f, .0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// 3. initialize shaders
		if (initShaders() != 0) return -1;
		// 4. create two triangles
		initVAOs();

		// 5. create render loop
		while (!glfwWindowShouldClose(window))
		{
			// 6. render the triangles
			renderTriangles();

			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, true);
			}

			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		// 7. clean resources
		glfwTerminate();

		return 0;
	}

	void renderTriangles()
	{
		glUseProgram(shaderProgram);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
	}

	void initVAOs() {
		// create the two triangles in RAM
		float vertices[] = {
			// positions          // colors
			-.5f, -.5f, .0f,      1.f, .0f, .0f,
			.5f, -.5f, .0f,       .0f, 1.f, .0f,
			.0f, .5f, .0f,        .0f, .0f, 1.f
		};

		// create VBO
		unsigned int VBO;
		glGenBuffers(1, &VBO);

		// create VAO
		glGenVertexArrays(1, &VAO);

		// bind VAO
		glBindVertexArray(VAO);
		// bind VBO to the VAO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// copy the vertices into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// tell OpenGl how to interpret the data in VBO
		// since position isnt the only thing in the VBO, we need to configure the attribute pointers
		// [layout=0], [the data type is float], [do not normalize], [stride is 6 times of float], [first byte of the data]
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// [layout=1], [the data type is float], [do not normalize], [stride is 6 times of float], [start byte of the data]
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// unbind VAO
		glBindVertexArray(0);
	}

	int initShaders() {
		// compile shaders by creating a shader object and attaching the shader sources to them
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
		glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);

		glCompileShader(vertexShader);
		glCompileShader(fragmentShader);

		int success;
		char infoLog[512];

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return 0;
	}

	int initContext() {
		int width = 800, height = 800;
		// init glfw
		glfwInit();
		// hint window for OpenGl version 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// hint window to use core profile
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// request glfw to create a window
		window = glfwCreateWindow(width, height, "Hello triangle excercise", NULL, NULL);
		// check for error during window creation
		if (window == NULL)
		{
			std::cout << "ERROR::WINDOW::FAILED_TO_CREATE" << std::endl;
			glfwTerminate();
			return -1;
		}
		// set the current context to the created window
		glfwMakeContextCurrent(window);
		// initialize glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "ERROR::GLAD::FAILED_TO_INITIALIZE" << std::endl;
			glfwTerminate();
			return -1;
		}

		glViewport(0, 0, width, height);

		return 0;
	}

}