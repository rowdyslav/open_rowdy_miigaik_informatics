#include "GameView.h"

GameView *NewGameView(const sf::Vector2i &windowSize) {
  GameView *pView = new GameView;
  pView->windowSize = windowSize;

  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;
  pView->window.create(sf::VideoMode(sf::Vector2u(unsigned(windowSize.x),
                                                  unsigned(windowSize.y))),
                       "Tiled Map Parser Demo", sf::Style::Close,
                       sf::State::Windowed, settings);
  pView->window.setFramerateLimit(60);

  pView->camera = sf::View(
      sf::FloatRect({0.0f, 0.0f}, {float(windowSize.x), float(windowSize.y)}));
  pView->camera.setViewport(sf::FloatRect({0.0f, 0.0f}, {2.0f, 2.0f}));

  return pView;
}

void EnterGameLoop(GameView &view, OnUpdate onUpdate, OnDraw onDraw,
                   void *pData) {
  while (view.window.isOpen()) {
    while (const std::optional<sf::Event> event = view.window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        view.window.close();
        break;
      }
    }

    const sf::Time elapsedTime = view.clock.getElapsedTime();
    view.clock.restart();
    onUpdate(pData, view, elapsedTime.asSeconds());

    view.window.clear();
    onDraw(pData, view);
    view.window.display();
  }
}

void DestroyGameView(GameView *&pView) {
  delete pView;
  pView = nullptr;
}

void SetCameraCenter(GameView &view, const sf::Vector2f &center) {
  view.camera.setCenter(center);
  view.window.setView(view.camera);
}
