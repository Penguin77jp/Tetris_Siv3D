#pragma once

#include <Siv3D.hpp>

namespace png {
  namespace tetris {
    class Controller {
    public:
      Controller() {}

      bool InputDown() {
        return KeyS.down();
      }
      bool InputUp() {
        return KeyW.down();
      }
      bool InputRight() {
        return KeyD.down();
      }
      bool InputLeft() {
        return KeyA.down();
      }
    private :
    };
  }
}
