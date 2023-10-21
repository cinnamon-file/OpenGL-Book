#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/* - Shaders are written in the C-like language GLSL. GLSL is tailored for use with graphics and contains
useful features specifically targeted at vector and matrix manipulation.
- Shaders always begin with a version declaration, followed by a list of input and output variables,
uniforms and its main function. Each shader’s entry point is at its main function where we process
any input variables and output the results in its output variables.
---------------------------------------------------
- A shader typically has the following structure:

version version_number
in type in_variable_name;
in type in_variable_name;
out type out_variable_name;
uniform type uniform_name;

void main()
{
// process input(s) and do some weird graphics stuff
...
// output processed stuff to output variable
out_variable_name = weird_stuff_we_processed;
}
---------------------------------------------------
- When we’re talking specifically about the "vertex shader", each input variable is also known
as a vertex attribute.
- The vertex shader differs in its input, in that it receives its input straight from the vertex data. To
define how the vertex data is organized we specify the input variables with location metadata so we
can configure the vertex attributes on the CPU. We’ve seen this in the previous chapter as layout
(location = 0). The vertex shader thus requires an extra layout specification for its inputs so
we can link it with the vertex data.
- It is also possible to omit the layout (location = 0) specifier and query for the
attribute locations in your OpenGL code via glGetAttribLocation, but I’d prefer
to set them in the vertex shader. It is easier to understand and saves you (and OpenGL)
some work. */
const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";
/* - The other exception is that the fragment shader requires a vec4 color output variable, since the
fragment shaders needs to generate a final output color. If you fail to specify an output color in your
fragment shader, the color buffer output for those fragments will be undefined (which usually means
OpenGL will render them either black or white).
------------------------------ TIPS ---------------------------------
- So if we want to send data from one shader to the other we’d have to declare an output in the
sending shader and a similar input in the receiving shader. When the types and the names are equal
on both sides OpenGL will link those variables together and then it is possible to send data between
shaders (this is done when linking a program object). To show you how this works in practice we’re
going to alter the shaders from the previous chapter to let the vertex shader decide the color for the
fragment shader: (Example next)

Vertex shader
#version 330 core
layout (location = 0) in vec3 aPos; // position has attribute position 0
out vec4 vertexColor; // specify a color output to the fragment shader
void main()
{
gl_Position = vec4(aPos, 1.0); // we give a vec3 to vec4’s constructor
vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // output variable to dark-red
}

Fragment shader
#version 330 core
out vec4 FragColor;
in vec4 vertexColor; // input variable from vs (same name and type)
void main()
{
FragColor = vertexColor;
}

- You can see we declared a vertexColor variable as a vec4 output that we set in the vertex
shader and we declare a similar vertexColor input in the fragment shader. Since they both have
the same type and name, the vertexColor in the fragment shader is linked to the vertexColor
in the vertex shader. Because we set the color to a dark-red color in the vertex shader, the resulting
fragments should be dark-red as well.
------------------------------ END OF TIPS ---------------------------------
- We declared a uniform vec4 ourColor in the fragment shader and set the fragment’s output
color to the content of this uniform value. Since uniforms are global variables, we can define them
in any shader stage we’d like so no need to go through the vertex shader again to get something to
the fragment shader. We’re not using this uniform in the vertex shader so there’s no need to define it
there.
- The uniform is currently empty; we haven’t added any data to the uniform yet so let’s try that.
We first need to find the index/location of the uniform attribute in our shader -> (<glGetUniformLocation>).
*/
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f   // top 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);


    // bind the VAO (it was already bound, but just to demonstrate): seeing as we only have a single VAO we can 
    // just bind it beforehand before rendering the respective triangle; this is another approach.
    glBindVertexArray(VAO);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shaderProgram);

        // update shader uniform
        /* - Once we have the index/location of the uniform, we can update its values. Instead of passing a
        single color to the fragment shader, let’s spice things up by gradually changing color over time.
        - First, we retrieve the running time in seconds via glfwGetTime(). Then we vary the color in the
        range of 0.0 until 1.0 by using the sin function and store the result in greenValue. */
        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // render the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
