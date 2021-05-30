#include "mino.h"
#include "gameScene.h"

using namespace png::tetris;

namespace png {
	namespace scene {
		BaseScene::BaseScene(int texture_w, int texture_h, const Camera& _cam)
			: cam(_cam)
			, inputBuffer(std::vector<int>())
			, texture_w(texture_w)
			, texture_h(texture_h)
			, embreeRenderScene(EmbreeRenderScene()) { }

		Camera BaseScene::GetCamera() const { return cam; }

		void BaseScene::SetInputBuffer(int _input) {
			inputBuffer.push_back(_input);
		}

		void BaseScene::SetInputBuffer(std::vector<int>& _inputBuffer) {
			inputBuffer = _inputBuffer;
		}



		StaticScene::StaticScene(
			int texture_w,
			int texture_h,
			const Camera& _cam,
			EmbreeRenderScene _embreeObjList)
			: BaseScene(texture_w, texture_h, _cam) {}

		Image StaticScene::ComputeResultImage() const {
			return Image(1);
		}

		void StaticScene::GetEmbreeRenderScene(EmbreeRenderScene& _embreeScene) { _embreeScene = embreeRenderScene; };



		GameScene::GameScene(int texture_w, int texture_h, const Camera& _cam, float _updatingTimeFrequency, tetris::Controller _cont)
			: updatingTimeFrequency(_updatingTimeFrequency)
			, deltaTime(std::chrono::system_clock::now())
			, BaseScene(texture_w, texture_h, _cam)
			, handlingMino(NULL)
			, controller(_cont) {
			for (int y = 0; y < TETRIS_HEIGHT + 1; ++y) {
				for (int x = 0; x < TETRIS_WIDTH + 2; ++x) {
					if (x == 0 || x == TETRIS_WIDTH + 1 || y == 0) {
						grid[y][x] = true;
					}
					else {
						grid[y][x] = false;
					}
				}
			}
			renderer = new Renderer(texture_w, texture_h, embreeRenderScene);
		}

		void GameScene::InitEmbreeRenderSceneVector(std::vector<png::vec3>& vec, float& size) {
			const float space = 0.1f;
			//tate
			for (int i = 0; i <= TETRIS_HEIGHT; ++i) {
				vec.push_back(vec3(-(float)TETRIS_WIDTH / 2 - 0.5, (float)TETRIS_HEIGHT / 2 - 0.5 - i, +0.0f));
				vec.push_back(vec3(+(float)TETRIS_WIDTH / 2 - 0.5, (float)TETRIS_HEIGHT / 2 - 0.5 - i, +0.0f));
			}
			// yoko
			for (int i = 0; i < TETRIS_WIDTH; ++i) {
				vec.push_back(vec3(-(float)TETRIS_WIDTH / 2 + 0.5 + i, -(float)TETRIS_HEIGHT / 2 - 0.5, +0.0f));
			}
			size = 1.0 - 2.0 * space;
		}

		void GameScene::InitEmbreeRenderScene(EmbreeRenderScene& embreeScene) {
			std::vector<vec3> vec;
			float size;
			InitEmbreeRenderSceneVector(vec, size);
			//futi
			auto futiMat = new MaterialReflect(vec3{ 1.0f,0.1f,0.1f }, 0.8, 0.0);
			for (int i = 0; i < vec.size(); ++i) {
				embreeScene.list.emplace_back(std::make_shared<Box>(
					Box(vec[i], futiMat, size)
					));
			}

			auto light = new png::vec3(1.0, 1.0, 1.0);
			embreeScene.sceneLight = png::SceneLight(
				light, 0.7
			);
		}

		void GameScene::AddSceneGameObject(EmbreeRenderScene& embreeScene, vec3 vec, Material* mat, float size) {
			embreeScene.list.emplace_back(std::make_shared<Box>(
				Box(vec, mat, size)
				));
		}

		void GameScene::GetEmbreeRenderScene(EmbreeRenderScene& embreeScene) {
			embreeScene.list.erase(embreeScene.list.begin() + 2 * (TETRIS_HEIGHT + 1) + TETRIS_WIDTH, embreeScene.list.end());
			auto mat = new MaterialReflect(vec3(0.0, 1.0, 0.0), 1.0, 0.0);
			AddSceneGameObject(embreeScene, vec3(ElapsedSec(), -4, +0.0f), mat, 1.0);
		}

		Image GameScene::ComputeResultImage() const {
			//renderer->Draw(cam);
			return Image(1);
		}

		std::vector<TetrisPosition> GameScene::GetBox() {
			std::vector<TetrisPosition> vec;
			if (handlingMino == NULL) {
				if (stashMino.size() == 0) {
					for (int i = 0; i < 7; ++i) {
						stashMino.push_back(i);
					}
				}
				int randomMino = (int)(s3d::Random() * stashMino.size());
				handlingMino = new Mino(randomMino);
				stashMino.erase(stashMino.begin() + randomMino);
			}
			else {
				if (controller.InputDown() || isUpdateScene()) {
					if (handlingMino->Movable(grid, png::tetris::TetrisPosition(0, -1))) {
						handlingMino->Move(png::tetris::TetrisPosition(0, -1));
					}
					else {
						auto tmpPosi = handlingMino->GetPosi();
						for (int i = 0; i < tmpPosi.size(); ++i) {
							grid[tmpPosi[i].y][tmpPosi[i].x] = true;
						}

						//delete line
						while (true) {
							int cell = 0;
							for (int i = 0; i < grid[0].size(); ++i) {
								if (grid[1][i]) cell++;
							}
							if (cell == grid[0].size()) {
								for (int y = 1; y < grid.size(); ++y) {
									if (y == grid.size() - 1) {
										for (int x = 0; x < grid[0].size(); ++x) {
											grid[y][x] = false;
										}
									}
									else {
										for (int x = 0; x < grid[0].size(); ++x) {
											grid[y][x] = grid[y + 1][x];
										}
									}
								}
							}
							else {
								break;
							}
						}

						delete handlingMino;
						handlingMino = NULL;
					}
				}
				else if (controller.InputRight() && handlingMino->Movable(grid, png::tetris::TetrisPosition(1, 0))) {
					handlingMino->Move(png::tetris::TetrisPosition(1, 0));
				}
				else if (controller.InputLeft() && handlingMino->Movable(grid, png::tetris::TetrisPosition(-1, 0))) {
					handlingMino->Move(png::tetris::TetrisPosition(-1, 0));
				}
				else if (controller.InputRotateClockwise() && handlingMino->Rotatable(grid, true)) {
					handlingMino->RotateClockwise();
				}
				else if (controller.InputRotateCounterClockwise() && handlingMino->Rotatable(grid, false)) {
					handlingMino->RotateCounterClockwise();
				}
			}

			//grid
			{
				for (int y = 0; y < grid.size(); ++y) {
					for (int x = 0; x < grid[0].size(); ++x) {
						if (grid[y][x]) {
							vec.push_back(TetrisPosition(x, y));
						}
					}
				}
			}
			// handlingMino
			{
				if (handlingMino != NULL) {
					auto tmpPosi = handlingMino->GetPosi();
					for (int i = 0; i < tmpPosi.size(); ++i) {
						vec.push_back(tmpPosi[i]);
					}
				}
			}
			return vec;
		}

		bool GameScene::isUpdateScene() {
			if (ElapsedSec() >= updatingTimeFrequency) {
				return true;
			}
			return false;
		}

		void GameScene::UpdateDeltaTime() {
			deltaTime = std::chrono::system_clock::now();
		}

		void GameScene::Update() {
			if (handlingMino == NULL || controller.InputKeyDown() == 1 || isUpdateScene()) {
				//init renderer
			}
		}

		int GameScene::ElapsedSec() const {
			auto time = (int)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - deltaTime).count();
			ClearPrint();
			Print << U"{}"_fmt(time);
			return time;
		}
	}
}
