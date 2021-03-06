#ifndef COLONY_WINDOW_H
#define COLONY_WINDOW_H

#include <GL/glew.h> // Initialize with glewInit()
#include <GLFW/glfw3.h>

#include "../fig/Camera.h"
#include "../fig/Window.h"
#include "../fig/events/Event.h"
#include "../logic/EventManager.h"

class EventFabric;
class ColonyWindow : public Window
{
public:
  ColonyWindow(glm::mat4& view,
               glm::mat4& projection,
               std::function<void(std::unique_ptr<Event> event)> onEvent,
               EventFabric* eventFabric);
  ~ColonyWindow() override;

  void preUpdate();
  void postUpdate();
  float width() const;
  float height() const;
  void getCursorPos(double* xpos, double* ypos) const;

  GLFWwindow* _window;

private:
  void showDebug();

  float _screenWidth;
  float _screenHeight;
  glm::mat4& _view;
  glm::mat4& _projection;

  static EventFabric* _eventFabric;
  static std::function<void(std::unique_ptr<Event> event)> _onEvent;
};

#endif
