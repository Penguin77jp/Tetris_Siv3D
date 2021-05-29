#pragma once

#include "gameObject.h"
#include "sceneData.h"
#include "camera.h"
#include "mino.h"
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
    using Field = std::array<std::array<bool, TETRIS_WIDTH>, TETRIS_HEIGHT>;
    class GameScene : public BaseScene {
    public:
      GameScene(const Camera& _cam)
        : startTime(std::chrono::system_clock::now())
        , timeUpdated(0)
        , BaseScene(_cam) {
        for (int y = 0; y < TETRIS_HEIGHT; ++y) {
          for (int x = 0; x < TETRIS_WIDTH; ++x) {
            grid[y][x] = false;
          }
        }
      }

      void InitEmbreeRenderSceneVector(std::vector<png::vec3>& vec, float& size) {
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

      void InitEmbreeRenderScene(EmbreeRenderScene& embreeScene) {
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

      void inline AddSceneGameObject(EmbreeRenderScene& embreeScene, vec3 vec, Material* mat, float size) {
        embreeScene.list.emplace_back(std::make_shared<Box>(
          Box(vec, mat, size)
          ));
      }

      void GetEmbreeRenderScene(EmbreeRenderScene& embreeScene) {
        embreeScene.list.erase(embreeScene.list.begin() + 2 * (TETRIS_HEIGHT + 1) + TETRIS_WIDTH, embreeScene.list.end());
        auto mat = new MaterialReflect(vec3(0.0, 1.0, 0.0), 1.0, 0.0);
        AddSceneGameObject(embreeScene, vec3(ElapsedSec(1), -4, +0.0f), mat, 1.0);
      }

      std::vector<png::vec3> GetBox() {
        std::vector<png::vec3> vec;
        if (isUpdateScene(5)) {
          if (stashMino.size() == 0) {
            for (int i = 0; i < 7; ++i) {
              stashMino.push_back(i);
            }
          }
          stashMino.erase(stashMino.begin());
          handlingMino = new tetris::Mino(0);
        }

        //futi
        //tate
        for (int i = 0; i <= TETRIS_HEIGHT; ++i) {
          vec.push_back(vec3(-(float)TETRIS_WIDTH / 2 - 0.5, (float)TETRIS_HEIGHT / 2 - 0.5 - i, +0.0f));
          vec.push_back(vec3(+(float)TETRIS_WIDTH / 2 - 0.5, (float)TETRIS_HEIGHT / 2 - 0.5 - i, +0.0f));
        }
        // yoko
        for (int i = 0; i < TETRIS_WIDTH; ++i) {
          vec.push_back(vec3(-(float)TETRIS_WIDTH / 2 + 0.5 + i, -(float)TETRIS_HEIGHT / 2 - 0.5, +0.0f));
        }
        return vec;
      }

      bool isUpdateScene(int division) {
        if (timeUpdated < ElapsedSec(division)) {
          timeUpdated = ElapsedSec(division);
          return true;
        }
        return false;
      }

      int ElapsedSec(int division) const {
        auto time = (int)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count();
        return time / division;
      }
    private:
      Field grid;
      std::chrono::system_clock::time_point startTime;
      int timeUpdated;
      std::vector<unsigned int> stashMino;
      tetris::Mino* handlingMino;
    };
  }
}
