#include <print>
#include <memory>

// glad should include before glfw
#include "glad/glad.h"
#include "glfw3/glfw3.h"

namespace OpenGLStudy
{
  struct GL_NDCVertex
  {
    union
    {
      struct
      {
        float X;
        float Y;
        float Z;  
      };

      float XYZ[3];
    };
  };
}


namespace
{
  static const char* WINDOW_TITLE = "LearnOpenGL";
  constexpr uint32_t WINDOW_WIDTH = 800;
  constexpr uint32_t WINDOW_HEIGHT = 600;

  static void framebuffer_size_callback(GLFWwindow*, /**width */GLsizei, /**height */GLsizei);
  static void pollGLFWEvent(GLFWwindow*);

  static void processInput(GLFWwindow*);
}

namespace GLSLShaderText
{
  static const char* vertexShaderSource =
  "#version 460 core\nlayout (location = 0) in vec3 aPos;\nvoid main()\n{\n  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n}\0";

  static const char* fragmentShaderSource =
  "#version 460 core\nout vec4 FragColor;\nvoid main()\n{\n  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n}\0";
}

int main()
{
  using OpenGLStudy::GL_NDCVertex;

  std::print("Hello GCC!Size of GL_NDCVertex is {}", sizeof(GL_NDCVertex));

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  {
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr)
    {
      std::println("Failed to create GLFW window");
      glfwTerminate();
      return -1;
    }
  
    glfwMakeContextCurrent(window);
  
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
      std::println("Failed to initialize GLAD");
      return -1;
    }
  
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize triangle

    // Vertices
    GL_NDCVertex vertices[] =
    {
      {-0.5f, -0.5f, 0.0f},
      { 0.5f, -0.5f, 0.0f},
      { 0.0f,  0.5f, 0.0f}
    };

    // Indices
    uint32_t indices[] =
    {
      0, 1, 3,
      1, 2, 3
    };

    // Use vertex buffer object to store vertices(OpenGL object)
    uint32_t VBO = 0;
    glGenBuffers(1, &VBO);

    // Generate vertex array object(OpenGL object)
    uint32_t VAO;
    glGenVertexArrays(1, &VAO);

    // Generate element buffer object
    uint32_t EBO;
    glGenBuffers(1, &EBO);

    

    // bind vertex array object;
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /**
     * Copy user-defined data into the currently bound buffer
     * First argument: Type of buffer
     * Second argument: Size of the data we want to pass to the buffer
     * Third argument: Actual data we want to send
     * Fourth argument: Specifies how we want the graphics card to manage the given data
     *                  GL_STREAM_DRAW: The data is set only once and used by the GPU at most a few times
     *                  GL_STATIC_DRAW: The data is set only once and used many times
     *                  GL_DYNAMIC_DRAW: The data is changed a lot and used many times
     */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Tell OpenGL how it should interpret vertex data
    /**
     * // NOTE layout (location = 0)
     * First argument: Specifies which vertex attribute we want to configure
     * // NOTE vec3 -> 3
     * Second argument: Size of the vertex attribute.
     * // NOTE vec* in GLSL consists of floating point values -> GL_FLOAT
     * Third argument: Specifies the type of data
     * // NOTE GL_TRUE if we inputting integer data type(int, byte)
     * Fourth argument: Specifies if we want the data to be normalized
     * // NOTE We could've also specified the stride as 0 to let OpenGL determine it.
     * // NOTE Only works when values are tightly packed
     * Fifth argument: Known as the Stride.Space between consecutive vertex attributes
     * // FIXME Explore this in more detail later
     * Sixth argument: Offset of where the position data begins.
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_NDCVertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    
    while(!glfwWindowShouldClose(window))
    {
      processInput(window);
  
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      constexpr int32_t infoLogLength = 512;
      // Create vertex shader object(OpenGL object)
      uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);

      /**
       * First argument: Shader object to compile
       * Second argument: Specifies how many strings we're passing as source code
       * Third argument: Source code of GLSL
       * Fourth argument: We can leave it to NULL //NOTE Figure out why
       */
      glShaderSource(vertexShader, 1, &GLSLShaderText::vertexShaderSource, nullptr);
      glCompileShader(vertexShader);

      // Check compilation status
      {
        int32_t success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
          char infoLog[infoLogLength];
          glGetShaderInfoLog(vertexShader, infoLogLength, nullptr, infoLog);
          std::println("ERROR:Shader vertex compilation failed\n{}", infoLog);
        }
      }

      // Create fragment shader object(OpenGL object)
      uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragmentShader, 1, &GLSLShaderText::fragmentShaderSource, nullptr);
      glCompileShader(fragmentShader);

      // Check compilation status
      {
        int32_t success;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
          char infoLog[infoLogLength];
          glGetShaderInfoLog(fragmentShader, infoLogLength, nullptr, infoLog);
          std::println("ERROR:Shader fragment compilation failed\n{}", infoLog);
        }
      }

      // Link both vertex and fragment to a shader program object(OpenGL object)
      uint32_t shaderProgram;
      shaderProgram = glCreateProgram();
      glAttachShader(shaderProgram, vertexShader);
      glAttachShader(shaderProgram, fragmentShader);
      glLinkProgram(shaderProgram);
    
      // Check link status
      {
        int32_t success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
          char infoLog[infoLogLength];
          glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, infoLog);
          std::println("ERROR:Shader program link failed\n{}", infoLog);
        }
      }

      // Delete shader objects if we linked them into the shader program
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      // Active shader program
      glUseProgram(shaderProgram);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      pollGLFWEvent(window);
    }
  }

  glfwTerminate();

  return 0;
}

namespace
{
  void framebuffer_size_callback(GLFWwindow* Window, GLsizei InWidth, GLsizei InHeight)
  {
    glViewport(0, 0, InWidth, InHeight);
  }

  void pollGLFWEvent(GLFWwindow* Window)
  {
    glfwPollEvents();
    glfwSwapBuffers(Window);
  }

  void processInput(GLFWwindow* Window)
  {
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      glfwSetWindowShouldClose(Window, true);
    }
  }
}