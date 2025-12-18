#include <print>
#include <memory>
#include <cmath>
#include <filesystem>
#include <windows.h>
#include <map>
#include <format>

// glad should include before glfw
#include <glad/glad.h>
#include <glfw3/glfw3.h>

// glm
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Render/GLSLShader.h"
#include "Render/Model.h"
#include "Runtime/Camera.h"
#include "Render/ImageLoadHelper.h"



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

  constexpr glm::vec3 g_LIGHT_POS{1.2f, 1.0f, 2.0f};
}

namespace Chapter
{
  static void StencilTesting();
}

int main()
{
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
  // stbi_set_flip_vertically_on_load(true);

  {
    AllocConsole();
    FILE* fp;

    // stdout
    freopen_s(&fp, "CONOUT$", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    // stderr
    freopen_s(&fp, "CONOUT$", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    std::ios::sync_with_stdio(true);

  }

  // Set OpenGL polygon mode
  // {
  //   // Wireframe mode
  //   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // }

  // // Create framebuffer object
  uint32_t FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // uint32_t framebufferTexture;
  // glGenTextures(1, &framebufferTexture);
  // glBindTexture(GL_TEXTURE_2D, framebufferTexture);

  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Multisampled texture attachment
  uint32_t framebufferTexture_multiSampled;
  glGenTextures(1, &framebufferTexture_multiSampled);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture_multiSampled);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 10, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

  // /**
  //  * First argument:  The framebuffer type we're targeting(draw,read or both).
  //  * Second argument: The type of attachment we're going to attach. 0 at the end suggests we can attach more than 1 color attachment.
  //  * Third argument:  The type of the texture you want to attach.
  //  * Fourth argument: The actual texture to attach.
  //  * Fifth argument:  The mipmap level
  //  */
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture_multiSampled, 0);

  // // Create renderbuffer object
  // uint32_t RBO;
  // glGenRenderbuffers(1, &RBO);
  // glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
  // glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // Create multisampled renderbuffer object
  uint32_t RBO_multiSampled;
  glGenRenderbuffers(1, &RBO_multiSampled);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO_multiSampled);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 10, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO_multiSampled);

  // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

  // Check if requirements of framebuffer are all completed
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::println("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Activate depth test
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // // Activate stencil test
  // glEnable(GL_STENCIL_TEST);
  // // update stencil buffer
  // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  // Activate blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Activate face culling
  glEnable(GL_CULL_FACE);
  // Set which face you want to cull
  // glCullFace(GL_FRONT);
  // Tell OpenGL that which order of vertex is specifying as front face
  /**
   * GL_CW :  clockwise ordering
   * GL_CCW : counter-clockwise ordering
   */
  // glFrontFace(GL_CW);

  // Set different options for the RGB and alpha by using glBlendFuncSeparate
  // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

  // glBlendEquation allows us to set how Src and Dst color operate
  // GL_FUNC_ADD :              result = Src + Dst
  // GL_FUNC_SUBTRACT :         result = Src - Dst
  // GL_FUNC_REVERSE_SUBTRACT : result = Dst - Src
  
  // Takes the component-wise min/max of both
  // GL_MIN :                   result = min(Src, Dst);
  // GL_MAX :                   result = max(Src, Dst);

  // Enable gl_PointSize in GLSL
  glEnable(GL_PROGRAM_POINT_SIZE);

  // Enable Anti-aliasing
  glEnable(GL_MULTISAMPLE);
  
  // Use generic_string() instead of string to avoid platform diff
  // Windows: '/' will turn to '\'
  // Linus: Keep '/' still
  using std::filesystem::absolute;
  std::string&& gsPath = absolute("resource/Shader/shader.gs").generic_string();

  OpenGLStudy::Render::GLSLShader shaderProgram{
    absolute("resource/Shader/shader.vs").generic_string(), 
    absolute("resource/Shader/shader.fs").generic_string()
    // &gsPath
  };

  OpenGLStudy::Render::GLSLShader outlineShaderProgram{
    absolute("resource/Shader/shader.vs").generic_string(),
    absolute("resource/Shader/shaderSingleColor.fs").generic_string(),
  };

  OpenGLStudy::Render::GLSLShader transparentShaderProgram{
    absolute("resource/Shader/transparencyShader.vs").generic_string(),
    absolute("resource/Shader/transparencyShader.fs").generic_string(),
  };

  OpenGLStudy::Render::Model ourModel{absolute("resource/Meshes/Backpack/backpack.obj").generic_string()};

  // Load 6 texture by offset
  const std::vector<std::string> faces
  {
    absolute("resource/Textures/skybox/right.jpg").generic_string(),
    absolute("resource/Textures/skybox/left.jpg").generic_string(),
    absolute("resource/Textures/skybox/top.jpg").generic_string(),
    absolute("resource/Textures/skybox/bottom.jpg").generic_string(),
    absolute("resource/Textures/skybox/front.jpg").generic_string(),
    absolute("resource/Textures/skybox/back.jpg").generic_string()
  };

  OpenGLStudy::Render::GLSLShader skyboxShaderProgram
  {
    absolute("resource/Shader/cubemapShader.vs").generic_string(),
    absolute("resource/Shader/cubemapShader.fs").generic_string()
  };

  uint32_t cubemapTexture = OpenGLStudy::Helper::ImageLoadHelper::CubemapTextureFromFile(faces);

  const float transparentVertices[] = 
  {
      // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
      0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
      0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
      1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

      0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
      1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
      1.0f,  0.5f,  0.0f,  1.0f,  0.0f
  };

  const std::vector<glm::vec3> windows
  {
    glm::vec3(-1.5f, 0.0f, -0.48f),
    glm::vec3( 1.5f, 0.0f, 0.51f),
    glm::vec3( 0.0f, 0.0f, 0.7f),
    glm::vec3(-0.3f, 0.0f, -2.3f),
    glm::vec3( 0.5f, 0.0f, -0.6f)
  };

  const float quadVertices[] = 
  {
    // positions // texCoords
    -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f
  };

  const float skyboxVertices[] = 
  {
      // positions          
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
  };

  uint32_t quadVAO;
  glGenVertexArrays(1, &quadVAO);
  glBindVertexArray(quadVAO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);

  OpenGLStudy::Render::GLSLShader screenShaderProgram{
    absolute("resource/Shader/framebufferShader.vs").generic_string(),
    absolute("resource/Shader/framebufferShader.fs").generic_string(),
  };

  uint32_t transparentTexture = OpenGLStudy::Helper::ImageLoadHelper::TextureFromFile(
    "blending_transparent_window.png",
    absolute("resource/Textures").generic_string()
  );

  transparentShaderProgram.Activate();
  transparentShaderProgram.SetInt("texture1", 0);

  // transparent VAO
  uint32_t transparentVAO, transparentVBO;
  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glBindVertexArray(0);

  // Create skybox VAO
  uint32_t skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glBindVertexArray(0);

  // Create uniform buffer object
  uint32_t UBOMatricesBlock;
  glGenBuffers(1, &UBOMatricesBlock);
  glBindBuffer(GL_UNIFORM_BUFFER, UBOMatricesBlock);
  // mat4 * 2 = 128bytes
  constexpr uint32_t UNIFORM_MATRICES_BLOCK_SIZE = 2 * sizeof(glm::mat4);
  glBufferData(GL_UNIFORM_BUFFER, UNIFORM_MATRICES_BLOCK_SIZE, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // Before OpenGL4.2 we must call glUniformBlockBinding
  // uint32_t matricesIndex = glGetUniformBlockIndex(shaderProgram.GetID(), "Matrices");
  // glUniformBlockBinding(shaderProgram.GetID(), matricesIndex, 0);

  // Bind uniform buffer object to the same binding point
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBOMatricesBlock);

  // NOTE: Offset of glBindBufferRange must be multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
  // GLint uniformBufferOffsetAlign = 0;
  // glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferOffsetAlign);
  // glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatricesBlock, 0, uniformBufferOffsetAlign);

  // Bind projection mat to uniform object buffer
  const glm::mat4 projection = getProjectionMat();
  glBindBuffer(GL_UNIFORM_BUFFER, UBOMatricesBlock);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projection), glm::value_ptr(projection));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  std::string&& normalGSPath = absolute("resource/Shader/normalVisualizeShader.gs").generic_string();

  OpenGLStudy::Render::GLSLShader normalVisualizeShaderProgram{
    absolute("resource/Shader/normalVisualizeShader.vs").generic_string(),
    absolute("resource/Shader/normalVisualizeShader.fs").generic_string(),
    &normalGSPath
  };

  glm::vec2 translations[100];
  int32_t index = 0;
  constexpr float offset = 0.1f;
  for (int32_t y = -10; y < 10; y+=2)
  {
    for (int32_t x = -10; x < 10; x+=2)
    {
      const float transX = static_cast<float>(x) / 10.0f + offset;
      const float transY = static_cast<float>(y) / 10.0f + offset;
      translations[index++] = glm::vec2{transX, transY};
    }
  }

  while(!glfwWindowShouldClose(window))
  {
    updateTime();
    processInput(window);

    // first pass
    {
      // glBindFramebuffer(GL_FRAMEBUFFER, FBO);
      // Set background color
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      glBindFramebuffer(GL_FRAMEBUFFER, framebufferTexture_multiSampled);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      
      // sort the transparent windows before rendering
      std::map<float, glm::vec3> sorted{};
      for (size_t i = 0; i < windows.size(); i++)
      {
          const float distance = glm::length(g_Camera.GetPosition() - windows[i]);
          sorted[distance] = windows[i];
      }
  
      const glm::mat4 view = glm::lookAt(g_Camera.GetPosition(), g_Camera.GetPosition() + g_Camera.GetForwardVector(), g_Camera.GetUpVector());
      constexpr uint32_t VIEW_MEMORY_OFFSET = UNIFORM_MATRICES_BLOCK_SIZE - sizeof(glm::mat4);
      glBindBuffer(GL_UNIFORM_BUFFER, UBOMatricesBlock);
      glBufferSubData(GL_UNIFORM_BUFFER, VIEW_MEMORY_OFFSET, sizeof(view), glm::value_ptr(view));
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      // 1st. render pass, draw objects as normal, writing to the stencil buffer
      {
        shaderProgram.Activate();

        shaderProgram.SetFloat("time", g_LastFrameTime);
  
        shaderProgram.SetVec3("viewPos", g_Camera.GetPosition());
        shaderProgram.SetMat4("projection", projection);
        shaderProgram.SetMat4("view", view);
  
        glm::mat4 model = glm::mat4{1.0f};
        model = glm::translate(model, glm::vec3{0.0f});
        model = glm::scale(model, glm::vec3{1.0f});
        shaderProgram.SetMat4("model", model);

        glm::mat4 normalMat = model;
        normalMat = glm::inverse(normalMat);
        normalMat = glm::transpose(normalMat);
        shaderProgram.SetMat4("normalMatrix", normalMat);
  
        // backpack
        ourModel.Draw(shaderProgram, 100);

        // normalVisualizeShaderProgram.Activate();
        
        // normalVisualizeShaderProgram.SetMat4("model", model);
        // normalVisualizeShaderProgram.SetMat4("normalMatrix", normalMat);

        // // normal visualize
        // ourModel.Draw(normalVisualizeShaderProgram);
      }
      
      // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
      // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
      // the objects' size differences, making it look like borders.
      {
        transparentShaderProgram.Activate();
        transparentShaderProgram.SetMat4("projection", projection);
        transparentShaderProgram.SetMat4("view", view);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (decltype(sorted)::reverse_iterator rit = sorted.rbegin(); rit != sorted.rend(); ++rit)
        {
          glm::mat4 model = glm::mat4{1.0f};
          model = glm::translate(model, rit->second);
          transparentShaderProgram.SetMat4("model", model);
          glDrawArrays(GL_TRIANGLES, 0, 6);
        }
      }
    }

    // // second pass
    // {
    //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //   glClear(GL_COLOR_BUFFER_BIT);

    //   screenShaderProgram.Activate();
    //   glBindVertexArray(quadVAO);
    //   glDisable(GL_DEPTH_TEST);
    //   glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    //   glDrawArrays(GL_TRIANGLES, 0, 6);
    // }

    // Draw skybox as last
    {
      // Change depth function so depth test passes when values are equal to depth buffer's content
      glDepthFunc(GL_LEQUAL);
      skyboxShaderProgram.Activate();
      const glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(g_Camera.GetPosition(), g_Camera.GetPosition() + g_Camera.GetForwardVector(), g_Camera.GetUpVector())));
      const glm::mat4 projection = getProjectionMat();
      skyboxShaderProgram.SetMat4("view", view);
      skyboxShaderProgram.SetMat4("projection", projection);

      glBindVertexArray(skyboxVAO);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);

      // Set depth function back to default
      glDepthFunc(GL_LESS);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    pollGLFWEvent(window);
    
  }

  glfwTerminate();
  FreeConsole();

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

namespace Chapter
{
  void StencilTesting()
  {
    // const glm::mat4 view = glm::lookAt(g_Camera.GetPosition(), g_Camera.GetPosition() + g_Camera.GetForwardVector(), g_Camera.GetUpVector());
    // const glm::mat4 projection = getProjectionMat();
  
    // // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // {
    //   shaderProgram.Activate();

    //   // shaderProgram.SetFloat("material.shininess", 32.0f);
    //   // // Directional light
    //   // shaderProgram.SetVec3("sunLight.ambient", glm::vec3{0.2f});
    //   // shaderProgram.SetVec3("sunLight.diffuse", glm::vec3{0.5f});
    //   // shaderProgram.SetVec3("sunLight.specular", glm::vec3{1.0f});
    //   // shaderProgram.SetVec3("sunLight.direction", glm::vec3{-0.2f, -1.0f, -0.3f});

    //   // // Point light
    //   // {
    //   //   shaderProgram.SetVec3("pointLights[0].ambient", glm::vec3{0.2f});
    //   //   shaderProgram.SetVec3("pointLights[0].diffuse", glm::vec3{0.5f});
    //   //   shaderProgram.SetVec3("pointLights[0].specular", glm::vec3{1.0f});
    //   //   shaderProgram.SetFloat("pointLights[0].constant", 1.0f);
    //   //   shaderProgram.SetFloat("pointLights[0].linear", 0.09f);
    //   //   shaderProgram.SetFloat("pointLights[0].quadratic", 0.032f);

    //   //   shaderProgram.SetVec3("pointLights[1].ambient", glm::vec3{0.2f});
    //   //   shaderProgram.SetVec3("pointLights[1].diffuse", glm::vec3{0.5f});
    //   //   shaderProgram.SetVec3("pointLights[1].specular", glm::vec3{1.0f});
    //   //   shaderProgram.SetFloat("pointLights[1].constant", 1.0f);
    //   //   shaderProgram.SetFloat("pointLights[1].linear", 0.09f);
    //   //   shaderProgram.SetFloat("pointLights[1].quadratic", 0.032f);

    //   //   shaderProgram.SetVec3("pointLights[2].ambient", glm::vec3{0.2f});
    //   //   shaderProgram.SetVec3("pointLights[2].diffuse", glm::vec3{0.5f});
    //   //   shaderProgram.SetVec3("pointLights[2].specular", glm::vec3{1.0f});
    //   //   shaderProgram.SetFloat("pointLights[2].constant", 1.0f);
    //   //   shaderProgram.SetFloat("pointLights[2].linear", 0.09f);
    //   //   shaderProgram.SetFloat("pointLights[2].quadratic", 0.032f);

    //   //   shaderProgram.SetVec3("pointLights[3].ambient", glm::vec3{0.2f});
    //   //   shaderProgram.SetVec3("pointLights[3].diffuse", glm::vec3{0.5f});
    //   //   shaderProgram.SetVec3("pointLights[3].specular", glm::vec3{1.0f});
    //   //   shaderProgram.SetFloat("pointLights[3].constant", 1.0f);
    //   //   shaderProgram.SetFloat("pointLights[3].linear", 0.09f);
    //   //   shaderProgram.SetFloat("pointLights[3].quadratic", 0.032f);

    //   // }

    //   // // Spot light
    //   // shaderProgram.SetVec3("flashLight.position", g_Camera.GetPosition());
    //   // shaderProgram.SetVec3("flashLight.direction", g_Camera.GetForwardVector());
    //   // shaderProgram.SetFloat("flashLight.cutOff", std::cosf(glm::radians(12.5f)));
    //   // shaderProgram.SetFloat("flashLight.outerCutOff", std::cosf(glm::radians(17.5f)));
    //   // shaderProgram.SetVec3("flashLight.ambient", glm::vec3{0.2f});
    //   // shaderProgram.SetVec3("flashLight.diffuse", glm::vec3{0.5f});
    //   // shaderProgram.SetVec3("flashLight.specular", glm::vec3{1.0f});
    //   // shaderProgram.SetFloat("flashLight.constant", 1.0f);
    //   // shaderProgram.SetFloat("flashLight.linear", 0.09f);
    //   // shaderProgram.SetFloat("flashLight.quadratic", 0.032f);

    //   shaderProgram.SetVec3("viewPos", g_Camera.GetPosition());
    //   shaderProgram.SetMat4("projection", projection);
    //   shaderProgram.SetMat4("view", view);

    //   glm::mat4 model = glm::mat4{1.0f};
    //   model = glm::translate(model, glm::vec3{0.0f});
    //   model = glm::scale(model, glm::vec3{1.0f});
    //   shaderProgram.SetMat4("model", model);

    //   glStencilFunc(GL_ALWAYS, 1, 0xFF);
    //   glStencilMask(0xFF);

    //   // backpack
    //   ourModel.Draw(shaderProgram);
    // }
    
    // // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
    // // the objects' size differences, making it look like borders.
    // {
    //   glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //   glStencilMask(0x00);
    //   glDisable(GL_STENCIL_TEST);
    //   outlineShaderProgram.Activate();
    //   outlineShaderProgram.SetVec3("viewPos", g_Camera.GetPosition());
    //   outlineShaderProgram.SetMat4("projection", projection);
    //   outlineShaderProgram.SetMat4("view", view);
    //   const float scale = 1.1f;

    //   glm::mat4 model = glm::mat4{1.0f};
    //   model = glm::translate(model, glm::vec3{0.0f});
    //   model = glm::scale(model, glm::vec3{scale});
    //   outlineShaderProgram.SetMat4("model", model);

    //   // backpack outline
    //   ourModel.Draw(outlineShaderProgram);

    //   glStencilMask(0xFF);
    //   glStencilFunc(GL_ALWAYS, 0, 0xFF);
    //   glEnable(GL_STENCIL_TEST);
    // }
  }

}