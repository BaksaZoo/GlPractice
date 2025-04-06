#ifndef PRACTICE_H
#define PRACTICE_H

struct GLFWwindow;

namespace practice {
    int main();
	// Prototype for a method that gets called if the user resizes the window.
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	// Method signature for a callback that gets invoked when the user presses a button.
	void processInput(GLFWwindow* window);
	// Handles drawing onto the screen.
	void draw();
	void drawTriangle();
	// initializes the window
	int initWindow();
	// initializes shaders
	int initShaders();
	int initVBO();
	int initVAO();
}

#endif