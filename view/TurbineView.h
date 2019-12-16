#ifndef TURBINE_VIEW_H
#define TURBINE_VIEW_H

#include "../engine/Beam.h"
#include "StructureView.h"

class TurbineView : public StructureView
{
public:
  TurbineView(Shader& textureShader,
              Shader& linesShader,
              glm::vec3 position,
              glm::vec3 shroudPosition);
  void draw() override;

private:
  Shader& _linesShader;
  Timer _timer;
  Beam _beam;
  static std::chrono::milliseconds TURBINE_CYCLE;
  static float TURBINE_HEALTH_BAR_WIDTH;
  static float TURBINE_HEALTH_BAR_HEIGHT;
  static float TURBINE_SCALE_FACTOR;
  static float TURBINE_MODEL_HEIGHT;
};

#endif
