#pragma once

#include "gameScene.h"

#include <array>
#include <vector>

class Field;

namespace png {
  namespace tetris {
    // x : 0 -> TETRIS_WIDTH
    // y : 0 -> TETRIS_HEIGHT
    struct TetrisPosition {
      TetrisPosition operator+ (const TetrisPosition& a)const;
      TetrisPosition operator= (const TetrisPosition& a);
      TetrisPosition& operator+= (const TetrisPosition& a);

      int x, y;
    };

    std::vector<vec3> TetrisPosition2vec3(const std::vector<TetrisPosition>& ref);


    class Mino {
    public:
      Mino(int type);

      std::vector<TetrisPosition> GetPosi() const;

      void Move(TetrisPosition movingPosi);
      void RotateClockwise();
      void RotateCounterClockwise();
      bool Movable(const Field& field, TetrisPosition movingPosi) const;
      bool Rotatable(const Field& field, bool isClockwise) const;
    private:
      std::vector<TetrisPosition> blockLocalPosi;
      TetrisPosition posi;
      int minoType;
    };
  }
}
