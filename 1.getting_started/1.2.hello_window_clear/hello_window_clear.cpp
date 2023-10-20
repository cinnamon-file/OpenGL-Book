#include <glad/glad.h>
#include <GLFW/glfw3.h>
/* The include file for GLAD includes the required OpenGL headers,
so be sure to include GLAD before other header files that require OpenGL (like GLFW) */

#include <iostream>

/* -The moment a user resizes the window the viewport should be adjusted as well.
We can register a callback function on the window that gets called each time the window is resized.
This resize callback function has the following prototype: */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

/* settings */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// instantiate the GLFW window:
	glfwInit(); // initialize GLFW
	/* Configure GLFW with glfwWindowHint:
	If user doesn't have the proper OpenGL version (between MINOR and MAJOR version), GLFW fails to run.
	We also tell GLFW we want to explicitly use the CORE_PROFILE.

	(I think: if you have version 4, for example, you will also have all the versions under 4.
	In 2022, OpenGL latest version is 4.7, for example.
	I also think: the newest GPUs supports the lattest OpenGL version, and older GPUs might
	only support up until a certain version.) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	/* We are creating a Window Object. This Window Object holds all the windowing data
	and is required by most of GLFW's other functions.

	glfwCreateWindow -> first and seconds arguments: width and height of the window.
	third argument: name for the window.
	This function returns a GLFWwindow object that we'll later need for other GLFW operations.
	After that we tell GLFW to make the context of our window the main context on the current thread. */
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/* Before, we mentioned that GLAD manages function pointers for OpenGL
	(#include <glad.h> must come before #include GLFW <glfw3.h> because of that),
	so we want to initialize GLAD before we call any OpenGL function: */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/* -We have to tell OpenGL the size of the rendering window so OpenGL knows
	how we want to display the data and coordinates with respect to the window.
	-The first two parameters of glViewport set the location of the lower left corner of the window.
	-The third and fourth parameter set the width and height of the rendering window in pixels,
	which we set equal to GLFW’s window size. However, the moment a user resizes the window the
	viewport should be adjusted as well, so we are not gonna use the following:
	glViewport(0, 0, 800, 600);

	(but another function called "framebuffer_size_callback", set at the bottom) */

	/* -The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has
	been instructed to close. If so, the function returns true and the render loop stops running,
	after which we can close the application */
	while (!glfwWindowShouldClose(window))
	{
		/* input */
		processInput(window);

		/* rendering commands next: */
		
		/* - We want to clear the screen with a color of our choice. At the
  		start of frame, we want to clear the screen. Otherwise, we would still see the
    		results from the previous frame (this could be the effect you're looking for,
      		but usually you don't). We can clear the screen's color buffer using glClear where
		we pass in buffer bits to specify which buffer we would like to clear. The possible
  		bits we can set are "GL_COLOR_BUFFER_BIT", "GL_DEPTH_BUFFER_BIT" and
    		"GL_STENCIL_BUFFER_BIT". Right now we only care about the color values, so we only
      		clear the color buffer "GL_COLOR_BUFFER_BIT". */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		/* - Note that we also specify the color to clear the screen with using glClearColor.
  		Whenever we call glClear and clear the buffer, the entire color buffer will be filled
    		with the color as configured.
      		- The first line of code is to select the color (dark green-blueish color), and the
		second line of code is to clear the buffer and paint the screen.
  		- As you may recall from the OpenGL chapter, the glClearColor function is a
    		state-setting function and glClear is a state-using function in that it uses the
      		current state to retrieve the clearing color from. */

		/* - The glfwSwapBuffers will swap the color buffer (a large 2D buffer that contains
		color values for each pixel in GLFW’s window) that is used to render to during this
			render iteration and show it as output to the screen */
		glfwSwapBuffers(window);
		/* - The glfwPollEvents function checks if any events are triggered
		(like keyboard input or mouse movement events), updates the window state,
			and calls the corresponding functions (which we can register via callback methods) */
		glfwPollEvents();
	}

	/* - As soon as we exit the render loop we would like to properly clean/delete all of
	GLFW’s resources that were allocated */
	glfwTerminate();
	return 0;
}

/* -The framebuffer size function takes a GLFWwindow as its first argument and two
integers indicating the new window dimensions. Whenever the window changes in size,
GLFW calls this function and fills in the proper arguments for you to process. */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	/* - GLFW’s glfwGetKey function that takes the window as input together with a key.
	The function returns whether this key is currently being pressed.
	- Here we check whether the user has pressed the escape key (if it’s not pressed, glfwGetKey
	returns GLFW_RELEASE).
	- If the user did press the escape key, we close GLFW by setting its WindowShouldClose property
	to true using glfwSetwindowShouldClose. */
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
