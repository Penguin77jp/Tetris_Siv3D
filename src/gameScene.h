#pragma once

#include "sceneData.h"
#include "camera.h"
#include "controller.h"

#include <vector>
#include <array>
#include <chrono>


namespace png {
  namespace tetris {
    class Mino;
    struct TetrisPosition;
    constexpr unsigned int TETRIS_WIDTH = 10;
    constexpr unsigned int TETRIS_HEIGHT = 20;
    //Field[ y ][ x ]
    using Field = std::array<std::array<bool, (TETRIS_WIDTH + 2)>, (TETRIS_HEIGHT + 1)>;
  }
  namespace scene {
    class BaseScene {
    public:
      BaseScene(const Camera& _cam);
      Camera GetCamera() const;
      virtual void GetEmbreeRenderScene(EmbreeRenderScene&) = 0;
      void SetInputBuffer(int _input);
      void SetInputBuffer(std::vector<int>& _inputBuffer);

    private:
      Camera cam;
      std::vector<int> inputBuffer;
    };

    class StaticScene {
    public:
      StaticScene(const Camera& _cam,
        EmbreeRenderScene _embreeObjList);
      StaticScene(const Camera& _cam);

      Camera GetCamera() const;
      void GetEmbreeRenderScene(EmbreeRenderScene& _embreeScene);

    private:
      EmbreeRenderScene  embreeScene;
      Camera cam;
    };

    class GameScene : public BaseScene {
    public:
      GameScene(const Camera& _cam, float _updatingTimeFrequency, tetris::Controller _cont);

      void InitEmbreeRenderSceneVector(std::vector<png::vec3>& vec, float& size);

      void InitEmbreeRenderScene(EmbreeRenderScene& embreeScene);

      void AddSceneGameObject(EmbreeRenderScene& embreeScene, vec3 vec, Material* mat, float size);

      void GetEmbreeRenderScene(EmbreeRenderScene& embreeScene);

      std::vector<tetris::TetrisPosition> GetBox();

      bool isUpdateScene();

      int ElapsedSec() const;
    private:
      float updatingTimeFrequency;
      tetris::Field grid;
      std::chrono::system_clock::time_point deltaTime;
      std::vector<unsigned int> stashMino;
      tetris::Mino* handlingMino;
      tetris::Controller controller;
    };
  }
}
