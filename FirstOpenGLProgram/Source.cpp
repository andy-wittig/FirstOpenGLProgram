/*
#include <iostream>

//OpenGL Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

//Signature
void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//Shader Sources
const char* vShaderSrc = "#version 460 core \n"
	"layout (location = 0) in vec3 aPos; \n"
	"void main() { gl_position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }";

const char* fShaderSrc = "#version 460 core \n"
	"out vec4 color; \n"
	"void main() { color = vec4(1.0f, 0.5f, 0.2f, 1.0f); }";

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
	GLFWwindow* window = glfwCreateWindow(960, 540, "Insane Triangles!", NULL, NULL);

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

	//Build and Compile Shader Program
	//Vertex Shader
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vShaderSrc, NULL);
	glCompileShader(vShader);

	//Check Vertex Shader Compiles
	int compile_success;
	char infoLog[512];
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compile_success);

	if (!compile_success)
	{
		glGetShaderInfoLog(vShader, 512, NULL, infoLog);
		std::cout << "Shader Vertex Compilation: Failed!\n" << infoLog << std::endl;
	}

	//Fragment Shader
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderSrc, NULL);
	glCompileShader(fShader);

	//Check Fragment Shader Compiles
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compile_success);

	if (!compile_success)
	{
		glGetShaderInfoLog(fShader, 512, NULL, infoLog);
		std::cout << "Shader Fragment Compilation: Failed!\n" << infoLog << std::endl;
	}

	//Link Shaders
	GLuint Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	glLinkProgram(Program);

	//Check if Shader Program Links
	glGetProgramiv(Program, GL_LINK_STATUS, &compile_success);

	if (!compile_success)
	{
		glGetProgramInfoLog(Program, 512, NULL, infoLog);
		std::cout << "Linking Shader Program: Failed!\n" << infoLog << std::endl;
	}

	//Setup Vertex Data
	float vertices[] = { //Vertices of triangle
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Main Window Loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//Rendering
		glUseProgram(Program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Swap Buffers and Poll Events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Deallocate Resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(Program);

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
*/