#include <SFML/Graphics.hpp>
#include <optional>

namespace {
const sf::Color BACKGROUND(150, 170, 180);
const sf::Color MONITOR_FILL(200, 200, 200);
const sf::Color MONITOR_OUTLINE(70, 70, 70);
const sf::Color SCREEN_FILL(30, 30, 30);
const sf::Color STAND_FILL(180, 180, 180);
const sf::Color KEYBOARD_FILL(210, 210, 210);
const sf::Color KEYBOARD_OUTLINE(100, 100, 100);
const sf::Color KEY_FILL(230, 230, 230);
const sf::Color KEY_OUTLINE(120, 120, 120);
const sf::Color TOWER_FILL(190, 190, 190);
const sf::Color TOWER_OUTLINE(80, 80, 80);
const sf::Color BAY_FILL(230, 230, 230);
const sf::Color BAY_OUTLINE(100, 100, 100);
const sf::Color BUTTON_FILL(100, 180, 100);

sf::RectangleShape makeRect(sf::Vector2f position, sf::Vector2f size,
                            sf::Color fill) {
  sf::RectangleShape rect(size);
  rect.setPosition(position);
  rect.setFillColor(fill);
  return rect;
}

sf::RectangleShape makeRect(sf::Vector2f position, sf::Vector2f size,
                            sf::Color fill, sf::Color outline,
                            float thickness) {
  sf::RectangleShape rect = makeRect(position, size, fill);
  rect.setOutlineColor(outline);
  rect.setOutlineThickness(thickness);
  return rect;
}
} // namespace

int main() {
  sf::RenderWindow window(sf::VideoMode({610u, 499u}),
                          "SFML drawing - variant 4");
  window.setFramerateLimit(60);

  const sf::RectangleShape monitor = makeRect(
      {47.f, 32.f}, {300.f, 220.f}, MONITOR_FILL, MONITOR_OUTLINE, 4.f);
  const sf::RectangleShape screen =
      makeRect({87.f, 62.f}, {220.f, 160.f}, SCREEN_FILL);
  const sf::RectangleShape stand =
      makeRect({147.f, 252.f}, {100.f, 30.f}, STAND_FILL);

  const sf::RectangleShape keyboard = makeRect(
      {47.f, 312.f}, {400.f, 60.f}, KEYBOARD_FILL, KEYBOARD_OUTLINE, 2.f);

  const sf::RectangleShape tower =
      makeRect({447.f, 212.f}, {120.f, 250.f}, TOWER_FILL, TOWER_OUTLINE, 3.f);
  const sf::RectangleShape bay =
      makeRect({462.f, 242.f}, {90.f, 20.f}, BAY_FILL, BAY_OUTLINE, 2.f);

  sf::CircleShape powerButton(8.f);
  powerButton.setPosition({497.f, 412.f});
  powerButton.setFillColor(BUTTON_FILL);

  while (window.isOpen()) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    window.clear(BACKGROUND);
    window.draw(monitor);
    window.draw(screen);
    window.draw(stand);
    window.draw(tower);
    window.draw(bay);
    window.draw(powerButton);
    window.draw(keyboard);

    for (int row = 0; row < 3; ++row) {
      for (int col = 0; col < 12; ++col) {
        const float x = 56.f + static_cast<float>(col) * 30.f;
        const float y = 321.f + static_cast<float>(row) * 18.f;
        window.draw(makeRect({x + 1.f, y + 1.f}, {20.f, 15.f}, KEY_FILL,
                             KEY_OUTLINE, 1.f));
      }
    }

    window.display();
  }
  return 0;
}
