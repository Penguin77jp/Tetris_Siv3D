#include "mino.h"
#include "gameScene.h"

namespace png {
  namespace tetris {
    TetrisPosition TetrisPosition::operator+ (const TetrisPosition& a) {
      return TetrisPosition(this->x + a.x, this->y + a.y);
    }
    TetrisPosition TetrisPosition::operator= (const TetrisPosition& a) {
      return TetrisPosition(a.x, a.y);
    }
    TetrisPosition& TetrisPosition::operator+= (const TetrisPosition& a) {
      this->x += a.x;
      this->y += a.y;
      return *this;
    }

    std::vector<vec3> TetrisPosition2vec3(const std::vector<TetrisPosition>& ref) {
      std::vector<vec3> vec;
      for (int i = 0; i < ref.size(); ++i) {
        vec.push_back(vec3(
          ref[i].x + 0.5f,
          ref[i].y + 0.5f,
          0
        ));
      }
      return vec;
    }

    Mino::Mino(int type)
      : blockLocalPosi(std::vector<TetrisPosition>())
      , posi(TetrisPosition(TETRIS_WIDTH / 2, TETRIS_HEIGHT/ 2)) {
      if (true) {
        blockLocalPosi.push_back(TetrisPosition(-1.0, +0.0));
        blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
        blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
        blockLocalPosi.push_back(TetrisPosition(+0.0, +1.0));
      }
    }

    std::vector<TetrisPosition> Mino::GetPosi() {
      std::vector<TetrisPosition> vec = blockLocalPosi;
      for (int i = 0; i < vec.size(); ++i) {
        vec[i] += posi;
      }
      return vec;
    }


    void Mino::Move(TetrisPosition movingPosi) {
      posi += movingPosi;
    }

    bool Mino::Movable(Field field, TetrisPosition movingPosi) {
      auto tmpPosi = posi + movingPosi;
      for (int i = 0; i < blockLocalPosi.size(); ++i) {
        auto checkPosi = TetrisPosition(blockLocalPosi[i] + tmpPosi);
        if (field[checkPosi.x][checkPosi.y]) {
          return false;
        }
      }
      return true;
    }
  }
}
