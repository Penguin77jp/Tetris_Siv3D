#pragma once

#include "ray.h"
#include "gameScene.h"

#include <vector>

namespace png {
  namespace tetris {
    struct TetrisPosition {
      TetrisPosition& operator+ (const TetrisPosition& a) {
        return TetrisPosition(this->x + a.x, this->y + a.y);
      }
      TetrisPosition& operator= (const TetrisPosition& a) {
        this->x = a.x;
        this->y = a.y;
        return *this;
      }
      TetrisPosition& operator+= (const TetrisPosition& a) {
        this->x += a.x;
        this->y += a.y;
        return *this;
      }

      int x, y;
    };

    class Mino {
    public:
      Mino(int type) {
        if (true) {
          blockLocalPosi.push_back(TetrisPosition(-1.0, +0.0));
          blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
          blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
          blockLocalPosi.push_back(TetrisPosition(+0.0, +1.0));
        }
      }

      void SetPosi(TetrisPosition movingPosi) {
        posi += movingPosi;
      }

      bool Movable(const scene::Field& field, TetrisPosition movingPosi) {
        auto tmpPosi = posi + movingPosi;
        for (int i = 0; i < blockLocalPosi.size(); ++i) {
          auto checkPosi = blockLocalPosi[i] + tmpPosi;
          if (field[checkPosi.x][checkPosi.y]) {
            return false;
          }
        }
        return true;
      }
    private:
      std::vector<TetrisPosition> blockLocalPosi;
      TetrisPosition posi;
    };
  }
}
