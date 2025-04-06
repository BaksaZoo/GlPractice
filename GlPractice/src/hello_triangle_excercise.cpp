// excercise from https://learnopengl.com/Getting-started/Hello-Triangle (bottom of the page)

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "hello_triangle_excercise.h"

namespace hello_triangle {

	GLFWwindow* window;
	// hint: could just attach and detach the yellow and orange shaders
	unsigned int yellowShaderProgram, orangeShaderProgram;
	unsigned int orangeVAO, yellowVAO;

	const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";
	const char* orangeFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";
	const char* yellowFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
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
		// 5. render the triangles
		renderTriangles();

		glfwSwapBuffers(window);

		// 6. create render loop
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, true);
			}
		}

		// 7. clean resources
		glfwTerminate();

		return 0;
	}

	void renderTriangles() 
	{
		glUseProgram(orangeShaderProgram);
		glBindVertexArray(orangeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(yellowShaderProgram);
		glBindVertexArray(yellowVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
	}

	void initVAOs() {
		// create the two triangles in RAM
		float orangeVertices[] = {
			-.5f, -.5f, .0f,
			.0f, -.5f, .0f,
			-.25f, .0f, .0f
		};
		float yellowVertices[] = {
			.0f, -.5f, .0f,
			.5f, -.5f, .0f,
			.25f, .0f, .0f
		};

		// create VBOs
		unsigned int orangeVBO, yellowVBO;
		glGenBuffers(1, &orangeVBO);
		glGenBuffers(1, &yellowVBO);

		// create VAOs
		glGenVertexArrays(1, &orangeVAO);
		glGenVertexArrays(1, &yellowVAO);

		// bind orange VAO
		glBindVertexArray(orangeVAO);
		// bin VBO to the VAO
		glBindBuffer(GL_ARRAY_BUFFER, orangeVBO);
		// copy the vertices into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(orangeVertices), orangeVertices, GL_STATIC_DRAW);
		// tell OpenGl how to interpret the data in VBO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		// unbind VAO
		glBindVertexArray(0);

		// same for yellow
		glBindVertexArray(yellowVAO);
		glBindBuffer(GL_ARRAY_BUFFER, yellowVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(yellowVertices), yellowVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	int initShaders() {
		// compile shaders by creating a shader object and attaching the shader sources to them
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int orangeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		unsigned int yellowFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
		glShaderSource(orangeFragmentShader, 1, &orangeFragmentShaderSrc, NULL);
		glShaderSource(yellowFragmentShader, 1, &yellowFragmentShaderSrc, NULL);
		
		glCompileShader(vertexShader);
		glCompileShader(orangeFragmentShader);
		glCompileShader(yellowFragmentShader);

		int success;
		char infoLog[512];

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		glGetShaderiv(orangeFragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(orangeFragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		glGetShaderiv(yellowFragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(yellowFragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		orangeShaderProgram = glCreateProgram();
		yellowShaderProgram = glCreateProgram();

		glAttachShader(orangeShaderProgram, vertexShader);
		glAttachShader(orangeShaderProgram, orangeFragmentShader);
		glAttachShader(yellowShaderProgram, vertexShader);
		glAttachShader(yellowShaderProgram, yellowFragmentShader);

		glLinkProgram(orangeShaderProgram);
		glLinkProgram(yellowShaderProgram);

		glGetProgramiv(orangeShaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(orangeShaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
			return -1;
		}
		glGetProgramiv(yellowShaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(yellowShaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
			return -1;
		}

		glDeleteShader(orangeFragmentShader);
		glDeleteShader(yellowFragmentShader);

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