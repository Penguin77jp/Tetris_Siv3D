#include "mino.h"
#include "gameScene.h"

using namespace png::tetris;

namespace png {
  namespace scene {
    BaseScene::BaseScene(const Camera& _cam)
      : cam(_cam)
      , inputBuffer(std::vector<int>()) { }

    Camera BaseScene::GetCamera() const { return cam; }

    void BaseScene::SetInputBuffer(int _input) {
      inputBuffer.push_back(_input);
    }

    void BaseScene::SetInputBuffer(std::vector<int>& _inputBuffer) {
      inputBuffer = _inputBuffer;
    }



    StaticScene::StaticScene(const Camera& _cam,
      EmbreeRenderScene _embreeObjList)
      : embreeScene(_embreeObjList)
      , cam(_cam) {}
    StaticScene::StaticScene(const Camera& _cam)
      : cam(_cam) {}

    Camera StaticScene::GetCamera() const { return cam; }
    void StaticScene::GetEmbreeRenderScene(EmbreeRenderScene& _embreeScene) { _embreeScene = embreeScene; };



    GameScene::GameScene(const Camera& _cam, int _updatingTimeFrequency, tetris::Controller _cont)
      : updatingTimeFrequency(_updatingTimeFrequency)
      , startTime(std::chrono::system_clock::now())
      , BaseScene(_cam)
      , handlingMino(NULL)
      , controller(_cont) {
      for (int y = 0; y < TETRIS_HEIGHT; ++y) {
        for (int x = 0; x < TETRIS_WIDTH; ++x) {
          grid[y][x] = false;
        }
      }
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

    std::vector<TetrisPosition> GameScene::GetBox() {
      std::vector<TetrisPosition> vec;
      if (handlingMino == NULL) {
        if (stashMino.size() == 0) {
          for (int i = 0; i < 7; ++i) {
            stashMino.push_back(i);
          }
        }
        stashMino.erase(stashMino.begin());
        handlingMino = new Mino(0);
      }
      else {
        if (controller.InputDown() && handlingMino->Movable(grid, png::tetris::TetrisPosition(0, -1))) {
          handlingMino->Move(png::tetris::TetrisPosition(0, -1));
        }
        else if (controller.InputRight()) {
          handlingMino->Move(png::tetris::TetrisPosition(1, 0));
        }
      }

      //futi
      {
        //tate
        for (int i = 0; i <= TETRIS_HEIGHT; ++i) {
          vec.push_back(TetrisPosition(0, i));
          vec.push_back(TetrisPosition(TETRIS_WIDTH, i));
        }
        // yoko
        for (int i = 1; i <= TETRIS_WIDTH; ++i) {
          vec.push_back(TetrisPosition(i, 0));
        }
      }
      //grid
      {

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
      if (ElapsedSec() < updatingTimeFrequency) {
        startTime = std::chrono::system_clock::now();
        return true;
      }
      return false;
    }

    int GameScene::ElapsedSec() const {
      auto time = (int)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count();
      return time;
    }
  }
}
