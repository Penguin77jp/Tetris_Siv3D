#pragma once


#include <string>

namespace png {
  namespace scene {
    class GameObject {
    public:
      GameObject() {}
    };

    class GameText : public GameObject {
    private:
      std::string text;
    public:
      GameText(std::string _text) : text(_text) {}
    };
  }
}
