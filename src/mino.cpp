#include "mino.h"
#include "gameScene.h"

namespace png {
	namespace tetris {
		TetrisPosition TetrisPosition::operator+ (const TetrisPosition& a) const {
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
			, posi(TetrisPosition(TETRIS_WIDTH / 2, TETRIS_HEIGHT - 2))
			, minoType(type) {
			if (minoType == 0) {
				blockLocalPosi.push_back(TetrisPosition(-1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+2.0, +0.0));
			}
			else if (minoType == 1) {
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, +1.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +1.0));
			}
			else if (minoType == 2) {
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, -1.0));
				blockLocalPosi.push_back(TetrisPosition(-1.0, -1.0));
			}
			else if (minoType == 3) {
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(-1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, -1.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, -1.0));
			}
			else if (minoType == 4) {
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, +1.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+2.0, +0.0));
			}
			else if (minoType == 5) {
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(-1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(-2.0, +0.0));
			}
			else if (minoType == 6) {
				blockLocalPosi.push_back(TetrisPosition(-1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+1.0, +0.0));
				blockLocalPosi.push_back(TetrisPosition(+0.0, +1.0));
			}
		}

		std::vector<TetrisPosition> Mino::GetPosi() const {
			std::vector<TetrisPosition> vec = blockLocalPosi;
			for (int i = 0; i < vec.size(); ++i) {
				vec[i] += posi;
			}
			return vec;
		}


		void Mino::Move(TetrisPosition movingPosi) {
			posi += movingPosi;
		}

		void Mino::RotateClockwise() {
			for (int i = 0; i < blockLocalPosi.size(); ++i) {
				TetrisPosition tmp = blockLocalPosi[i];
				blockLocalPosi[i].x = tmp.y;
				blockLocalPosi[i].y = -tmp.x;
			}
		}

		void Mino::RotateCounterClockwise() {
			for (int i = 0; i < blockLocalPosi.size(); ++i) {
				TetrisPosition tmp = blockLocalPosi[i];
				blockLocalPosi[i].x = -tmp.y;
				blockLocalPosi[i].y = tmp.x;
			}
		}

		bool Mino::Movable(const Field& field, TetrisPosition movingPosi) const {
			auto tmpPosi = posi + movingPosi;
			for (int i = 0; i < blockLocalPosi.size(); ++i) {
				auto checkPosi = blockLocalPosi[i] + tmpPosi;
				if (field[checkPosi.y][checkPosi.x]) {
					return false;
				}
			}
			return true;
		}

		bool Mino::Rotatable(const Field& field, bool isClockwise) const {
			auto tmpLocalPosi = blockLocalPosi;
			for (int i = 0; i < tmpLocalPosi.size(); ++i) {
				if (isClockwise) {
					auto tmp = tmpLocalPosi[i];
					tmp.x = tmpLocalPosi[i].y + posi.x;
					tmp.y = -tmpLocalPosi[i].x + posi.y;
					if (field[tmp.y][tmp.x]) {
						return false;
					}
				}
				else {
					auto tmp = tmpLocalPosi[i];
					tmp.x = -tmpLocalPosi[i].y + posi.x;
					tmp.y = tmpLocalPosi[i].x + posi.y;
					if (field[tmp.y][tmp.x]) {
						return false;
					}
				}
			}
			return true;
		}
	}
}
