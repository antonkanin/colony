#include <string>

#include "../imgui/imgui.h"

#include "../math/Noise.h"
#include "TerrainMesh.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

struct RgbColor
{
  float r;
  float g;
  float b;
};

using HeightPart = float;
std::map<HeightPart, RgbColor> colorMapping = { { 0.0f, { 113, 128, 143 } },
                                                { 0.5f, { 237, 227, 143 } },
                                                { 1.0f, { 242, 127, 115 } } };

TerrainMesh::TerrainMesh()
{
  _vao = 0;
  _vertexVbo = 0;
  _indicesEbo = 0;
  deinit();

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vertexVbo);
  glGenBuffers(1, &_indicesEbo);
}

TerrainMesh::~TerrainMesh()
{
  deinit();
}

void TerrainMesh::deinit()
{
  if (_vertexVbo != 0) {
    glDeleteBuffers(1, &_vertexVbo);
    glDeleteBuffers(1, &_indicesEbo);
  }

  if (_vao != 0) {
    glDeleteVertexArrays(1, &_vao);
    _vao = 0;
  }
}

void TerrainMesh::render()
{
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _v.size() * 3, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void TerrainMesh::initTerrain(float bottomLeftX,
                              float bottomLeftY,
                              float topRightX,
                              float topRightY,
                              int divisions,
                              float xyScale,
                              float zScale)
{
  _v.reserve((divisions + 1) * 2 * divisions);
  _width = topRightX - bottomLeftX;
  _height = topRightY - bottomLeftY;
  _xStep = (topRightX - bottomLeftX) / divisions;
  _yStep = (topRightY - bottomLeftY) / divisions;
  _xyScale = xyScale;
  _zScale = zScale;

  ImGui::Begin("surface_mountain");
  static float frequency = 0.3;
  static float frequencyFactor = 2.0;
  static float amplitudeFactor = 0.6;
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::SetWindowSize(ImVec2(500, 110));
  ImGui::SliderFloat("frequency slider", &frequency, 0.0f, 1.5f);
  ImGui::SliderFloat("frequencyFactor slider", &frequencyFactor, 0.0f, 3.0f);
  ImGui::SliderFloat("amplitudeFactor slider", &amplitudeFactor, 0.1f, 1.5f);
  ImGui::End();
  ImGui::Begin("surface_plain");
  static float frequency_plain = 0.077;
  static float frequencyFactor_plain = 3.0;
  static float amplitudeFactor_plain = 0.366;
  ImGui::SetWindowPos(ImVec2(0, 340));
  ImGui::SetWindowSize(ImVec2(500, 110));
  ImGui::SliderFloat("frequency slider", &frequency_plain, 0.0f, 1.5f);
  ImGui::SliderFloat(
    "frequencyFactor slider", &frequencyFactor_plain, 0.0f, 3.0f);
  ImGui::SliderFloat(
    "amplitudeFactor slider", &amplitudeFactor_plain, 0.1f, 1.5f);
  ImGui::End();
  auto noise = Noise(777);
  auto max = 0.0f;
  auto min = 0.0f;
  int width = divisions + 1;
  std::vector<float> plainZ;
  float x, y;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < width; ++j) {
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
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < width; ++j) {
      VertexColor vertex;
      vertex.p.x = bottomLeftX + static_cast<float>(i) * _xStep;
      vertex.p.y = bottomLeftY + static_cast<float>(j) * _yStep;
      glm::vec2 derivs;
      auto nv = noise.fractal(glm::vec2(vertex.p.x, vertex.p.y),
                              derivs,
                              frequency,
                              frequencyFactor,
                              amplitudeFactor,
                              5);
      vertex.p.x *= _xyScale;
      vertex.p.y *= _xyScale;
      vertex.p.z = ::max(nv * _zScale, plainZ.at(i * width + j));
      min = std::min(min, vertex.p.z);
      max = std::max(max, vertex.p.z);
      vertex.normal = glm::vec3(0.0f);

      _v.push_back(vertex);
      if (j != 0 && j != (width - 1)) {
        _v.push_back(vertex);
      }
    }
  }
  auto augmentedWidth = divisions + 1 + (divisions + 1 - 2);
  _latticeWidth = augmentedWidth;
  _latticeHeight = width;
  for (int i = 0; i < width - 1; ++i) {
    for (int j = 0; j < augmentedWidth; ++j) {
      glm::vec3 p0(0);
      glm::vec3 p1(0);
      glm::vec3 p2(0);
      auto rectangleTypeNum = ((i % 2) * 2 + j) % 4;
      if (rectangleTypeNum == 0) {
        p1 = _v.at(augmentedWidth * i + j + 1 + augmentedWidth).p;
        p2 = _v.at(augmentedWidth * i + j).p;
        p0 = _v.at(augmentedWidth * i + j + augmentedWidth).p;
      } else if (rectangleTypeNum == 1) {
        p1 = _v.at(augmentedWidth * i + j - 1).p;
        p2 = _v.at(augmentedWidth * i + j + augmentedWidth).p;
        p0 = _v.at(augmentedWidth * i + j).p;
      } else if (rectangleTypeNum == 2) {
        p1 = _v.at(augmentedWidth * i + j + augmentedWidth).p;
        p2 = _v.at(augmentedWidth * i + j + 1).p;
        p0 = _v.at(augmentedWidth * i + j).p;
      } else if (rectangleTypeNum == 3) {
        p1 = _v.at(augmentedWidth * i + j).p;
        p2 = _v.at(augmentedWidth * i + j - 1 + augmentedWidth).p;
        p0 = _v.at(augmentedWidth * i + j + augmentedWidth).p;
      }

      _v[augmentedWidth * i + j].normal = glm::cross(p1 - p0, p2 - p0);
    }
  }
  auto amplitude = max - min;
  std::cout << "max= " << max << std::endl;
  std::cout << "min= " << min << std::endl;
  std::cout << "amplitude= " << amplitude << std::endl;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < augmentedWidth; ++j) {
      RgbColor a, b;
      auto h = (_v[augmentedWidth * i + j].p.z - min) / amplitude;
      if (h <= amplitude * 0.2) {
        a = colorMapping[0.0f];
        b = colorMapping[0.5f];
        h *= 2;
      } else {
        a = colorMapping[0.5f];
        b = colorMapping[1.0f];
        h = (h - 0.5) * 2;
      }
      _v[augmentedWidth * i + j].color.x = glm::lerp(a.r, b.r, h) / 255.0;
      _v[augmentedWidth * i + j].color.y = glm::lerp(a.g, b.g, h) / 255.0;
      _v[augmentedWidth * i + j].color.z = glm::lerp(a.b, b.b, h) / 255.0;
    }
  }
  _indices.reserve(::pow(divisions, 2) * 2 * 3);
  for (int i = 0; i < divisions; ++i) {
    for (int j = 0; j < divisions; ++j) {
      auto j2 = j * 2;
      if (((i % 2) + j) % 2 == 0) {
        _indices.push_back(i * augmentedWidth + j2);
        _indices.push_back(i * augmentedWidth + j2 + augmentedWidth);
        _indices.push_back(i * augmentedWidth + j2 + augmentedWidth + 1);

        _indices.push_back(i * augmentedWidth + j2 + 1);
        _indices.push_back(i * augmentedWidth + j2);
        _indices.push_back(i * augmentedWidth + j2 + augmentedWidth + 1);
      } else {
        _indices.push_back(i * augmentedWidth + j2);
        _indices.push_back(i * augmentedWidth + j2 + augmentedWidth);
        _indices.push_back(i * augmentedWidth + j2 + 1);

        _indices.push_back(i * augmentedWidth + j2 + 1);
        _indices.push_back(i * augmentedWidth + j2 + augmentedWidth);
        _indices.push_back(i * augmentedWidth + j2 + augmentedWidth + 1);
      }
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vertexVbo);
  glBufferData(
    GL_ARRAY_BUFFER, sizeof(VertexColor) * _v.size(), &_v[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(VertexColor),
                        (void*)offsetof(VertexColor, color));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(VertexColor),
                        (void*)offsetof(VertexColor, normal));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(_indices[0]) * _indices.size(),
               &_indices[0],
               GL_STATIC_DRAW);

  glBindVertexArray(0);
}

float TerrainMesh::getZ(float x, float y) const
{
  x += _width / 2;
  y += _height / 2;
  /* std::cout << "x= " << x << std::endl; */
  /* std::cout << "y= " << y << std::endl; */
  /* std::cout << "_xStep= " << _xStep << std::endl; */
  /* std::cout << "_yStep= " << _yStep << std::endl; */
  auto i = ::floor(x / _xStep / _xyScale);
  auto j = ::floor(y / _yStep / _xyScale);
  /* std::cout << "i= " << i << std::endl; */
  /* std::cout << "j= " << j << std::endl; */
  auto mappedJ = (j == 0) ? 0 : 2 * j - 1;
  /* std::cout << "mappedJ= " << mappedJ << std::endl; */
  return _v.at(i * _latticeWidth + mappedJ).p.z;
}
