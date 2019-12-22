#include "Barrier.h"
#include "../globals.h"
#include "PlantBuilder.h"
#include "TreeBuilder.h"

const int Barrier::BARRIER_HP = 200;

Barrier::Barrier(Shader& textureShader,
                 Shader& linesShader,
                 glm::vec3 position,
                 Terrain* terrain) :
  EnergyStructure(textureShader,
                  linesShader,
                  std::make_unique<BarrierView>(textureShader,
                                                linesShader,
                                                position,
                                                terrain)),
  _terrain(terrain)
{

  _health = BARRIER_HP;
  _maxHealth = _health;
}

void Barrier::render()
{
  if (_stage == BuildStage::Done) {
    // TODO downcast!
    BarrierView* v = dynamic_cast<BarrierView*>(_view.get());
    v->drawShroud();
    if (v->onOrbit()) {
      v->startAnimation();
    }
    if (v->shroudSetUp()) {
      v->drawBeam();
      Buildable::render();
      if (_livingArea != nullptr) {
        if (_clock.elapsed() >= _bioUpdateTime) {
          logger.log("updating area...");
          _terrain->updateLivingArea(_livingArea);
          _clock.reload();
        }
      }
    }
  } else {
    Buildable::render();
  }
}

UnitBuilders Barrier::getUnitBuilders(Game* game)
{
  auto builders = UnitBuilders();
  std::unique_ptr<AbstractUnitBuilder> pb =
    std::make_unique<PlantBuilder>(_shader, game, *this, _terrain);
  builders.push_back(std::move(pb));

  std::unique_ptr<AbstractUnitBuilder> tb =
    std::make_unique<TreeBuilder>(_shader, game, *this, _terrain);
  builders.push_back(std::move(tb));

  return builders;
}

StructureBuilders Barrier::getStructureBuilders()
{
  return StructureBuilders();
}

void Barrier::addPlant(std::shared_ptr<AbstractPlant> p)
{
  _livingArea->plants.push_back(p->position());
  _plants.push_back(p);
}

void Barrier::commit()
{
  auto p = _view->position();
  // TODO downcast
  BarrierView* v = dynamic_cast<BarrierView*>(_view.get());
  CircularRegion r = { p.x, p.y, v->radius() };
  auto c = _terrain->getRgbColor(p.x, p.y);
  _livingArea = _terrain->addLivingArea(r, glm::vec4(c.x, c.y, c.z, 0.5));
  BuildableStructure::commit();
}

glm::vec3 Barrier::shroudPosition() const
{
  // TODO downcast
  BarrierView* v = dynamic_cast<BarrierView*>(_view.get());
  std::cout << "v->shroudPosition().z= " << v->shroudPosition().z << std::endl;
  return v->shroudPosition();
}

void Barrier::addEnergyStructure(EnergyStructure* es)
{
  _energyStructures.push_back(es);
  // TODO downcast
  BarrierView* v = dynamic_cast<BarrierView*>(_view.get());
  v->grow(_livingArea);
}
