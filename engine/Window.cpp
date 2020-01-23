#include <cstdio>
#include <iomanip>

#include "../globals.h"
#include "Window.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

Window* winPtr;

void Window::processInput(GLFWwindow* window)
{
  _camera.updateSpeed();
}

void error_callback(int error, const char* description)
{
  if (winPtr) {
    winPtr->error_cb(error, description);
  }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  if (winPtr) {
    winPtr->mouse_cb(window, xpos, ypos);
  }
}

void keyboard_callback(GLFWwindow* window,
                       int key,
                       int scancode,
                       int action,
                       int mods)
{
  if (winPtr) {
    winPtr->keyboard_cb(window, key, scancode, action, mods);
  }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if (winPtr) {
    winPtr->scroll_cb(window, xoffset, yoffset);
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (winPtr) {
    winPtr->mouse_button_cb(window, button, action, mods);
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  if (winPtr) {
    winPtr->mouse_cb(window, width, height);
  }
}

void Window::error_cb(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Window::mouse_cb(GLFWwindow* window, double xpos, double ypos)
{
  _eventManager->handleMouseMove(window, xpos, ypos);
}

void Window::keyboard_cb(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods)
{
  _eventManager->handleKeyPress(window, key, scancode, action, mods);
}

void Window::scroll_cb(GLFWwindow* window, double xoffset, double yoffset)
{
  _camera.zoom(yoffset);
}

void Window::mouse_button_cb(GLFWwindow* window,
                             int button,
                             int action,
                             int mods)
{
  _eventManager->handleMousePressed(button, action);
}

void Window::framebuffer_size_cb(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

Window::Window(std::unique_ptr<EventManager>& em,
               Camera& c,
               glm::mat4& view,
               glm::mat4& projection) :
  _camera(c),
  _eventManager(em), _view(view), _projection(projection)
{
  winPtr = this;
  glfwInit();
  const char* glsl_version = "#version 450";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  int count;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  const GLFWvidmode* mode = glfwGetVideoMode(monitors[1]);
  _screenWidth = mode->width;
  _screenHeight = mode->height - 150;
  _window =
    glfwCreateWindow(_screenWidth, _screenHeight, "LearnOPenGl", NULL, NULL);
  if (_window == NULL) {
    glfwTerminate();
    throw "Application ctor failed: failed to create window";
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui::GetStyle().WindowRounding = 0.0f;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);

  auto err = glewInit();
  if (err) {
    throw "Application ctor failed: Failed to initialize OpenGL loader!";
  }

  glViewport(0, 0, _screenWidth, _screenHeight);
  glfwSetErrorCallback(error_callback);
  glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
  glfwSetCursorPosCallback(_window, mouse_callback);
  glfwSetScrollCallback(_window, scroll_callback);
  glfwSetMouseButtonCallback(_window, mouse_button_callback);
  glfwSetKeyCallback(_window, keyboard_callback);

  glEnable(GL_DEPTH_TEST);
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
}

Window::~Window()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(_window);
  glfwTerminate();
}

float Window::width() const
{
  return _screenWidth;
}

float Window::height() const
{
  return _screenHeight;
}

void Window::preUpdate()
{
  glfwPollEvents();
  processInput(_window);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  logger.render();

  int display_w, display_h;
  glfwGetFramebufferSize(_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glm::vec4 clear_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.00f);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
}

void Window::postUpdate()
{

  showDebug();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glBindVertexArray(0);
  glfwSwapBuffers(_window);
}

void Window::showDebug()
{
  auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
               ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs;
  ImGui::Begin("3dCoordinates", NULL, flags);
  ImGui::SetWindowPos(ImVec2(0, _screenHeight - 22));
  ImGui::SetWindowSize(ImVec2(500, 22));
  auto pos = EventManager::unProject(_window, _view, _projection);
  std::stringstream ss;
  ss << "x:" << std::setw(5) << std::setprecision(2) << pos.x
     << "; y:" << std::setw(5) << std::setprecision(2) << pos.y
     << "; z: " << pos.z;
  ImGui::Text(ss.str().c_str());
  ImGui::End();
}
