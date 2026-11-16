#include "GameScene.h"
#include "GameView.h"
#include <iostream>
#include <memory>
int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  try {
    // unique_ptr освобождает память даже при исключении —
    // например, при ошибке загрузки карты в NewGameScene.
    std::unique_ptr<GameView> pGameView(NewGameView({800, 600}));
    std::unique_ptr<GameScene> pGameScene(NewGameScene());
    // Аргумент типа GameScene* будет преобразован в void*.
    EnterGameLoop(*pGameView, UpdateGameScene, DrawGameScene, pGameScene.get());
  } catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
