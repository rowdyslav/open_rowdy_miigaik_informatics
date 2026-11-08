#pragma once

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace tinyxml2 {
class XMLDocument;
}

// В SFML 3 спрайт обязан ссылаться на текстуру с момента создания:
// конструктора по умолчанию у sf::Sprite нет. Пустая текстура-заглушка
// позволяет хранить спрайт в структуре до привязки тайлсета карты.
// Объект намеренно не удаляется: деструктор sf::Texture нельзя вызывать
// после освобождения ресурсов SFML при завершении процесса.
inline const sf::Texture &GetEmptyTexture() {
  static const sf::Texture *const emptyTexture = new sf::Texture;
  return *emptyTexture;
}

struct TmxObject {
  int id;
  int GetPropertyInt(const std::string &propertyName);
  float GetPropertyFloat(const std::string &propertyName);
  std::string GetPropertyString(const std::string &propertyName);

  void MoveBy(const sf::Vector2f &movement);
  void MoveTo(const sf::Vector2f &position);

  std::string name;
  std::string type;
  sf::FloatRect rect;
  std::map<std::string, std::string> properties;

  sf::Sprite sprite{GetEmptyTexture()};
};

struct TmxLayer {
  std::uint8_t opacity = 0;
  std::vector<sf::Sprite> tiles;
};

class TmxLevel {
public:
  // Загружает данные из TMX в память объекта.
  bool LoadFromFile(const std::string &filepath);
  // Загружает TMX и его тайлсет из массивов байтов автономной сборки.
  bool LoadFromMemory(const void *levelData, std::size_t levelSize,
                      const void *tilesetData, std::size_t tilesetSize);

  TmxObject GetFirstObject(const std::string &name) const;
  std::vector<TmxObject> GetAllObjects(const std::string &name) const;
  sf::Vector2i GetTileSize() const;
  float GetTilemapWidth() const;
  float GetTilemapHeight() const;
  sf::Vector2f GetTilemapSize() const;

  // Рисует все слои тайлов один за другим,
  //  но не рисует объекты (рисованием которых должна заниматься игра).
  // Принимает любую цель для рисования, например, sf::RenderWindow.
  void Draw(sf::RenderTarget &target) const;

private:
  bool LoadFromDocument(tinyxml2::XMLDocument &levelFile,
                        const std::string &parentDirectory,
                        const void *tilesetData, std::size_t tilesetSize);

  int m_width = 0;
  int m_height = 0;
  int m_tileWidth = 0;
  int m_tileHeight = 0;
  int m_firstTileID = 0;
  sf::Texture m_tilesetImage;
  std::vector<TmxObject> m_objects;
  std::vector<TmxLayer> m_layers;
};
