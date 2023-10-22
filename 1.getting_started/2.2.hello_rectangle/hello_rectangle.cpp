/* Draws rectangle by storing 4 vertices, and then just specify at which order we’d like to draw them. */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
    /* - If we'd put 2 triangles to form a rectangle, we'd need to draw the border 2 times. So, in
    big projects we'd waste a lot of the PCs power. The solution is to store only the unique vertices
    and then specify the order at which we want to draw these vertices in. In that case we would only
    have to store 4 vertices for the rectangle, and then just specify at which order we’d like to draw them.
    Wouldn’t it be great if OpenGL provided us with a feature like that?
    Thankfully, "element buffer objects" work exactly like that. An (EBO) is a buffer, just like a vertex
    buffer object, that stores indices that OpenGL uses to decide what vertices to draw. This so called
    indexed drawing is exactly the solution to our problem. To get started we first have to specify the
    (unique) vertices and the indices to draw them as a rectangle.
    You can see that, when using indices, we only need 4 vertices instead of 6. */
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* - Note that we’re now giving GL_ELEMENT_ARRAY_BUFFER as the buffer target.
    - Similar to the VBO we bind the EBO and copy the indices into the buffer with <glBufferData>.
    Also, just like the VBO we want to place those calls between a bind and an unbind call, although
    this time we specify GL_ELEMENT_ARRAY_BUFFER as the buffer type. */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    /* - "Wireframe mode": instead of drawing the triangles with color inside, we only draw the border.
    - Wireframe mode To draw your triangles in wireframe mode, you can configure
    how OpenGL draws its primitives via glPolygonMode(GL_FRONT_AND_BACK,GL_LINE).
    The first argument says we want to apply it to the front and back of all triangles and the second line
    tells us to draw them as lines. Any subsequent drawing calls will render the triangles in wireframe mode
    until we set it back to its default using glPolygonMode(GL_FRONT_AND_BACK, GL_FILL).*/
    
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
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        /* - The last thing left to do is replace the glDrawArrays call with glDrawElements to indicate
        we want to render the triangles from an index buffer. When using glDrawElements we’re going to draw
        using indices provided in the element buffer object currently bound (EBO).
        - The first argument specifies the mode we want to draw in, similar to glDrawArrays.
        - The second argument is the count or number of elements we’d like to draw. We specified 6 indices so
        we want to draw 6 vertices in total.
        - The third argument is the type of the indices which is of type GL_UNSIGNED_INT.
        - The last argument allows us to specify an offset in the EBO (or pass in an index array, but that
        is when you’re not using element buffer objects), but we’re just going to leave this at 0.
        - The <glDrawElements> function takes its indices from the EBO currently bound to the
        GL_ELEMENT_ARRAY_BUFFER target. This means we have to bind the corresponding EBO
        each time we want to render an object with indices which again is a bit cumbersome. It just so
        happens that a vertex array object (VAO) also keeps track of element buffer object bindings.
        The last element buffer object that gets bound while a VAO is bound, is stored as the VAO’s element
        buffer object (EBO). Binding to a VAO then also automatically binds that EBO.
         ______________________        ________
        |        VAO 1         |      |        V       VBO 1
        |______________________|      |     pos[0] pos[1] pos[2] pos[3] ... pos[n]
        |attribute pointer 0   | -> __|        |      ^
        |attribute pointer 1   |               |______|
        |attribute pointer 2   |                stride = 4 byte
        |...                   |
        |attribute pointer 15  |                                             VBO 2
        |                      |                                  pos[0] col[0] pos[1] col[1] ... col[n]
        |element buffer object | -> goes to EBO 1 (index data)       |       |     ^       ^
        |______________________|                                     |_______|_____|       |
                                                                             |   strides   |
         ______________________                                              |_____________|
        |        VAO 2         |
        |______________________|
        |attribute pointer 1   | -> goes to VBO 2 (pos[0])
        |attribute pointer 2   | -> goes to VBO 2 (col[0])
        |attribute pointer 3   |
        |...                   |                                            EBO 1
        |attribute pointer 15  |                                          index data
        |                      |
        |element buffer object | -> goes to EBO 2 (index data)              EBO 2
        |______________________|                                          index data

        - A VAO stores the glBindBuffer calls when the target is GL_ELEMENT_ARRAY_BUFFER. */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    glDeleteBuffers(1, &EBO);
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
