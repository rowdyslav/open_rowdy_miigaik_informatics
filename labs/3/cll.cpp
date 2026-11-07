#include <SFML/Graphics.hpp>
#include <optional>

const int N = 10;
const float CELL_SIZE = 50.0f;

int main() {
  sf::RenderWindow window(sf::VideoMode({500u, 500u}),
                          L"Лабораторная работа № 3 — Вариант 4",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    window.clear(sf::Color::White);

    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        sf::RectangleShape cell({CELL_SIZE, CELL_SIZE});
        cell.setPosition({x * CELL_SIZE, (N - 1 - y) * CELL_SIZE});

        const bool inTriangle = (x + y <= N - 2);
        cell.setFillColor(inTriangle ? sf::Color(70, 130, 180)
                                     : sf::Color::White);
        cell.setOutlineColor(sf::Color::Black);
        cell.setOutlineThickness(-1.0f);
        window.draw(cell);
      }
    }

    window.display();
  }

  return 0;
}
