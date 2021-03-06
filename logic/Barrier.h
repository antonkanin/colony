#ifndef BARRIER_H
#define BARRIER_H

#include <glm/glm.hpp>

#include "../fig/Terrain.h"
#include "../view/BarrierView.h"
#include "AbstractPlant.h"
#include "EnergyStructure.h"
#include "Shroud.h"

using EnergyStructures = std::vector<EnergyStructure*>;
class Barrier : public EnergyStructure
{
public:
  Barrier(Shader& textureShader,
          Shader& linesShader,
          glm::vec3 position,
          Terrain* terrain,
          AStar* router);
  void render() override;
  UnitBuilders getUnitBuilders(Game* game) override;
  StructureBuilders getStructureBuilders() override;
  void addPlant(std::shared_ptr<AbstractPlant> p);
  void addEnergyStructure(EnergyStructure* es);
  void commit() override;
  float radius() const;
  std::shared_ptr<Shroud> shroud();

private:
  Shroud _shroud;

  float _radius{ 1.0f };
  Terrain* _terrain;
  std::shared_ptr<LivingArea> _livingArea{ nullptr };
  EnergyStructures _energyStructures;
  Plants _plants;
  Timer _clock;
  std::chrono::milliseconds _bioUpdateTime{ 1000 };
  static const int BARRIER_HP;
};

#endif
