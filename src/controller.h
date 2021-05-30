#pragma once

#include <Siv3D.hpp>

namespace png {
  namespace tetris {
    class Controller {
    public:
      Controller() {}

      //move
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
      //rotate
      bool InputRotateClockwise() {
        return KeyE.down();
      }
      bool InputRotateCounterClockwise() {
        return KeyQ.down();
      }
    private :
    };
  }
}
