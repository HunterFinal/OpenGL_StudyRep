#include <print>
#include <memory>

// glad should include before glfw
#include "glad/glad.h"
#include "glfw3/glfw3.h"

namespace
{
  static const char* WINDOW_TITLE = "LearnOpenGL";
  constexpr uint32_t WINDOW_WIDTH = 800;
  constexpr uint32_t WINDOW_HEIGHT = 600;

  static void framebuffer_size_callback(GLFWwindow*, /**width */GLsizei, /**height */GLsizei);
  static void pollGLFWEvent(GLFWwindow*);

  static void processInput(GLFWwindow*);
}

int main()
{
  std::print("Hello GCC!");

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
  
    while(!glfwWindowShouldClose(window))
    {
      processInput(window);
  
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

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