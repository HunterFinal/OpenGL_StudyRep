#include <print>
#include <memory>
#include <cmath>
#include <filesystem>

// glad should include before glfw
#include <glad/glad.h>
#include <glfw3/glfw3.h>

// glm
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Render/GLSLShader.h"
#include "Runtime/Camera.h"


namespace OpenGLStudy
{
  struct GL_NDCVector3
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
  using OpenGLStudy::Runtime::Camera;

  static const char* WINDOW_TITLE = "LearnOpenGL";
  constexpr uint32_t WINDOW_WIDTH = 800;
  constexpr uint32_t WINDOW_HEIGHT = 600;
  constexpr float WINDOW_WIDTHF = static_cast<float>(WINDOW_WIDTH);
  constexpr float WINDOW_HEIGHTF = static_cast<float>(WINDOW_HEIGHT);

  static void framebuffer_size_callback(GLFWwindow*, /**width */GLsizei, /**height */GLsizei);
  static void mouse_callback(GLFWwindow*, /**X */double, /**Y */double);
  static void scroll_callback(GLFWwindow*, /**X */double, /**Y */double);
  static void pollGLFWEvent(GLFWwindow*);
  static void processInput(GLFWwindow*);
  static void updateTime();
  static constexpr glm::mat4 getProjectionMat();

  static glm::mat4 LookAt(const Camera& InCamera, glm::vec3 TargetPos);

  constexpr uint8_t OPENGL_VERSION_MAJOR = 4;
  constexpr uint8_t OPENGL_VERSION_MINOR = 6;

  float g_LerpAlpha = 0.0f;

  constexpr float g_ASPECT_RATIO =  WINDOW_WIDTHF / 
                                    WINDOW_HEIGHTF;

  constexpr float g_NEAR_CLIP = 0.1f;
  constexpr float g_FAR_CLIP = 100.0f;

  constexpr bool g_IS_ORTHO = false;

  Camera g_Camera;

  float g_DeltaTime = 0.0f;
  float g_LastFrameTime = 0.0f;

  float g_mouseLastPosX = WINDOW_WIDTHF / 2.0f;
  float g_mouseLastPosY = WINDOW_HEIGHTF / 2.0f;

  bool g_firstMouse = true;
}

int main()
{
  using OpenGLStudy::GL_NDCVector3;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
  if (window == nullptr)
  {
    std::println("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
  {
    std::println("Failed to initialize GLAD");
    return -1;
  }

  // Hide cursor and capture mouse movement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // Filp the y-axis during image loading
  stbi_set_flip_vertically_on_load(true);

  // Set OpenGL polygon mode
  {
    // Wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  // // Set Texture wrapping mode
  // {
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // }

  // // Set texture filtering
  // {
  //   // Magnifying -> scaling upward
  //   // Minifying  -> scaling downward(Also can set mipmap filter)
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // }

  // Activate depth test
  glEnable(GL_DEPTH_TEST);

  // Load image data
  using std::filesystem::absolute;
  int width, height, nrChannels;
  uint8_t* data = stbi_load(
    absolute("../resource/Textures/container.jpg").string().c_str(),
    &width,
    &height,
    &nrChannels,
    0
  );

  // Generate texture object(OpenGL object)
  uint32_t texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  // Generate texture
  /**
   * // NOTE GL_TEXTURE_2D means this operation will generate a texture on the currently bound texture object at the same target
   * First argument: Specifies texture target
   * // NOTE For which we want to create a texture for if you want to set eacn mipmap level manually.
   * Second argument: Specifies the mipmap level
   * // NOTE Our image has only RGB values
   * Third argument: Tells OpenGL in what kind of format we want to store the texture
   * Fourth/Fifth argument: Width tand height of the resulting texture
   * // NOTE Some legacy stuff
   * Six argument: Should always be 0
   * // NOTE Loaded image with RGB values and stored them as chars(unsigne byte)
   * Seventh/Eighth argument: Specify the format and datatype of the source image
   * Ninth argument: Actual image data
   */
  if (data != nullptr)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(data));

    // Call this to generate all the required mipmaps for the currently bound texture automatically
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::println("Failed to load texture(.jpg)");
  }

  // Release image data
  stbi_image_free(data);

  uint32_t texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  // Load another image
  data = stbi_load(
    absolute("../resource/Textures/awesomeface.png").string().c_str(),
    &width,
    &height,
    &nrChannels,
    0
  );

  if (data != nullptr)
  {
    // .png contains an alpha channel
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(data));

    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::println("Failed to load texture(.png)");
  }

  // Initialize triangle

  // Vertices
  const GL_NDCVector3 vertices[] =
  {
    { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f},
    { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f},
    {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
    {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
  };

  // Indices
  const uint32_t indices[] =
  {
    0, 1, 3,
    1, 2, 3
  };

  // Cube
  const float vertices_cube[] =
  {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
  };

  // Cube positions
  glm::vec3 cubePositions[] = 
  {
    glm::vec3( 0.0f, 0.0f, 0.0f),
    glm::vec3( 2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f, 2.0f, -2.5f),
    glm::vec3( 1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);

  // // Copy index array in a element buffer for OpenGL use
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // Color attribute
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(sizeof(GL_NDCVector3)));
  // glEnableVertexAttribArray(1);

  // Texture UV attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  using std::filesystem::absolute;
  OpenGLStudy::Render::GLSLShader shaderProgram{
    absolute("../resource/Shader/shader.vs").string(), 
    absolute("../resource/Shader/shader.fs").string()
  };
        
  // Active shader program
  shaderProgram.Activate();
  shaderProgram.SetInt("texture1", 0);
  shaderProgram.SetInt("texture2", 1);

  
  while(!glfwWindowShouldClose(window))
  {
    updateTime();
    processInput(window);

    const float deltaTime = glfwGetTime();

    // Set background color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Set lerp alpha
    glUniform1f(glGetUniformLocation(shaderProgram.GetID(), "LerpAlpha"), g_LerpAlpha);

    // Render triangles
    glBindVertexArray(VAO);
    /**
     * First argument: Specifies the mode we want to draw in(Similar to glDrawArrays)
     * Second argument: Count or number of elements we'd like to draw
     * Third argument: Type of indices(uint32_t -> GL_UNSIGNED_INT)
     * // NOTE Pass in an index array when you're not using element buffer objects
     * Fourth argument: Specify an offset in the EBO
     * 
     * @see glDrawElements()
     */
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    constexpr float radius = 10.0f;

    const float camX = std::sinf(deltaTime) * radius;
    const float camZ = std::cosf(deltaTime) * radius;

    // NOTE Buggy
    // const glm::mat4 view = LookAt(g_Camera, g_Camera.GetPosition() + g_Camera.GetForwardVector());
    const glm::mat4 view = glm::lookAt(g_Camera.GetPosition(), g_Camera.GetPosition() + g_Camera.GetForwardVector(), g_Camera.GetUpVector());
    const glm::mat4 projection = getProjectionMat();

    for (uint32_t i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i)
    {
      glm::mat4 model = glm::mat4{1.0f};
      float angle = 20.0f * (float)i;
      if (i % 3 == 0)
      {
        angle += (float)glfwGetTime() * 50.0f;
      }
      model = glm::translate(model, cubePositions[i]);
      model = glm::rotate(model, glm::radians(angle), glm::vec3{1.0f, 0.3f, 0.5f});

      shaderProgram.SetMat4("transformMatrix", projection * view * model);

      glDrawArrays(GL_TRIANGLES, 0, 36);

    }

    pollGLFWEvent(window);
    
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();

  return 0;
}

namespace
{
  void framebuffer_size_callback(GLFWwindow* Window, GLsizei InWidth, GLsizei InHeight)
  {
    glViewport(0, 0, InWidth, InHeight);
  }

  void mouse_callback(GLFWwindow* Window, double X, double Y)
  {
    if (g_firstMouse)
    {
      g_mouseLastPosX = X;
      g_mouseLastPosY = Y;
      g_firstMouse = false; 
    }

    constexpr float sensitivity = 0.01f;
    const float offsetX = (X - g_mouseLastPosX) * sensitivity;
    const float offsetY = (g_mouseLastPosY - Y) * sensitivity;
    
    g_mouseLastPosX = X;
    g_mouseLastPosY = Y;

    g_Camera.UpdateCameraRotation(glm::vec3{offsetY, offsetX, 0.0f});
  }

  void scroll_callback(GLFWwindow* Window, double X, double Y)
  {
    (void)Window;
    (void)X;

    g_Camera.UpdateFOV((float)-Y);
  }

  void pollGLFWEvent(GLFWwindow* Window)
  {
    glfwSwapBuffers(Window);
    glfwPollEvents();
  }

  void processInput(GLFWwindow* Window)
  {
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      glfwSetWindowShouldClose(Window, true);
    }

    if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
    {
      g_LerpAlpha = std::min(g_LerpAlpha + 0.001f, 1.0f);
    }

    if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
      g_LerpAlpha = std::max(g_LerpAlpha - 0.001f, 0.0f);
    }

    glm::vec3 cameraMove{0.0f, 0.0f, 0.0f};
    const float cameraSpeed = 2.5f * g_DeltaTime;
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
    {
      cameraMove += cameraSpeed * g_Camera.GetForwardVector();
    }

    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
    {
      cameraMove -= cameraSpeed * g_Camera.GetForwardVector();
    }

    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
    {
      cameraMove -= cameraSpeed * glm::normalize(glm::cross(g_Camera.GetForwardVector(), g_Camera.GetUpVector()));
    }

    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
    {
      cameraMove += cameraSpeed * glm::normalize(glm::cross(g_Camera.GetForwardVector(), g_Camera.GetUpVector()));
    }

    g_Camera.UpdateCameraPosition(std::move(cameraMove));
  }

  constexpr glm::mat4 getProjectionMat()
  {
    glm::mat4 projection{};

    if (g_IS_ORTHO)
    {
      projection = glm::ortho(0.0f, WINDOW_WIDTHF, 0.0f, WINDOW_HEIGHTF, g_NEAR_CLIP, g_FAR_CLIP);
    }
    else
    {
      projection = glm::perspective(glm::radians(g_Camera.GetFOV()), g_ASPECT_RATIO, g_NEAR_CLIP, g_FAR_CLIP);
    }

    return projection;

  }

  void updateTime()
  {
    g_DeltaTime = glfwGetTime() - g_LastFrameTime;
    g_DeltaTime = std::clamp(g_DeltaTime, 0.0f, 0.005f); 
    g_LastFrameTime = glfwGetTime();
  }

  glm::mat4 LookAt(const Camera& InCamera, glm::vec3 TargetPos)
  {
    glm::mat4 lookAtMatLeft{1.0f};
    glm::mat4 lookAtMatRight{1.0f};

    const glm::vec3 cameraPos = InCamera.GetPosition();
    const glm::vec3 direction = glm::normalize(TargetPos - cameraPos); // zaxis
    const glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3{0.0f, 1.0f, 0.0f}, direction)); // xaxis
    const glm::vec3 lookAtUp = glm::normalize(glm::cross(cameraRight, direction)); // yaxis

    lookAtMatLeft[0][0] = cameraRight.x;
    lookAtMatLeft[1][0] = cameraRight.y;
    lookAtMatLeft[2][0] = cameraRight.z;

    lookAtMatLeft[0][1] = lookAtUp.x;
    lookAtMatLeft[1][1] = lookAtUp.y;
    lookAtMatLeft[2][1] = lookAtUp.z;

    lookAtMatLeft[0][2] = direction.x;
    lookAtMatLeft[1][2] = direction.y;
    lookAtMatLeft[2][2] = direction.z;

    lookAtMatRight[3][0] = -cameraPos.x;
    lookAtMatRight[3][1] = -cameraPos.y;
    lookAtMatRight[3][2] = -cameraPos.z;

    return lookAtMatLeft * lookAtMatRight;

  }
}