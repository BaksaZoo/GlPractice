#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// showcases an ebo and texture
namespace texture {

	GLFWwindow* window;
	// hint: could just attach and detach the yellow and orange shaders
	unsigned int shaderProgram;
	unsigned int VAO;
	unsigned int wallTexture;

	const char* vertexShaderSrc = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 vtxColor;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.0);
    vtxColor = aColor;
	texCoord = aTexCoord;
}
)";
	// note the textureSampler uniform
	// the fragment shader needs to know the texture we are using to render the final pixel color
	// OpenGL has sampler[123]D for this purpose
	const char* fragmentShaderSrc = R"(
#version 330 core

uniform sampler2D textureSampler;

in vec3 vtxColor;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = texture(textureSampler, texCoord);
}
)";

	int main() {
		// create a window, initialize OpenGL
		if (initContext() != 0) return -1;
		// black background color
		glClearColor(.0f, .0f, .0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// initialize shaders
		if (initShaders() != 0) return -1;
		// initialize textures
		initTextures();
		// create two triangles
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

	void initTextures()
	{
		// read image data
		int width, height, nrChannels;
		unsigned char* data = stbi_load("assets/64x64.jpg", &width, &height, &nrChannels, 0);

		// generate the texture object
		glGenTextures(1, &wallTexture);

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, wallTexture);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (data)
		{
			// create the texture
			// args:
				// 1: target
				// 2: mip-map level. 0 because we dont want to manually define all mipmap levels. 
				//    Instead we let glGenerateMipmap do it for us.
				// 3: the color format we want to store the texture
				// 4, 5: width, height
				// 6: always 0, legacy stuff
				// 7: the color format of the actual texture, loaded from the image
				// 8: the data type of the loaded texture pixels
				// 9: the actual pixels of the image
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		// free the memory
		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void renderTriangles()
	{
		glUseProgram(shaderProgram);

		glBindVertexArray(VAO);
		// draw elements from the EBO
		// parameters:
			// 1: render mode
			// 2: how many indicies we want to draw from the EBO
			// 3: the type of data in the EBO
			// 4: offset in the buffer is EBO is not used
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	void initVAOs() {
		// create the triangle
		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		// EBO is the element buffer object, which we can use to specify vertex index order to draw
		unsigned int VBO, EBO;

		// create VAO
		glGenVertexArrays(1, &VAO);

		// create EBO
		glGenBuffers(1, &EBO);

		// create VBO
		glGenBuffers(1, &VBO);

		// bind VAO
		glBindVertexArray(VAO);
		// bind VBO to the VAO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// copy the vertices into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// bind EBO to VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// tell OpenGl how to interpret the data in VBO
		// since position isnt the only thing in the VBO, we need to configure the attribute pointers
		// [layout=0,1,2], [the data type is float], [do not normalize], [stride is 8 times of float], [first byte of the data]
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
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