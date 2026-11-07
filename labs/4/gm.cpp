#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <optional>
#include <random>
#include <vector>

const unsigned int WORLD_WIDTH  = 65;
const unsigned int WORLD_HEIGHT = 65;
const unsigned int CELL_SIZE    = 10;
const sf::Color LIVE_COLOR = sf::Color::White;
const sf::Color DEAD_COLOR = sf::Color::Black;
const sf::Color GRID_COLOR = sf::Color::Red;

const std::array<unsigned int, 1> B_RULE = {1};
const std::array<unsigned int, 9> S_RULE = {0, 1, 2, 3, 4, 5, 6, 7, 8};

struct Point {
    bool is_live = false;
};

bool applyRule(bool isLive, unsigned int liveNeighbors) {
    if (isLive) {
        return std::find(S_RULE.begin(), S_RULE.end(), liveNeighbors) != S_RULE.end();
    }
    return std::find(B_RULE.begin(), B_RULE.end(), liveNeighbors) != B_RULE.end();
}

void initWorld(std::vector<std::vector<Point>>& world) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (unsigned int y = 0; y < WORLD_HEIGHT; ++y) {
        for (unsigned int x = 0; x < WORLD_WIDTH; ++x) {
            world[y][x].is_live = dis(gen) == 1;
        }
    }
}

void initWorldManual(std::vector<std::vector<Point>>& world) {
    for (auto& row : world) {
        for (auto& cell : row) {
            cell.is_live = false;
        }
    }
    world[WORLD_HEIGHT / 2][WORLD_WIDTH / 2].is_live = true;
}

void updateWorld(std::vector<std::vector<Point>>& world,
                 const std::vector<std::vector<Point>>& prevWorld) {
    auto countLiveNeighbors = [&](unsigned int x, unsigned int y) -> unsigned int {
        unsigned int count = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = static_cast<int>(x) + dx;
                int ny = static_cast<int>(y) + dy;
                if (nx >= 0 && ny >= 0 && nx < static_cast<int>(WORLD_WIDTH)
                    && ny < static_cast<int>(WORLD_HEIGHT)
                    && prevWorld[ny][nx].is_live) {
                    ++count;
                }
            }
        }
        return count;
    };

    for (unsigned int y = 0; y < WORLD_HEIGHT; ++y) {
        for (unsigned int x = 0; x < WORLD_WIDTH; ++x) {
            unsigned int liveNeighbors = countLiveNeighbors(x, y);
            world[y][x].is_live = applyRule(prevWorld[y][x].is_live, liveNeighbors);
        }
    }
}

bool worldsAreEqual(const std::vector<std::vector<Point>>& w1,
                    const std::vector<std::vector<Point>>& w2) {
    for (unsigned int y = 0; y < WORLD_HEIGHT; ++y) {
        for (unsigned int x = 0; x < WORLD_WIDTH; ++x) {
            if (w1[y][x].is_live != w2[y][x].is_live) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    sf::RenderWindow window(
        sf::VideoMode({WORLD_WIDTH * CELL_SIZE, WORLD_HEIGHT * CELL_SIZE}),
        "H-Trees B1/S012345678"
    );

    std::vector<std::vector<Point>> world(WORLD_HEIGHT, std::vector<Point>(WORLD_WIDTH));
    std::vector<std::vector<Point>> prevWorld(WORLD_HEIGHT, std::vector<Point>(WORLD_WIDTH));

    initWorldManual(world);

    sf::Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text statusText(font);
    statusText.setCharacterSize(24);
    statusText.setFillColor(sf::Color::Red);

    sf::Clock clock;
    const sf::Time updateInterval = sf::seconds(0.2f);

    bool optimalDetected = false;
    bool showGrid = false;
    bool isPaused = false;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::G) {
                    showGrid = !showGrid;
                }
                else if (keyPressed->code == sf::Keyboard::Key::P) {
                    isPaused = !isPaused;
                    if (isPaused) statusText.setString("Paused");
                }
                else if (keyPressed->code == sf::Keyboard::Key::R) {
                    initWorldManual(world);
                    isPaused = false;
                    optimalDetected = false;
                    statusText.setString("");
                }
            }
        }

        if (!isPaused && clock.getElapsedTime() >= updateInterval) {
            clock.restart();
            prevWorld = world;
            updateWorld(world, prevWorld);

            if (worldsAreEqual(world, prevWorld)) {
                statusText.setString("Optimal configuration detected");
                optimalDetected = true;
            }

            bool allDead = true;
            for (const auto& row : world) {
                for (const auto& cell : row) {
                    if (cell.is_live) { allDead = false; break; }
                }
                if (!allDead) break;
            }
            if (allDead) {
                statusText.setString("All points died");
                optimalDetected = true;
            }
        }

        window.clear(sf::Color::Black);

        for (unsigned int y = 0; y < WORLD_HEIGHT; ++y) {
            for (unsigned int x = 0; x < WORLD_WIDTH; ++x) {
                sf::RectangleShape cell({
                    static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)
                });
                cell.setPosition({
                    static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE)
                });
                cell.setFillColor(world[y][x].is_live ? LIVE_COLOR : DEAD_COLOR);
                window.draw(cell);
            }
        }

        if (showGrid) {
            sf::VertexArray gridLines(sf::PrimitiveType::Lines);
            for (unsigned int i = 0; i <= WORLD_WIDTH; ++i) {
                gridLines.append(sf::Vertex{
                    {static_cast<float>(i * CELL_SIZE), 0.0f}, GRID_COLOR});
                gridLines.append(sf::Vertex{
                    {static_cast<float>(i * CELL_SIZE),
                     static_cast<float>(WORLD_HEIGHT * CELL_SIZE)}, GRID_COLOR});
            }
            for (unsigned int i = 0; i <= WORLD_HEIGHT; ++i) {
                gridLines.append(sf::Vertex{
                    {0.0f, static_cast<float>(i * CELL_SIZE)}, GRID_COLOR});
                gridLines.append(sf::Vertex{
                    {static_cast<float>(WORLD_WIDTH * CELL_SIZE),
                     static_cast<float>(i * CELL_SIZE)}, GRID_COLOR});
            }
            window.draw(gridLines);
        }

        if (optimalDetected || isPaused) {
            window.draw(statusText);
        }

        window.display();
    }
    return 0;
}
