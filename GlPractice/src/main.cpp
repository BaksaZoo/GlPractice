#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Prototype for a method that gets called if the user resizes the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// Method signature for a callback that gets invoked when the user presses a button.
void processInput(GLFWwindow* window);
// Handles drawing onto the screen.
void draw();

int main() {
    glfwInit();
    // request client API with this version (3.3.?)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Telling GLFW we want to use the core-profile means we'll get access to a smaller subset of OpenGL
    // features without backwards-compatible features we no longer need. 
    // This might lead to errors so comment it if you need to use legacy code.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
}
