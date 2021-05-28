#pragma once

#include "gameObject.h"
#include "sceneData.h"
#include "camera.h"

#include <memory>
#include <vector>
#include <chrono>


namespace png {
  namespace scene {
    class BaseScene {
    public:
      BaseScene(const Camera& _cam)
        : cam(_cam)
        , inputBuffer(std::vector<int>()) { }
      Camera GetCamera() const { return cam; }
      virtual void GetEmbreeRenderScene(EmbreeRenderScene&) = 0;
      void SetInputBuffer(int _input) {
        inputBuffer.push_back(_input);
      }
      void SetInputBuffer(std::vector<int>& _inputBuffer) {
        inputBuffer = _inputBuffer;
      }
      int Hoge() {}

    private:
      Camera cam;
      std::vector<int> inputBuffer;
    };

    class StaticScene {
    public:
      StaticScene(const Camera& _cam,
        EmbreeRenderScene _embreeObjList)
        : embreeScene(_embreeObjList)
        , cam(_cam) {}
      StaticScene(const Camera& _cam)
        : cam(_cam) {}

      Camera GetCamera() const { return cam; }
      void GetEmbreeRenderScene(EmbreeRenderScene& _embreeScene) { _embreeScene = embreeScene; };

    private:
      EmbreeRenderScene  embreeScene;
      Camera cam;
    };

    constexpr unsigned int TETRIS_WIDTH = 10;
    constexpr unsigned int TETRIS_HEIGHT = 10;
    class GameScene : public BaseScene {
    public:
      GameScene(const Camera& _cam)
        : startTime(std::chrono::system_clock::now())
        , timeUpdated(ElapsedSec())
        , BaseScene(_cam) {
        for (int y = 0; y < TETRIS_HEIGHT; ++y) {
          for (int x = 0; x < TETRIS_WIDTH; ++x) {
            grid[y][x] = false;
          }
        }
      }

      void InitEmbreeRenderScene(EmbreeRenderScene& embreeScene) {
        const float space = 0.1f;
        //futi
        auto futiMat = new MaterialReflect(vec3{ 1.0f,0.1f,0.1f }, 0.8, 0.0);
        //tate
        for (int i = 0; i <= TETRIS_HEIGHT; ++i) {
          embreeScene.list.emplace_back(std::make_shared<Box>(
            Box(vec3(-(float)TETRIS_WIDTH / 2 - 0.5, (float)TETRIS_HEIGHT / 2 - 0.5 - i, +0.0f), futiMat, 1.0 - 2.0*space)
            ));
          embreeScene.list.emplace_back(std::make_shared<Box>(
            Box(vec3(+(float)TETRIS_WIDTH / 2 - 0.5, (float)TETRIS_HEIGHT / 2 - 0.5 - i, +0.0f), futiMat, 1.0 - 2.0 * space)
            ));
        }
        // yoko
        for (int i = 0; i < TETRIS_WIDTH; ++i) {
          embreeScene.list.emplace_back(std::make_shared<Box>(
            Box(vec3(-(float)TETRIS_WIDTH / 2 + 0.5 + i, -(float)TETRIS_HEIGHT / 2 - 0.5, +0.0f), futiMat, 1.0 - 2.0 * space)
            ));
        }

        auto light = new png::vec3(1.0, 1.0, 1.0);
        embreeScene.sceneLight = png::SceneLight(
          light, 0.7
        );
      }

      void GetEmbreeRenderScene(EmbreeRenderScene& embreeScene) {
        embreeScene.list.erase(embreeScene.list.begin() + 2*(TETRIS_HEIGHT+1) + TETRIS_WIDTH, embreeScene.list.end());
        auto mat = new MaterialReflect(vec3(0.0, 1.0, 0.0), 1.0, 0.0);
        embreeScene.list.emplace_back(std::make_shared<Box>(
          Box(vec3(ElapsedSec(), -4, +0.0f), mat, 1.0)
          ));
      }

      bool isUpdateScene() {
        if (timeUpdated < ElapsedSec()) {
          timeUpdated = ElapsedSec();
          return true;
        }
        return false;
      }

      int ElapsedSec() const {
        return (int)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count();
      }
    private:
      std::array<std::array<bool, TETRIS_WIDTH>, TETRIS_HEIGHT> grid;
      std::chrono::system_clock::time_point startTime;
      int timeUpdated;
    };
  }
}
