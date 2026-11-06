#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

const float ORIGIN_X = 500.0f;
const float ORIGIN_Y = 350.0f;
const float SCALE_X = 50.0f;
const float SCALE_Y = 7.5f;

float f1(float x) {
    return -x * x + 2.0f * x + 10.0f;
}

float f2(float x) {
    return x * x + 2.0f * x - 10.0f;
}

int determineZone(float x, float y) {
    const float y1 = f1(x);
    const float y2 = f2(x);
    const float intersectionX = std::sqrt(10.0f);

    if (y > y1 && y > y2)
        return 5;
    if (y < y1 && y < y2)
        return 2;
    if (x < -intersectionX)
        return 1;
    if (x > intersectionX)
        return 4;
    return 3;
}

std::string zoneName(int zone) {
    switch (zone) {
        case 1: return "Zone 1 - between graphs on the left";
        case 2: return "Zone 2 - below both graphs";
        case 3: return "Zone 3 - between graphs at the center";
        case 4: return "Zone 4 - between graphs on the right";
        case 5: return "Zone 5 - above both graphs";
        default: return "Unknown zone";
    }
}

void drawGraph(sf::RenderWindow& window, std::function<float(float)> func,
               float xMin, float xMax, float scaleX, float scaleY, sf::Color color) {
    sf::VertexArray graph(sf::LinesStrip);

    for (float x = xMin; x <= xMax; x += 0.02f) {
        const float y = func(x);
        const float screenX = ORIGIN_X + x * scaleX;
        const float screenY = ORIGIN_Y - y * scaleY;
        graph.append(sf::Vertex(sf::Vector2f(screenX, screenY), color));
    }

    window.draw(graph);
}

sf::Text makeText(const sf::Font& font, const std::string& value,
                  unsigned int size, float x, float y, sf::Color color) {
    sf::Text text;
    text.setFont(font);
    text.setString(value);
    text.setCharacterSize(size);
    text.setPosition(x, y);
    text.setFillColor(color);
    return text;
}

void drawGridAndAxes(sf::RenderWindow& window, const sf::Font& font) {
    const sf::Color gridColor(45, 58, 75);
    const sf::Color axisColor(205, 214, 224);
    const sf::Color labelColor(150, 165, 180);

    for (int x = -9; x <= 9; ++x) {
        const float sx = ORIGIN_X + x * SCALE_X;
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vertex(sf::Vector2f(sx, 50), gridColor);
        line[1] = sf::Vertex(sf::Vector2f(sx, 650), gridColor);
        window.draw(line);

        if (x != 0) {
            window.draw(makeText(font, std::to_string(x), 13,
                                 sx - 7, ORIGIN_Y + 8, labelColor));
        }
    }

    for (int y = -40; y <= 40; y += 5) {
        const float sy = ORIGIN_Y - y * SCALE_Y;
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vertex(sf::Vector2f(50, sy), gridColor);
        line[1] = sf::Vertex(sf::Vector2f(950, sy), gridColor);
        window.draw(line);

        if (y != 0 && y % 10 == 0) {
            window.draw(makeText(font, std::to_string(y), 13,
                                 ORIGIN_X + 10, sy - 8, labelColor));
        }
    }

    sf::VertexArray xAxis(sf::Lines, 2);
    xAxis[0] = sf::Vertex(sf::Vector2f(50, ORIGIN_Y), axisColor);
    xAxis[1] = sf::Vertex(sf::Vector2f(950, ORIGIN_Y), axisColor);
    window.draw(xAxis);

    sf::VertexArray yAxis(sf::Lines, 2);
    yAxis[0] = sf::Vertex(sf::Vector2f(ORIGIN_X, 50), axisColor);
    yAxis[1] = sf::Vertex(sf::Vector2f(ORIGIN_X, 650), axisColor);
    window.draw(yAxis);

    window.draw(makeText(font, "X", 18, 925, ORIGIN_Y + 10, axisColor));
    window.draw(makeText(font, "Y", 18, ORIGIN_X + 12, 55, axisColor));
    window.draw(makeText(font, "0", 13, ORIGIN_X + 8, ORIGIN_Y + 8, labelColor));
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Lab 2 - Graph zones");
    window.setFramerateLimit(60);

    sf::Font font;
    const std::vector<std::string> fontPaths = {
        "arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };

    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    if (!fontLoaded)
        return 1;

    sf::CircleShape userPoint(6.0f);
    userPoint.setOrigin(6.0f, 6.0f);
    userPoint.setFillColor(sf::Color(255, 82, 82));
    userPoint.setOutlineColor(sf::Color::White);
    userPoint.setOutlineThickness(2.0f);
    bool userPointExists = false;

    sf::Text coordinatesText = makeText(font, "Point is not selected", 20, 22, 18, sf::Color::White);
    sf::Text zoneText = makeText(font, "Click inside the window", 20, 22, 48,
                                 sf::Color(255, 213, 79));

    sf::RectangleShape infoPanel(sf::Vector2f(650, 86));
    infoPanel.setPosition(12, 10);
    infoPanel.setFillColor(sf::Color(24, 34, 52, 240));
    infoPanel.setOutlineColor(sf::Color(80, 98, 120));
    infoPanel.setOutlineThickness(1.0f);

    sf::RectangleShape legendPanel(sf::Vector2f(292, 86));
    legendPanel.setPosition(696, 10);
    legendPanel.setFillColor(sf::Color(24, 34, 52, 240));
    legendPanel.setOutlineColor(sf::Color(80, 98, 120));
    legendPanel.setOutlineThickness(1.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                const float mathX = (mousePos.x - ORIGIN_X) / SCALE_X;
                const float mathY = -(mousePos.y - ORIGIN_Y) / SCALE_Y;
                const int zone = determineZone(mathX, mathY);

                userPoint.setPosition(static_cast<float>(mousePos.x),
                                      static_cast<float>(mousePos.y));
                userPointExists = true;

                std::ostringstream coordinates;
                coordinates << std::fixed << std::setprecision(2)
                            << "Coordinates: (" << mathX << "; " << mathY << ")";
                coordinatesText.setString(coordinates.str());
                zoneText.setString(zoneName(zone));
            }
        }

        window.clear(sf::Color(18, 24, 38));

        sf::RectangleShape plotArea(sf::Vector2f(900, 600));
        plotArea.setPosition(50, 50);
        plotArea.setFillColor(sf::Color(13, 20, 32));
        window.draw(plotArea);

        drawGridAndAxes(window, font);
        drawGraph(window, f1, -6.0f, 6.0f, SCALE_X, SCALE_Y, sf::Color(66, 165, 245));
        drawGraph(window, f2, -6.0f, 6.0f, SCALE_X, SCALE_Y, sf::Color(255, 167, 38));

        window.draw(infoPanel);
        window.draw(legendPanel);
        window.draw(coordinatesText);
        window.draw(zoneText);

        sf::RectangleShape sample1(sf::Vector2f(25, 4));
        sample1.setPosition(710, 32);
        sample1.setFillColor(sf::Color(66, 165, 245));
        window.draw(sample1);
        window.draw(makeText(font, "y1 = -x^2 + 2x + 10", 17, 744, 20,
                             sf::Color(66, 165, 245)));

        sf::RectangleShape sample2(sf::Vector2f(25, 4));
        sample2.setPosition(710, 66);
        sample2.setFillColor(sf::Color(255, 167, 38));
        window.draw(sample2);
        window.draw(makeText(font, "y2 = x^2 + 2x - 10", 17, 744, 54,
                             sf::Color(255, 167, 38)));

        if (userPointExists)
            window.draw(userPoint);

        window.draw(makeText(font, "Left click: set a point    |    Esc: exit", 16,
                             20, 670, sf::Color(170, 184, 198)));
        window.display();
    }

    return 0;
}
