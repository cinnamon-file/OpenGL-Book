/* This file draws a dark green-blueish screen as background, with an orange triangle in the middle */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/* Stages of the graphics pipeline:
1) Vertex Shader
2) Shape Assembly
3) Geometry Shader
4) Rasterization
5) Fragment Shader
6) Tests and Bldening */

/* - In order for OpenGL to know what to make of your collection of coordinates and color values,
OpenGL requires you to hint what kinf of render types you want to form with the data. Do we want
the data redered as a collection of points, a collection of triangles or perhaps just one long line?
Those hints are called "primitives" and are given to OpenGL while calling any of the "drawing commands".
Some of these hints are GL_POINTS, GL_TRIANGLES and GL_LINE_STRIP. */

/* - The "vertex shader"(1) takes as input a single vertex. The main purpose is to transform 3D coordinates
into different 3D coordinates.
- The primitive "assembly stage" (Shape Assembly)(2) takes as input all the vertices (or vertex,
if GL_POINTS is chosen as a primitive) from the vertex shader that form a primitive and assembles
all the point(s) in the primitive shape give; in this case a triangle.
- The output of the primitive assembly stage is passed to the "geometry shader"(3). The geometry
shader takes as input a collection of vertices that form a primitive and has the ability to generate
other shapes by emitting new vertices to form new (or other) primitive(s). In this example case, it
generates a second triangle out of the given shape.
- The output of the geometry shader is then passed on to the "rasterization stage"(4) where it maps the
resulting primitive(s) to the corresponding pixels on the final screen, resulting in fragments for the
fragment shader to use. Before the fragment shaders run, clipping is performed. Clipping discards
all fragments that are outside your view, increasing performance.
---------------------------------------------
- The vertex shader is in the shader language GLSL (OpenGL Shading Language).
- We declare all the input vertex attributes in the vertex shader with the in keyword.
- Since each vertex has a 3D coordinate we create a vec3 input variable with the name aPos.
- A vector in GLSL has a maximum size of 4 and each of its values can be retrieved via vec.x,
vec.y, vec.z and vec.w .
- Note that the vec.w component is not used as a position in space (we’re dealing with 3D, not 4D),
but is used for something called perspective division. */
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

/* - A fragment in OpenGL is all the data required for OpenGL to render a single pixel.
- The main purpose of the "fragment shader"(5) is to calculate the final color of a pixel and this is
usually the stage where all the advanced OpenGL effects occur. Usually the fragment shader contains
data about the 3D scene that it can use to calculate the final pixel color (like lights, shadows, color
of the light and so on).
- After all the corresponding color values have been determined, the final object will then pass
through one more stage that we call the "alpha test" and "blending stage"(6). This stage checks the
corresponding depth (and stencil) value (we’ll get to those later) of the fragment and uses those to
check if the resulting fragment is in front or behind other objects and should be discarded accordingly.
The stage also checks for alpha values (alpha values define the opacity of an object) and blends the
objects accordingly. So even if a pixel output color is calculated in the fragment shader, the final
pixel color could still be something entirely different when rendering multiple triangles. */
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

/* - As you can see, the graphics pipeline is quite a complex whole and contains many configurable
parts. However, for almost all the cases we only have to work with the vertex(1) and fragment shader(5).
The geometry shader is optional and usually left to its default shader. There is also the "tessellation
stage" and "transform feedback loop" that we haven’t depicted here, but that’s something for later. */

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
    /* OpenGL only processes 3D coordinates when they’re in a specific range between -1.0 and 1.0 on
    all 3 axes (x, y and z). All coordinates within this so called normalized device coordinates range
    will end up visible on your screen (and all coordinates outside this region won’t).
    - Because we want to render a single triangle we want to specify a total of three vertices with
    each vertex having a 3D position. We define them in normalized device coordinates (the visible
    region of OpenGL) in a float array.
    - Because OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate
    of 0.0. This way the depth of the triangle remains the same making it look like it’s 2D.*/
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    /* Generates a "vertex buffer" (OpenGL object) with a unique ID, using <glGenBuffers> */
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    /* - The buffer type of a vertex buffer object is <GL_ARRAY_BUFFER>.
    - OpenGL allows us to bind to several buffers at once, as long as they have a different buffer type.
    - We are going to bind the newly created buffer <VBO> to the target <GL_ARRAY_BUFFER> with
    the <glBindBuffer> function.
    - From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to
    configure the currently bound buffer, which is VBO. */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /* - Then we can make a call to the <glBufferData> function that copies the previously defined
    "vertex data" into the "buffer’s memory":
    - glBufferData is a function specifically targeted to copy user-defined data into the currently
    bound buffer. Its first argument(1) is the type of the buffer we want to copy data into: the vertex buffer
    object currently bound to the GL_ARRAY_BUFFER target. The second argument(2) specifies the size
    of the data (in bytes) we want to pass to the buffer; a simple sizeof of the vertex data suffices.
    The third parameter(3) is the actual data we want to send.
    The fourth parameter(4) specifies how we want the graphics card to manage the given data. This
    can take 3 forms:
    -> GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    -> GL_STATIC_DRAW: the data is set only once and used many times.
    -> GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    - The position data of the triangle does not change, is used a lot, and stays the same for every
    render call so its usage type should best be GL_STATIC_DRAW. If, for instance, one would have a
    buffer with data that is likely to change frequently, a usage type of GL_DYNAMIC_DRAW ensures
    the graphics card will place the data in memory that allows for faster writes.
    - As of now we stored the vertex data within memory on the graphics card as managed by a
    vertex buffer object named VBO. Next we want to create a vertex and fragment shader that actually
    processes this data, so let’s start building those.*/
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
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
