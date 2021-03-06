#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include <GLFW/glfw3.h>

#include "../fig/Terrain.h"
#include "AbstractPlant.h"
#include "AttackUnit.h"
#include "Barrier.h"
#include "Control.h"
#include "VehicleGroup.h"

class Tank;
class AbstractStructureBuilder;

using AttackUnits = std::vector<std::shared_ptr<AttackUnit>>;
using Structures = std::vector<std::shared_ptr<GroundStructure>>;
using Shrouds = std::vector<std::shared_ptr<Shroud>>;

class Game
{
public:
  Game(glm::mat4& view, glm::mat4& projection);
  void tick();
  void addTank(std::shared_ptr<Tank> tank);
  void addStructure(std::shared_ptr<GroundStructure> buildable);
  void addShroud(std::shared_ptr<Shroud> shroud);
  void addPlant(std::shared_ptr<AbstractPlant> plant);
  void addTerrain(Terrain* terrain);
  void setControl(std::unique_ptr<Control> control);

  AttackUnit* getAttackUnit(const glm::vec3& mousePoint, bool select = false);
  VehicleGroup getTanks(Points area);
  Buildable* getStructure(const glm::vec3& mousePoint);
  void clearPanel(Panel::Type type);
  bool panelIsEmpty(Panel::Type type);
  glm::vec3 getNearestShroudPositionFlat(glm::vec3 p) const;
  glm::vec3 getNearestShroudPositionGlobe(glm::vec3 p) const;
  void addBarrierPotential(EnergyStructure* es);

private:
  Shroud* getNearestShroud(glm::vec3 p) const;
  void displayTanks();
  void displayShells();
  void displayStructures();
  void displayPlants();
  void displayControl();

  glm::mat4& _view;
  glm::mat4& _projection;
  AttackUnits _tanks;
  Structures _structures;
  Shrouds _shrouds;
  Plants _plants;
  std::unique_ptr<Control> _control;
  AttackUnit* _selectedAttackUnit = nullptr;
  GroundStructure* _selectedStructure = nullptr;
  Terrain* _terrain;
};

#endif
