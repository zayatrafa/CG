#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
  }
}

void OpenGLWindow::initializeGL() {

  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "SpaceGrotesk-Regular.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the bird
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "flappy.vert",
                                        getAssetsPath() + "flappy.frag");

  // Clear window
  glClearColor(0, 0, 0, 1);

  #if !defined(__EMSCRIPTEN__)
    glEnable(GL_PROGRAM_POINT_SIZE);
  #endif

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;
  m_score = 0;
  m_flappy.initializeGL(m_objectsProgram);
  m_blocks.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 3 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 3) {
    restart();
    return;
  }

  //m_flappy.update(m_gameData, deltaTime);
  m_flappy.update(m_gameData);
  m_blocks.update(m_flappy, m_gameData, deltaTime);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
  }
}

void OpenGLWindow::paintGL() {

  update();

  glClear(GL_COLOR_BUFFER_BIT);

  if (m_elapsedTimer.elapsed() >= 800 / 1000.0){
    m_elapsedTimer.restart();
    if(m_gameData.m_state == State::Playing){
      m_score += 1;
    }
  }

  m_blocks.paintGL();
  m_flappy.paintGL(m_gameData);

  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto widgetSize{ImVec2(m_viewportWidth - 10, 105)};
    ImGui::SetNextWindowPos(ImVec2(5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse };
    
	 ImGui::Begin("Flappy!", nullptr, windowFlags);

    ImGui::Text("Pontos: %d", m_score);
    ImGui::Separator();
    ImGui::Text("Escolha a dificuldade:");
    ImGui::RadioButton("Fácil", &m_blocks.m_delay, 1500);
    ImGui::SameLine();
    ImGui::RadioButton("Médio", &m_blocks.m_delay, 1000);
    ImGui::SameLine();
    ImGui::RadioButton("Difícil", &m_blocks.m_delay, 500);

    ImGui::End();
  }

  {
    auto size{ImVec2(300, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over :(");
    }

    ImGui::PopFont();
    ImGui::End();
  }

}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {

  glDeleteProgram(m_objectsProgram);

  m_blocks.terminateGL();
  m_flappy.terminateGL();
}

void OpenGLWindow::checkCollisions() {

  for (auto &block : m_blocks.m_blocks) {
    if (block.m_dead) continue;

    auto distance{glm::distance(m_flappy.m_translation, block.m_translation)};

    if (distance < m_flappy.m_scale * 0.9f + block.m_scale * 0.85f) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
    }

  }

}