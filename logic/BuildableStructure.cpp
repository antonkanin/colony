#include "BuildableStructure.h"

BuildableStructure::BuildableStructure(Shader& shader,
                                       std::unique_ptr<StructureView> view) :
  Buildable(shader),
  _view(std::move(view))
{
  _viewPtr = _view.get();
}

bool BuildableStructure::isUnderCursor(const glm::vec3& mousePoint)
{
  return _view->contain(mousePoint);
}
