#ifndef SHELL_VIEW_H
#define SHELL_VIEW_H

#include "../fig/Model.h"
#include "../fig/Shader.h"
#include "View.h"
#include <GL/glew.h>

class ShellView : public View
{
public:
  ShellView(Shader& shader, glm::vec3 position);
  void draw() override;
  void move(glm::vec2 moveIncrement);

private:
};

#endif
