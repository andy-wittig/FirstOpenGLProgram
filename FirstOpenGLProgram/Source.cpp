#include <iostream>

//OpenGL Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

//Signature
void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//Main
void main()
{
	if (!glfwInit()) //GLFW Init Failed
	{
		printf("GLFW Initialization: Failed!\n");
	}
	printf("GLFW Initialization: Succeeded!\n");

	glfwSetErrorCallback(error_callback);

	//Create Window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(960, 540, "My Window", NULL, NULL);

	if (!window) //Window Failed
	{
		printf("Window or Context Creation: Failed!\n");
	}
	printf("Window or Context Creation: Succeeded!\n");

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback); //Bind Key Callback

	if (glewInit()) //Initialize GLEW Libraries
	{
		printf("GLEW Initialization: Failed!\n");
	}
	printf("GLEW Initialization: Succeeded!\n");

	glfwSwapInterval(1);

	//Shader Init
	GLuint Program;
	GLuint vao[1];

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vShaderSrc = "#version 460 \n void main() { gl_position = vec4(0.0, 0.0, 0.0, 1.0); }";
	const char* fShaderSrc = "#version 460 \n out vec4 color void main() { color = vec4(0.0, 0.3, 1.0, 1.0); }";

	glShaderSource(vShader, 1, &vShaderSrc, NULL);
	glShaderSource(fShader, 1, &fShaderSrc, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);

	glLinkProgram(Program);

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	//Main Window Loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0, 0.2, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//Rendering
		glPointSize(10.f);
		glDrawArrays(GL_POINTS, 0, 1);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

//Implementation
void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}