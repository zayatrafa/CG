#include "flappy.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Flappy::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_init.x = -0.75;
  m_init.y = -0.85;
  m_gravity = -0.00005;
  //m_init.y = 0;
  m_translation.x = m_init.x;
  m_translation.y = m_init.y;
  m_translation.y = 0;
  m_velocity = glm::vec2(0);

  m_speedY = 0.0f;    
  m_gravity = 0.05;
  m_gravitySpeed = 0.0f;

  std::array<glm::vec2, 16> positions{
      // Ship body
		
		//0
		glm::vec2{-02.5f, +5.5f},
		//1
		glm::vec2{-15.5f, +0.5f},
		//2
      glm::vec2{-20.5f, -12.5f}, 
		//3
		glm::vec2{-09.5f, -07.5f},
		//4
      glm::vec2{-03.5f, -12.5f}, 
		//5
		glm::vec2{+03.5f, -12.5f},
		//6
      glm::vec2{+09.5f, -07.5f}, 
		//7
		glm::vec2{+15.5f, +5.5f},
		//8
		glm::vec2{+15.5f, +5.5f}, 
		//9
		glm::vec2{+02.5f, +12.5f},
		
		//Asa esquerda
		//10
		glm::vec2{-15.5f, +20.5f},
		//11
		glm::vec2{-15.5f, +10.5f},
		//12
		glm::vec2{-3.5f, +3.5f},
		
		//Asa direita
		//13
		glm::vec2{-3.5f, +20.5f},
		//14
		glm::vec2{0.5f, +20.5f},
		//15
		glm::vec2{-2.5f, +5.5f},


      };

  // Normalize
  for (auto &position :positions) {
		position /= glm::vec2{25.5f, 25.5f};
  }

  std::array indices{0, 1, 3,
                     1, 2, 3,
                     0, 3, 4,
                     0, 4, 5,
                     9, 0, 5,
                     9, 5, 6,
                     9, 6, 8,
							//asas
							10,11,12,
							13,14,15};


  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Flappy::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform1f(m_rotationLoc, m_rotation);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)] || gameData.m_input[static_cast<size_t>(Input::Fire)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

      glDisable(GL_BLEND);
    }
  }

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Flappy::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Flappy::update(const GameData &gameData) {
  if (gameData.m_input[static_cast<size_t>(Input::Up)] || gameData.m_input[static_cast<size_t>(Input::Fire)]){

    m_gravity = 0.0005;

    if(m_translation.y < 0.5){
      m_gravitySpeed += m_gravity;
      m_translation.y += m_speedY + m_gravitySpeed;
    }

    if(m_translation.y >= 0.5){
      m_translation.y = 0.5;
      m_speedY = 0.2f;
      m_gravitySpeed = 0.1f;
    }

    if(m_translation.y <= m_init.y){
      m_translation.y = m_init.y;
      m_gravitySpeed = 0.1f;
    }


  }else{
    if(m_translation.y >= 0.5){
      m_translation.y = 0.5;
      m_speedY = 0.2f;
      m_gravitySpeed = 0.1f;
    }
    m_gravity = -0.00005;
    if(m_translation.y > m_init.y){
      m_gravitySpeed += m_gravity;
      m_translation.y += m_speedY + m_gravitySpeed;
    }
    if(m_translation.y <= m_init.y){
      m_translation.y = m_init.y;
      m_gravitySpeed = 0.1f;
    }
  }

}
