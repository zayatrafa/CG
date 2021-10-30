#ifndef FLAPPY_HPP_
#define FLAPPY_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

class Blocks;
class OpenGLWindow;

class Flappy {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  //void update(const GameData &gameData, float deltaTime);
  void update(const GameData &gameData);
  void setRotation(float rotation) { m_rotation = rotation; }

 private:
  friend Blocks;
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1};
  float m_rotation{};
  float m_scale{0.125f};
  float m_speedY{};    
  float m_gravity{};
  float m_gravitySpeed{};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_init{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

  abcg::ElapsedTimer m_trailBlinkTimer;
  abcg::ElapsedTimer m_blockCoolDownTimer;
};

#endif