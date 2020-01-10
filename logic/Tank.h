#ifndef TANK_H
#define TANK_H

#include <GL/glew.h> // Initialize with glewInit()

#include <list>

#include "../Timer.h"
#include "../engine/LinesObject.h"
#include "../engine/Terrain.h"
#include "../view/TankView.h"
#include "BuildableAttackUnit.h"

class Game;

using Shells = std::list<Shell>;

enum class HealthLevel { Low, Medium, High };

class Tank : public BuildableAttackUnit
{
public:
  enum class Type { Light, Medium, Heavy };

  Tank() = delete;
  Tank(Shader& textureShader,
       Shader& linesShader,
       AStar* router,
       glm::vec3 position,
       Type type = Type::Light,
       HealthLevel healthLevel = HealthLevel::High,
       Shell::Size shellSize = Shell::Size::Small);

  bool isInsideArea(Points area);
  UnitBuilders getUnitBuilders(Game* game) override;
  StructureBuilders getStructureBuilders() override;

  void setRoute(glm::vec3 endPoint) override;
  void move();
  bool isMoving();

  /* void shootTarget() override; */
  /* void startShooting(Buildable* other) override; */
  /* void stopShooting() override; */
  /* bool isShooting() override; */

  /* void updateShells(); */
  void setTerrain(Terrain* terrain);

private:
  void startMoving(glm::vec2 endPoint) override;
  void stopMoving();
  /* float getTargetAngle(); */
  /* float getTargetDistance(); */
  float getMouseAngle(int mouseX, int mouseY);
  /* void reload(); */
  /* bool isShellLoaded(); */

  float _speed;
  glm::vec2 _moveIncrement{ 0, 0 };
  Type _type;
  /* Shell::Size _shellSize; */
  /* Shells _shells; */
  /* Buildable* _target; */
  glm::vec2 _destination;
  /* Timer _clock; */
  /* std::chrono::milliseconds _shellLoadTime{ 200 }; */
  Terrain* _terrain;

  APath _movingRoute;
  static const float MOVE_STOP_TOL;
};

std::shared_ptr<Tank> createTank(Game* game,
                                 Shader& textureShader,
                                 Shader& linesShader,
                                 AStar* router,
                                 glm::vec3 position,
                                 Tank::Type type = Tank::Type::Light,
                                 HealthLevel health = HealthLevel::High,
                                 Shell::Size shellSize = Shell::Size::Small);
#endif
