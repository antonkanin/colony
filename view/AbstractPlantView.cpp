#include "AbstractPlantView.h"

AbstractPlantView::AbstractPlantView(Shader& shader, glm::vec3 position) :
  View(shader, position)
{
}

void AbstractPlantView::draw()
{
  _shader.use();
  _shader.configure();
  _shader.setBool("animated", false);
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, _position);
  model = glm::scale(model, glm::vec3(_scaleFactor));
  _shader.setTransformation("model", glm::value_ptr(model));
  _model->render();
}