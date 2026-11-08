#include "TmxLevel.h"

#include "tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

#if defined(_WIN32)
const char DIR_SEPARATOR = '\\';
#else
const char DIR_SEPARATOR = '/';
#endif

std::string GetParentDirectory(const std::string &filepath) {
  const size_t slashPos = filepath.find_last_of('/');
  std::string parentDir;
  if (slashPos != std::string::npos) {
    parentDir = filepath.substr(0, slashPos);
  }
  return parentDir;
}

std::string JoinPaths(const std::string &path, const std::string &subpath) {
  if (path.empty()) {
    return subpath;
  }
  if (subpath.empty()) {
    return path;
  }

  return path + DIR_SEPARATOR + subpath;
}

// Парсит шестнадцатеричную кодировку RGB
sf::Color ParseColor(const std::string &hexRGB) {
  char *pEnd = nullptr;
  const long hexValue = strtol(hexRGB.c_str(), &pEnd, 16);
  if (*pEnd != '\0') {
    throw std::runtime_error(hexRGB + " is not valid hex-encoded RGB color");
  }

  const uint8_t red = uint8_t((hexValue >> 16) % 256);
  const uint8_t green = uint8_t((hexValue >> 8) % 256);
  const uint8_t blue = uint8_t(hexValue % 256);

  return sf::Color(red, green, blue);
}

float ParseFloat(const std::string &str) {
  char *pEnd = nullptr;
  const float value = strtof(str.c_str(), &pEnd);
  if (*pEnd != '\0') {
    throw std::runtime_error("'" + str + "' is not a float number");
  }

  return value;
}

int TmxObject::GetPropertyInt(const std::string &propertyName) {
  return std::stoi(properties[propertyName].c_str());
}

float TmxObject::GetPropertyFloat(const std::string &propertyName) {
  return ParseFloat(properties[propertyName].c_str());
}

std::string TmxObject::GetPropertyString(const std::string &propertyName) {
  return properties[propertyName];
}

void TmxObject::MoveBy(const sf::Vector2f &movement) {
  rect.position.x += movement.x;
  rect.position.y += movement.y;
  sprite.move(movement);
}

void TmxObject::MoveTo(const sf::Vector2f &position) {
  rect.position.x = position.x;
  rect.position.y = position.y;
  sprite.setPosition(position);
}

bool TmxLevel::LoadFromFile(const std::string &filepath) {
  tinyxml2::XMLDocument levelFile;

  // Загружает XML в XMLDocument в памяти.
  if (levelFile.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
    throw std::runtime_error("Loading level \"" + filepath + "\" failed.");
  }

  // В формате TMX корневым должен быть элемент <map>.
  tinyxml2::XMLElement *map = levelFile.FirstChildElement("map");
  if (map == nullptr) {
    throw std::runtime_error("<map> element not found");
  }

  m_width = std::stoi(map->Attribute("width"));
  m_height = std::stoi(map->Attribute("height"));
  m_tileWidth = std::stoi(map->Attribute("tilewidth"));
  m_tileHeight = std::stoi(map->Attribute("tileheight"));

  // Читает описание тайлсета и GID первого тайла.
  tinyxml2::XMLElement *tilesetElement = map->FirstChildElement("tileset");
  if (tilesetElement == nullptr) {
    throw std::runtime_error("No tileset found in TMX file");
  }

  m_firstTileID = std::stoi(tilesetElement->Attribute("firstgid"));
  int tileCount = std::stoi(tilesetElement->Attribute("tilecount"));
  int columns = std::stoi(tilesetElement->Attribute("columns"));

  tinyxml2::XMLElement *image = tilesetElement->FirstChildElement("image");
  const std::string imageFilename = image->Attribute("source");
  const std::string imagePath =
      JoinPaths(GetParentDirectory(filepath), imageFilename);

  sf::Color matteColor = sf::Color(0, 0, 0, 0);
  if (image->Attribute("trans") != nullptr) {
    matteColor = ParseColor(image->Attribute("trans"));
  }

  sf::Image img;
  if (!img.loadFromFile(imagePath)) {
    std::cout << "Failed to load tile sheet." << std::endl;
    return false;
  }

  // Задаёт маскирующий цвет тайлсета: им заливается фон
  //  при наложении прозрачного изображения.
  img.createMaskFromColor(matteColor);
  // Загружает текстуру из файла.
  m_tilesetImage.loadFromImage(img);
  // Запрещает сглаживание при растяжении текстуры.
  m_tilesetImage.setSmooth(false);

  const int rows = tileCount / columns;

  // Собирает список текстурных прямоугольников.
  // Каждый из них — изображение одного тайла.
  std::vector<sf::IntRect> subRects;
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < columns; x++) {
      sf::IntRect rect;
      rect.position.y = y * m_tileHeight;
      rect.size.y = m_tileHeight;
      rect.position.x = x * m_tileWidth;
      rect.size.x = m_tileWidth;
      subRects.push_back(rect);
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  /// Разбор слоёв тайлов
  ///
  tinyxml2::XMLElement *layerElement = map->FirstChildElement("layer");
  while (layerElement) {
    TmxLayer layer;

    // Копирует прозрачность из XML,
    //  или делает слой непрозрачным (255).
    if (layerElement->Attribute("opacity") != nullptr) {
      float opacity = ParseFloat(layerElement->Attribute("opacity"));
      layer.opacity = static_cast<std::uint8_t>(255 * opacity);
    } else {
      layer.opacity = 255;
    }

    tinyxml2::XMLElement *layerDataElement =
        layerElement->FirstChildElement("data");
    if (layerDataElement == nullptr) {
      std::cout << "Bad map. No layer information found." << std::endl;
    }

    tinyxml2::XMLElement *tileElement =
        layerDataElement->FirstChildElement("tile");
    if (tileElement == nullptr) {
      std::cout << "Bad map. No tile information found." << std::endl;
      return false;
    }

    int x = 0;
    int y = 0;
    while (tileElement) {
      const int tileGID = std::stoi(tileElement->Attribute("gid"));
      const int subRectToUse = tileGID - m_firstTileID;

      // Определяет текстурный прямоугольник каждого тайла.
      if (subRectToUse >= 0) {
        sf::Sprite sprite(m_tilesetImage);
        sprite.setTextureRect(subRects[subRectToUse]);
        sprite.setPosition({static_cast<float>(x * m_tileWidth),
                            static_cast<float>(y * m_tileHeight)});
        sprite.setColor(sf::Color(255, 255, 255, layer.opacity));

        layer.tiles.push_back(sprite);
      }

      tileElement = tileElement->NextSiblingElement("tile");

      x++;
      if (x >= m_width) {
        x = 0;
        y++;
        if (y >= m_height) {
          y = 0;
        }
      }
    }

    m_layers.push_back(layer);

    layerElement = layerElement->NextSiblingElement("layer");
  }

  ////////////////////////////////////////////////////////////////////////////
  /// Сбор списка объектов.
  ///

  // Если есть слои объектов.
  if (map->FirstChildElement("objectgroup") != nullptr) {
    tinyxml2::XMLElement *objectGroupElement =
        map->FirstChildElement("objectgroup");
    while (objectGroupElement) {
      // Переходим к узлу <object>
      tinyxml2::XMLElement *objectElement;
      objectElement = objectGroupElement->FirstChildElement("object");

      while (objectElement) {
        // Считываем свойства объекта — тип, имя, позицию и т.д.
        std::string objectType;
        if (objectElement->Attribute("type") != nullptr) {
          objectType = objectElement->Attribute("type");
        }
        std::string objectName;
        if (objectElement->Attribute("name") != nullptr) {
          objectName = objectElement->Attribute("name");
        }
        float x = std::stof(objectElement->Attribute("x"));
        float y = std::stof(objectElement->Attribute("y"));
        float width = std::stof(objectElement->Attribute("width"));
        float height = std::stof(objectElement->Attribute("height"));

        sf::Sprite sprite(m_tilesetImage);
        sprite.setTextureRect(sf::IntRect({0, 0}, {0, 0}));
        sprite.setPosition({x, y});

        if (objectElement->Attribute("gid") != nullptr) {
          const size_t index =
              std::stoi(objectElement->Attribute("gid")) - m_firstTileID;
          width = static_cast<float>(subRects[index].size.x);
          height = static_cast<float>(subRects[index].size.y);
          sprite.setTextureRect(subRects[index]);
          sprite.setOrigin({0, height});
        }

        // Создаём объект
        TmxObject object;
        object.id = std::stoi(objectElement->Attribute("id"));
        object.name = objectName;
        object.type = objectType;
        object.sprite = sprite;

        sf::FloatRect objectRect;
        objectRect.position.y = y;
        objectRect.position.x = x;
        objectRect.size.y = height;
        objectRect.size.x = width;
        object.rect = objectRect;

        // Читаем свойства объекта
        tinyxml2::XMLElement *properties =
            objectElement->FirstChildElement("properties");
        if (properties != nullptr) {
          tinyxml2::XMLElement *prop =
              properties->FirstChildElement("property");
          if (prop != nullptr) {
            while (prop) {
              std::string propertyName = prop->Attribute("name");
              std::string propertyValue = prop->Attribute("value");

              object.properties[propertyName] = propertyValue;

              prop = prop->NextSiblingElement("property");
            }
          }
        }

        // Добавляем объект в список
        m_objects.push_back(object);

        objectElement = objectElement->NextSiblingElement("object");
      }
      objectGroupElement =
          objectGroupElement->NextSiblingElement("objectgroup");
    }
  } else {
    std::cout << "No object layers found..." << std::endl;
  }

  return true;
}

TmxObject TmxLevel::GetFirstObject(const std::string &name) const {
  // Только первый объект с заданным именем
  for (size_t i = 0; i < m_objects.size(); i++)
    if (m_objects[i].name == name)
      return m_objects[i];
  throw std::runtime_error("Object with name " + name + " was not found");
}

std::vector<TmxObject> TmxLevel::GetAllObjects(const std::string &name) const {
  // Все объекты с заданным именем
  std::vector<TmxObject> vec;
  for (size_t i = 0; i < m_objects.size(); i++) {
    if (m_objects[i].name == name) {
      vec.push_back(m_objects[i]);
    }
  }

  return vec;
}

sf::Vector2i TmxLevel::GetTileSize() const {
  return sf::Vector2i(m_tileWidth, m_tileHeight);
}

float TmxLevel::GetTilemapWidth() const {
  return static_cast<float>(m_tileWidth * m_width);
}

float TmxLevel::GetTilemapHeight() const {
  return static_cast<float>(m_tileHeight * m_height);
}

sf::Vector2f TmxLevel::GetTilemapSize() const {
  return sf::Vector2f(GetTilemapWidth(), GetTilemapHeight());
}

void TmxLevel::Draw(sf::RenderTarget &target) const {
  const sf::FloatRect viewportRect = target.getView().getViewport();

  for (const auto &layer : m_layers) {
    for (const auto &tile : layer.tiles) {
      if (viewportRect.findIntersection(tile.getLocalBounds()).has_value()) {
        target.draw(tile);
      }
    }
  }
}
