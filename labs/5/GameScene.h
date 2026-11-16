#pragma once
#include "TmxLevel.h"

struct GameView;

struct GameScene {
  TmxLevel level;
  TmxObject player;
  std::vector<TmxObject> enemies;
  std::vector<TmxObject> coins;
  // Препятствия (в том числе невидимые стены) и начальная
  // позиция игрока, куда его возвращают враги при столкновении.
  std::vector<TmxObject> blocks;
  sf::Vector2f startPosition;
  // Вертикальная скорость: положительная вниз, отрицательная вверх.
  float fallVelocity = 0;
  bool isOnGround = false;
  bool wasJumpKeyPressed = false;
};
GameScene *NewGameScene();
void UpdateGameScene(void *pData, GameView &view, float deltaSec);
void DrawGameScene(void *pData, GameView &view);
void DestroyGameScene(GameScene *&pScene);
