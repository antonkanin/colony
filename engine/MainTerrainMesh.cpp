#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "../imgui/imgui.h"

#include "../math/Noise.h"
#include "MainTerrainMesh.h"
#include "TerrainMeshSegment.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

struct RgbColor
{
  RgbColor() = default;
  RgbColor(float red, float green, float blue) : r(red), g(green), b(blue)
  {
  }
  float r;
  float g;
  float b;
};

static float color0[3] = { 113.0f / 255, 128.0f / 255, 143.0f / 255 };
static float color1[3] = { 237.0f / 255, 227.0f / 255, 143.0f / 255 };
static float color2[3] = { 242.0f / 255, 127.0f / 255, 115.0f / 255 };
using HeightPart = float;
std::map<HeightPart, RgbColor> colorMapping = {
  { 0.0f, RgbColor(color0[0], color0[1], color0[2]) },
  { 0.5f, RgbColor(color1[0], color1[1], color1[2]) },
  { 1.0f, RgbColor(color2[0], color2[1], color2[2]) }
};

void MainTerrainMesh::calculateHeights(unsigned int width,
                                       float bottomLeftX,
                                       float bottomLeftY,
                                       float& min,
                                       float& max)
{
  static float frequency = 0.204;
  static float frequencyFactor = 2.0;
  static float amplitudeFactor = 0.6;
  static float frequency_plain = 0.077;
  static float frequencyFactor_plain = 4.0;
  static float amplitudeFactor_plain = 0.366;
  static bool plains = false;
  ImGui::Begin("nonPlain");
  ImGui::SetWindowPos(ImVec2(0, 500));
  ImGui::SetWindowSize(ImVec2(350, 110));
  ImGui::Checkbox("plains", &plains);
  ImGui::SliderFloat("frequency", &frequency, 0.0f, 1.0f);
  ImGui::SliderFloat("frequencyFactor", &frequencyFactor, 0.0f, 3.0f);
  ImGui::SliderFloat("amplitudeFactor", &amplitudeFactor, 0.0f, 1.0f);
  ImGui::End();
  if (plains) {
    ImGui::Begin("plain");
    ImGui::SetWindowPos(ImVec2(0, 620));
    ImGui::SetWindowSize(ImVec2(350, 90));
    ImGui::SliderFloat("frequency", &frequency_plain, 0.0f, 1.0f);
    ImGui::SliderFloat("frequencyFactor", &frequencyFactor_plain, 0.0f, 8.0f);
    ImGui::SliderFloat("amplitudeFactor", &amplitudeFactor_plain, 0.0f, 1.0f);
    ImGui::End();
  }
  auto noise = Noise(777);
  std::vector<float> plainZ;
  float x, y;
  for (unsigned int i = 0; i < width; ++i) {
    for (unsigned int j = 0; j < width; ++j) {
      auto dummy = glm::vec2();
      x = bottomLeftX + static_cast<float>(i) * _xStep;
      y = bottomLeftY + static_cast<float>(j) * _yStep;
      auto nv_plain = noise.fractal(glm::vec2(x, y),
                                    dummy,
                                    frequency_plain,
                                    frequencyFactor_plain,
                                    amplitudeFactor_plain,
                                    5);
      plainZ.push_back(nv_plain);
    }
  }
  std::vector<float> waterZ;
  for (unsigned int i = 0; i < width; ++i) {
    for (unsigned int j = 0; j < width; ++j) {
      auto dummy = glm::vec2();
      x = bottomLeftX + static_cast<float>(i) * _xStep;
      y = bottomLeftY + static_cast<float>(j) * _yStep;
      waterZ.push_back(-0.3f);
    }
  }
  float f = 1.0;
  float t = 1.0;
  static float R = 4.0 * M_PI;
  static float S = 6.0 * M_PI;
  static float coefLongitude = 2.0f / 8;
  static float coefLatitude = 2.0f / 8;
  for (unsigned int i = 0; i < width; ++i) {
    for (unsigned int j = 0; j < width; ++j) {
      VertexColor vertex;
      vertex.p.x = bottomLeftX + static_cast<float>(i) * _xStep;
      vertex.p.y = bottomLeftY + static_cast<float>(j) * _yStep;
      glm::vec2 derivs;
      ImGui::Begin("R");
      ImGui::SetWindowPos(ImVec2(0, 350));
      ImGui::SetWindowSize(ImVec2(200, 80));
      ImGui::SliderFloat("R", &R, 0.0f, 100.0f * M_PI);
      ImGui::SliderFloat("S", &S, 0.0f, 100.0f * M_PI);
      ImGui::End();
      auto a = (vertex.p.x - bottomLeftX) / _width;
      auto topRightX = bottomLeftX + _width;
      auto b = (topRightX - vertex.p.x) / _width;
      auto c = (vertex.p.y - bottomLeftY) / _height;
      auto topRightY = bottomLeftY + _height;
      auto d = (topRightY - vertex.p.y) / _height;
      f = 1.0f;
      t = 1.0f;
      auto func = [](float x, float coef) {
        return x / (coef);
      };
      if (a < coefLongitude) {
        f = func(a, coefLongitude);
      } else if (b < coefLongitude) {
        f = func(b, coefLongitude);
      }
      if (c < coefLatitude) {
        t = func(c, coefLatitude);
      } else if (d < coefLatitude) {
        t = func(d, coefLatitude);
      }

      auto nv = noise.fractal(glm::vec2(vertex.p.x, vertex.p.y),
                              derivs,
                              frequency,
                              frequencyFactor,
                              amplitudeFactor,
                              5);
      auto nonPlain = nv * _zScale;
      float plain;
      float water;
      if (plains) {
        plain = plainZ.at(i * width + j);
        water = waterZ.at(i * width + j);
        auto water = waterZ.at(i * width + j);
      } else {
        plain = nonPlain;
        water = nonPlain;
      }
      auto m = [](float& np, float& p, float mult) {
        float npf = 1;
        np = np * mult - (npf - mult);
        p = p * mult - (npf - mult) * (npf - mult);
      };
      if (a < coefLongitude) {
        m(nonPlain, plain, f);
      } else if (b < coefLongitude) {
        m(nonPlain, plain, f);
      }
      if (c < coefLatitude) {
        m(nonPlain, plain, t);
      } else if (d < coefLatitude) {
        m(nonPlain, plain, t);
      }

      if (nonPlain > plain) {
        vertex.p.z = std::max(nonPlain, water);
        _obstaclesMap.push_back(true);
      } else {
        vertex.p.z = std::max(plain, water);
        _obstaclesMap.push_back(false);
      }

      vertex.p.y -= _height / 2.0f;
      vertex.p.x -= _width / 2.0f;

      min = std::min(min, vertex.p.z);
      max = std::max(max, vertex.p.z);
      vertex.normal = glm::vec3(0.0f);

      _v.push_back(vertex);
      if (j != 0 && j != (width - 1)) {
        _v.push_back(vertex);
      }
    }
  }
}

void MainTerrainMesh::calculateColors(float min,
                                      float max,
                                      unsigned int width,
                                      unsigned int augmentedWidth)
{
  ImGui::Begin("color");
  ImGui::SetWindowPos(ImVec2(100, 0));
  ImGui::SetWindowSize(ImVec2(200, 120));
  ImGui::ColorEdit3("color0", color0);
  ImGui::ColorEdit3("color1", color1);
  ImGui::ColorEdit3("color2", color2);
  ImGui::End();
  auto amplitude = max - min;
  for (unsigned int i = 0; i < width; ++i) {
    for (unsigned int j = 0; j < augmentedWidth; ++j) {
      auto z = _v[augmentedWidth * i + j].p.z;
      if (z != -0.3f) {
        RgbColor a, b;
        auto h = (_v[augmentedWidth * i + j].p.z - min) / amplitude;
        if (h <= amplitude * 0.2) {
          a = RgbColor(color0[0], color0[1], color0[2]);
          b = RgbColor(color1[0], color1[1], color1[2]);
          h *= 2;
        } else {
          a = RgbColor(color1[0], color1[1], color1[2]);
          b = RgbColor(color2[0], color2[1], color2[2]);
          h = (h - 0.5) * 2;
        }
        _v[augmentedWidth * i + j].color.x = glm::lerp(a.r, b.r, h);
        _v[augmentedWidth * i + j].color.y = glm::lerp(a.g, b.g, h);
        _v[augmentedWidth * i + j].color.z = glm::lerp(a.b, b.b, h);
        _v[augmentedWidth * i + j].color.w = 1.0;
      } else {
        _v[augmentedWidth * i + j].color.x = 0.0;
        _v[augmentedWidth * i + j].color.y = 0.8;
        _v[augmentedWidth * i + j].color.z = 1.0;
        /* _v[augmentedWidth * i + j].color.x = 0.0f; */
        /* _v[augmentedWidth * i + j].color.y = 0.0f; */
        /* _v[augmentedWidth * i + j].color.z = 0.0f; */
        /* _v[augmentedWidth * i + j].color.w = 1.0; */
      }
    }
  }
}

float MainTerrainMesh::getZ(float x, float y) const
{
  x += _width / 2;
  y += _height / 2;
  auto i = ::floor(x / _xStep);
  auto j = ::floor(y / _yStep);
  auto mappedJ = (j == 0) ? 0 : 2 * j - 1;
  return _v.at(i * _latticeAugmentedWidth + mappedJ).p.z;
}

glm::vec3 MainTerrainMesh::getRgbColor(float x, float y) const
{
  x += _width / 2;
  y += _height / 2;
  auto i = ::floor(x / _xStep);
  auto j = ::floor(y / _yStep);
  auto mappedJ = (j == 0) ? 0 : 2 * j - 1;
  auto c = _v.at(i * _latticeAugmentedWidth + mappedJ).color;
  return glm::vec3(c.x, c.y, c.z);
}

void MainTerrainMesh::getSegmentObstaclesMap(glm::vec2 bottomLeft,
                                             glm::vec2 topRight,
                                             std::vector<bool>& m,
                                             SegmentDimensions* sd)
{
  sd->divisionsX = (topRight.x - bottomLeft.x) / _xStep;
  sd->divisionsY = (topRight.y - bottomLeft.y) / _yStep;
  sd->latticeWidth = sd->divisionsY + 1;
  sd->xStep = _xStep;
  sd->yStep = _yStep;

  bottomLeft.x += _width / 2;
  topRight.x += _width / 2;
  bottomLeft.y += _height / 2;
  topRight.y += _height / 2;

  unsigned int startI = _latticeHeight * bottomLeft.x / _width;
  unsigned int startJ = _latticeWidth * bottomLeft.y / _height;

  for (unsigned int i = startI; i < startI + sd->divisionsX + 1; ++i) {
    for (unsigned int j = startJ; j < startJ + sd->divisionsY + 1; ++j) {
      m.push_back(_obstaclesMap.at(_latticeWidth * i + j));
    }
  }
}
