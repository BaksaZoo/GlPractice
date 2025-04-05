#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Prototype for a method that gets called if the user resizes the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// Method signature for a callback that gets invoked when the user presses a button.
void processInput(GLFWwindow* window);
// Handles drawing onto the screen.
void draw();
void drawTriangle();

// This is the source code of the vertex shader.
// This program will be run on the GPU after compiling it.
// "in vec3 aPos" means that we take a 3D vector is an INPUT
// "layout (location = 0)" means that we specify the location of the input variable, "aPos"
// "gl_Position" will be the output of the sader which is always a vec4
const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";
// The fragment shader is responsible for setting a pixel's color.
// For the sake of simplicity, this is and orange hardcoded value (rgba).
// The fragment shader only has one output which is a vec4 (FragColor in this case).
const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})";

int main() {
	glfwInit();
	// request client API with this version (3.3.?)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Telling GLFW we want to use the core-profile means we'll get access to a smaller subset of OpenGL
	// features without backwards-compatible features we no longer need. 
	// This might lead to errors so comment it if you need to use legacy code.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// request GLFW to create a windows
	GLFWwindow* window = glfwCreateWindow(800, 600, "GlPractice", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// GLAD manages function pointers, so we need to initialize it too.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// compile shaders and attach it to the shader program
	// 1. create a shader object, get its id
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// 2. attach the shader source code to the shader object
	// it is possible to attach multiple strings that contain source code (2nd, 3rd, 4th parameter of the function)
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	// 3. compile shaders
	glCompileShader(vertexShader);
	// check if shader compilation was successful. If not, print it to the console.
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		// copy compilation info into the 512 long char array
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	// same for the fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}

	// create the shader program and link the compiled shaders.
	// the shader program links the shader outputs to the next shader's input on the pipeline
	// if we made an error, the linker will not be able to link the inputs and outputs so we will get an error
	// first, create the program object
	unsigned int shaderProgram = glCreateProgram();
	// attach the compiled shaders to the program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// now link the shaders outputs and inputs
	glLinkProgram(shaderProgram);
	// we can also check for errors here
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	// now we need to tell which shader program to use
	// after this call, this shader program will be used until told otherwise
	glUseProgram(shaderProgram);

	// finally, delete the shader objects, we dont need them after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Initialize viewport. (same as the window for now)
	// Im guessing this is for you to be able to set different sections of the screen for different purposes.
	// Ex.: you can have the top of the window for navigation bar and the bottom for displaying content which will 
	// be 2 different viewports.
	glViewport(0, 0, 800, 600);

	// Tell GLFW to call framebuffer_size_callback when the window gets resized by the user. 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// We need to create the render loop or else the application shuts down immediately.
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// put rendering commands in the middle of input processing and event polling
		draw();

		// Not sure what this does but according to the docs, this will swap a large 2D buffer that contains the pixel
		// values for the provided window. This is probably the method that renders the modified screen buffer.
		// OpenGl uses a double buffering technique, where the front buffer is the buffer that gets rendered to the user
		// while the back buffer is the one where current rendering tasks happen. When the framework finishes its work on
		// the back buffer, the front buffer will get replaced by it. This is to avoid flickering issues.
		glfwSwapBuffers(window);
		// Poll for events. Important because the registered callbacks/event handlers gets notified by polling events.
		glfwPollEvents();
	}

	// Free resources allocated by the framework upon closing the app.
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// If the windows gets resized, reset the viewport to take up the whole screen.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	// check if the user pressed down the escape button
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void draw()
{
	// clear the screen with rgba colors
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Tell OpenGl that it should redraw the screen with the specified color.
	glClear(GL_COLOR_BUFFER_BIT);

	drawTriangle();
}

void drawTriangle()
{
	// holds 3 vertex of x, y, z NDC (Normalized Device Coordinates) coordinates
	float vertices[] = {
		-.5f, -.5f, .0f,
		.5f, -.5f, .0f,
		.0f, .5f, .0f
	};
	// id for the vertex buffer object (VBO)
	unsigned int VBO;
	// create a new object and assagn its ID in the VBO variable
	// This object will hold the verticies array.
	glGenBuffers(1, &VBO);
	// Bind the VBO to the vertex buffer.
	// OpenGl has many types of buffers and it allows us to bind multiple buffers at once as long as they have different types.
	// So thats why we need to specify which type of buffer we want VBO to bind.
	// From this point on any buffer calls we make on the GL_ARRAY_BUFFER target will be used to configure the 
	// currently bound buffer, which is VBO.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy the vertices into the VBO.
	// I think this is the point where copying to the graphics cards memory happens.
	// Notice that we dont have to specify the VBO, because of OpenGl's state machine behaviour.
	// The last parameter tells the GPU how to manage the data:
		// GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		// GL_STATIC_DRAW : the data is set only once and used many times.
		// GL_DYNAMIC_DRAW : the data is changed a lot and used many times.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	// now we need to tell opengl how to interpret the VBO data
	// the parameters:
		// 1st: which vertex attribute we want to specify. Since layout = 0 in vertex shader for the position
		//      we need to set the 0th attribute to be interpreted as position
		// 2nd: the size of the vertex attribute. vec3 will be composed by 3 elements of the VBO array
		// 3rd: the data type of the attribute (vec3 consist of floating point values)
		// 4th: should opengl automatically normalize the data? If we pass integers, opengl will normalize the values 
		//      to be in [-1.0, 1.0] range (or [0.0, 1.0] range for unsigned values)
		// 5th: also known as stride. Tells opengl how much space are there between vertex attributes. The next vertex data 
		//      is located exactly 3 times of the size of a float (which is 4 bytes btw.) 
		//      Note that if your VBO is tightly packeg (no space betweeb vertex data), you can pass 0 here and let gl 
		//		figure it out.
		// 6th: the offset of the vertex data, but since we start our coordinates at the first element of the VBO, 
		//		we can say its 0
	// note that this works, because we already set the current active VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Now that we specified how OpenGL should interpret the vertex data we should also enable the vertex attribute
	// giving the vertex attribute location as its argument, because vertex attributes are disabled by default.
	glEnableVertexAttribArray(0);

	// TODO: document this!!!
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// TODO: document this!!!
	glDeleteBuffers(1, &VBO);
}