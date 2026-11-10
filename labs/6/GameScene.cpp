#include "GameScene.h"
#include "GameView.h"
#include "EmbeddedResources.h"
#include <float.h> // Для FLT_EPSILON
#include <math.h>

// В SFML 3 sf::Keyboard — это namespace (enum class Key, isKeyPressed).
namespace Keyboard = sf::Keyboard;
using sf::Vector2f;

// Абсолютная скорость движения игрока.
static const float PLAYER_SPEED = 200;

// Ускорение свободного падения на уровне (пиксели в секунду за секунду).
static const float GRAVITY = 1200;

// Начальная скорость прыжка в пикселях в секунду.
static const float JUMP_SPEED = 360;

// Максимальная скорость падения. Ограничивает смещение за один кадр,
// чтобы игрок не проходил сквозь тонкие препятствия.
static const float MAX_FALL_SPEED = 480;

// Выполняет нормализацию вектора (приведение к единичной длине).
static Vector2f Normalize(const Vector2f &value) {
  const float length = std::hypotf(value.x, value.y);
  if (length < FLT_EPSILON) {
    return Vector2f(0, 0);
  }

  return value / length;
}

// Округляет компоненты вектора до ближайших целых чисел.
static Vector2f Round(const Vector2f &value) {
  return Vector2f(roundf(value.x), roundf(value.y));
}

// Определяет состояние клавиатуры и выбирает направление движения.
// Возвращает нормализованный вектор направления игрока.
static Vector2f GetPlayerDirection() {
  Vector2f direction;
  if (Keyboard::isKeyPressed(Keyboard::Key::W) ||
      Keyboard::isKeyPressed(Keyboard::Key::Up)) {
    direction.y = -1;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::S) ||
             Keyboard::isKeyPressed(Keyboard::Key::Down)) {
    direction.y = +1;
  }
  if (Keyboard::isKeyPressed(Keyboard::Key::A) ||
      Keyboard::isKeyPressed(Keyboard::Key::Left)) {
    direction.x = -1;
  } else if (Keyboard::isKeyPressed(Keyboard::Key::D) ||
             Keyboard::isKeyPressed(Keyboard::Key::Right)) {
    direction.x = +1;
  }

  return Normalize(direction);
}

// Возвращает true, если спрайт игрока пересекается с прямоугольником
// препятствия из области карты (у препятствий часто нет спрайта).
static bool PlayerHitsBlock(const TmxObject &player, const TmxObject &block) {
  return player.sprite.getGlobalBounds()
      .findIntersection(block.rect)
      .has_value();
}

GameScene *NewGameScene() {
  GameScene *pLogic = new GameScene;
  TmxLevel &level = pLogic->level;

  level.LoadFromMemory(EmbeddedResources::LEVEL.data,
                       EmbeddedResources::LEVEL.size,
                       EmbeddedResources::TILESET.data,
                       EmbeddedResources::TILESET.size);
  pLogic->player = level.GetFirstObject("player");
  pLogic->coins = level.GetAllObjects("coin");
  pLogic->enemies = level.GetAllObjects("enemy");
  pLogic->blocks = level.GetAllObjects("block");
  pLogic->startPosition = pLogic->player.sprite.getPosition();

  return pLogic;
}

void UpdateGameScene(void *pData, GameView &view, float deltaSec) {
  // Извлекаем указатель на GameLogic, ранее переданный в игровой цикл.
  GameScene *pLogic = reinterpret_cast<GameScene *>(pData);

  // Ограничиваем длительность кадра, чтобы при подвисании окна
  // игрок не проходил сквозь препятствия одним большим смещением.
  if (deltaSec > 1.0f / 30.0f) {
    deltaSec = 1.0f / 30.0f;
  }

  TmxObject &player = pLogic->player;

  // Горизонтальное движение задаётся клавиатурой (целые пиксели за кадр).
  const float stepX = Round(GetPlayerDirection() * PLAYER_SPEED * deltaSec).x;
  if (stepX != 0) {
    player.MoveBy({stepX, 0});

    // Препятствия блокируют движение: при столкновении отменяем шаг.
    for (const TmxObject &block : pLogic->blocks) {
      if (PlayerHitsBlock(player, block)) {
        player.MoveBy({-stepX, 0});
        break;
      }
    }
  }

  // Прыжок выполняется только по новому нажатию и по состоянию земли,
  // полученному в прошлом кадре. Удержание клавиши не даёт новый импульс.
  const bool jumpKeyPressed =
      Keyboard::isKeyPressed(Keyboard::Key::W) ||
      Keyboard::isKeyPressed(Keyboard::Key::Up);
  if (jumpKeyPressed && !pLogic->wasJumpKeyPressed && pLogic->isOnGround) {
    pLogic->fallVelocity = -JUMP_SPEED;
  }
  pLogic->wasJumpKeyPressed = jumpKeyPressed;

  // До вертикальной проверки землю нужно подтвердить заново.
  pLogic->isOnGround = false;

  // Гравитация постепенно направляет вертикальную скорость вниз.
  pLogic->fallVelocity += GRAVITY * deltaSec;
  if (pLogic->fallVelocity > MAX_FALL_SPEED) {
    pLogic->fallVelocity = MAX_FALL_SPEED;
  }
  const sf::FloatRect oldBounds = player.sprite.getGlobalBounds();
  const float stepY = pLogic->fallVelocity * deltaSec;
  player.MoveBy({0, stepY});

  // Землей считается только контакт сверху при движении вниз.
  // При подъёме столкновение прекращает движение вверх без установки земли.
  for (const TmxObject &block : pLogic->blocks) {
    if (PlayerHitsBlock(player, block)) {
      const bool landed =
          stepY > 0 &&
          oldBounds.position.y + oldBounds.size.y <=
              block.rect.position.y + 0.01f;
      player.MoveBy({0, -stepY});
      if (landed) {
        pLogic->isOnGround = true;
      }
      pLogic->fallVelocity = 0;
      break;
    }
  }

  // 1. Монеты исчезают при столкновении с игроком.
  for (auto it = pLogic->coins.begin(); it != pLogic->coins.end();) {
    if (player.sprite.getGlobalBounds()
            .findIntersection(it->sprite.getGlobalBounds())
            .has_value()) {
      it = pLogic->coins.erase(it); // игрок собирает монету
    } else {
      ++it;
    }
  }

  // 2. Враги телепортируют игрока на начальную позицию уровня.
  for (const TmxObject &enemy : pLogic->enemies) {
    if (player.sprite.getGlobalBounds()
            .findIntersection(enemy.sprite.getGlobalBounds())
            .has_value()) {
      player.sprite.setPosition(pLogic->startPosition);
      pLogic->fallVelocity = 0;
      pLogic->isOnGround = false;
    }
  }

  // Камера следит за игроком по горизонтали, а по вертикали держит дно
  // карты у нижнего края вида: земля внизу, пространство над уровнем сверху.
  SetCameraCenter(view, {player.sprite.getPosition().x,
                         pLogic->level.GetTilemapHeight() -
                             view.camera.getSize().y / 2.0f});
}

void DrawGameScene(void *pData, GameView &view) {
  // Извлекаем указатель на GameLogic, ранее переданный в игровой цикл.
  GameScene *pLogic = reinterpret_cast<GameScene *>(pData);
  sf::RenderTarget &target = view.window;

  pLogic->level.Draw(target);
  for (const TmxObject &coin : pLogic->coins) {
    target.draw(coin.sprite);
  }
  for (const TmxObject &enemy : pLogic->enemies) {
    target.draw(enemy.sprite);
  }
  target.draw(pLogic->player.sprite);
}

void DestroyGameScene(GameScene *&pScene) {
  delete pScene;
  pScene = nullptr;
}
