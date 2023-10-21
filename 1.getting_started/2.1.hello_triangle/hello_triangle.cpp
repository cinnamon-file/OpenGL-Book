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
- When passing data to the vertex attribute, glVertexAttribPointer(0, ..., ..., ..., ..., ...) -> the 0 is
refering to the (location = 0) that we set as the first value in the data, at the beginning of the buffer.
- We declare all the input vertex attributes in the vertex shader with the in keyword.
- Since each vertex has a 3D coordinate we create a vec3 input variable with the name aPos.
- A vector in GLSL has a maximum size of 4 and each of its values can be retrieved via vec.x,
vec.y, vec.z and vec.w .
- Note that the vec.w component is not used as a position in space (we’re dealing with 3D, not 4D),
but is used for something called "perspective division".
- We simply forwarded the input of the current "vertex shader" to the shader's output. */
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
pixel color could still be something entirely different when rendering multiple triangles.
------------------------------------
- Colors in computer graphics are represented as an array of 4 values: the red, green, blue and
alpha (opacity) component, commonly abbreviated to RGBA. When defining a color in OpenGL or GLSL
we set the strength of each component to a value between 0.0 and 1.0.
- We can declare output values with the out keyword, that we here promptly named FragColor.
Next we simply assign a vec4 to the color output as an orange color with an alpha value of 1.0
(1.0 being completely opaque, and 0.0 being completely transparent). */
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


    // +++ Build and compile our "Vertex Shader"
    // -------------------------------------------------------------------------------
    /* - We create a "vertex"(1), put the "source code" inside it(2), and compile it(3).
    - In order for OpenGL to use the shader it has to dynamically compile it at run-time from its
    source code. The first thing we need to do is create a "shader object" (vertex shader), again
    referenced by an ID. So we store the "vertex shader" as an unsigned int and create the shader
    with <glCreateShader>.
    - We provide the type of shader we want to create as an argument to glCreateShader. Since
    we’re creating a vertex shader we pass in GL_VERTEX_SHADER. */
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // (1)
    /* Next we attach the "shader source code" (vertex shader source) to the
    shader object (vertex shader) and compile the shader. The second argument specifies
    how many strings we’re passing as source code. I think it's to put the source code inside the shader*/
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // (2)
    glCompileShader(vertexShader); // (3)
    
    // check for shader compile errors
    /* integer to indicate success. */
    int success;
    /* storage container for the error messages (if any). */
    char infoLog[512];
    /* we check if the compilation was successful with <glGetShaderiv> and put the result in var "success". */
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    
    // +++ Build and compile our "Fragment Shader"
    // ----------------------------------------------------------------------------------
    /* - Same as "Vertex Shader", except we use <GL_FRAGMENT_SHADER>, instead of <GL_VERTEX_SHADER> */
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
    
    // +++ "Link Shaders"
    /* - To use the recently compiled shaders (Vertex Shader and Fragment Shader), we have to link them
    to a "shader program" object and then activate this shader program when rendering objects.
    - The activated shader program’s shaders will be used when we issue render calls.
    - When linking the shaders into a program it links the outputs of each shader to the inputs of the
    next shader. This is also where you’ll get linking errors if your outputs and inputs do not match. */
    // -----------------------------------------------------------------------------------
    /* - The glCreateProgram function creates a program and returns the ID reference to the newly
    created program object. */
    unsigned int shaderProgram = glCreateProgram();
    /* - Now we need to attach the previously compiled shaders to the program object "shaderProgram" and
    then link them with <glLinkProgram>. */
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    /* - The result is a program object that we can activate by calling glUseProgram with the newly
    created program object as its argument:
    -> glUseProgram(shaderProgram);
    - Every shader and rendering call after glUseProgram will now use this program object (and thus
    the shaders). */
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    /* - Oh yeah, and don’t forget to delete the shader objects once we’ve linked them into the program
    object; we no longer need them anymore. */
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    /* - Right now we sent the input vertex data to the GPU and instructed the GPU how it should
    process the vertex data within a vertex and fragment shader. We’re almost there, but not quite yet.
    OpenGL does not yet know how it should interpret the vertex data in memory and how it should connect
    the vertex data to the vertex shader’s attributes. We’ll be nice and tell OpenGL how to do that.
    - So, the next step is "Linking Vertex Attributes" (jump to <glVertexAttribPointer> function). */
    // -------------------------------------------------------------------------------------------------

    
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

    /* - VAO (Vertex Array Object).
    - VBO (Vertex Buffer Object).
    - A vertex array object (also known as VAO) can be bound just like a vertex buffer object and any
    subsequent vertex attribute calls from that point on will be stored inside the VAO. This has the
    advantage that when configuring vertex attribute pointers you only have to make those calls once
    and whenever we want to draw the object, we can just bind the corresponding VAO. This makes
    switching between different vertex data and attribute configurations as easy as binding a different
    VAO. All the state we just set is stored inside the VAO.
    Core OpenGL requires that we use a VAO so it knows what to do with our vertex input.
    - VAO stores the following:
    (1) Calls to glEnableVertexAttribArray or glDisableVertexAttrib Array.
    (2) Vertex attribute configurations via glVertexAttribPointer.
    (3) Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.*/
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    
    /* (Gen)erates a "vertex buffer" (OpenGL object) with a unique ID, using <glGenBuffers> */
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

    /*
    _____________________________________
    |  Vertex1  |  Vertex2  |  Vertex3  |
    | x | y | z | x | y | z | x | y | z |
    0   4   8   12  16  20  24  28  32  36  : number of bytes
    |___|___|___|___|___|___|___|___|___|
    
    - Each position data is stored as 32-bit (32/8= 4 byte) floating point values.
    - Each of those position is composed of 3 values (4 byte * 3 position = 12 byte for each vertex).
    - The first value in the data is at the beginning of the buffer.
    - The "stride" = 3 * sizeof(float) = 3 * 4 = 12 byte.

    The function <glVertexAttribPointer> has quite a few parameters:
    - The first parameter specifies which vertex attribute we want to configure. Remember that we
    specified the location of the position vertex attribute in the vertex shader with layout (location = 0).
    This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex
    attribute, we pass in 0.
    - The next argument specifies the size of the vertex attribute. The vertex attribute is a vec3 so
    it is composed of 3 values.
    - The third argument specifies the type of the data which is GL_FLOAT (a vec* in GLSL consists
    of floating point values).
    - The fourth argument specifies if we want the data to be normalized. If we’re inputting integer
    data types (int, byte) and we’ve set this to GL_TRUE, the integer data is normalized to 0 (or -1 for
    signed data) and 1 when converted to float. This is not relevant for us so we’ll leave this at GL_FALSE.
    - The fifth argument is known as the "stride" and tells us the space between consecutive vertex attributes.
    Since the next set of position data is located exactly 3 times the size of a float away we specify that
    value as the stride. Note that since we know that the array is tightly packed (there is no space between
    the next vertex attribute value) we could’ve also specified the stride as 0 to let OpenGL determine the
    stride (this only works when values are tightly packed). Whenever we have more vertex attributes we have
    to carefully define the spacing between each vertex attribute but we’ll get to see more examples of that
    later on.
    - The last parameter is of type void* and thus requires that weird cast. This is the offset of where
    the position data begins in the buffer. Since the position data is at the start of the data array
    this value is just 0. We will explore this parameter in more detail later on.
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    /* - We should also enable the vertex attribute with glEnableVertexAttribArray giving the vertex
    attribute location as its argument. */
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
        /* Run the "shaderProgram". */
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
