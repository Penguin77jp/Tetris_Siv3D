#pragma once

#include "sceneData.h"
#include "camera.h"
#include "controller.h"
#include "renderer.h"

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
		using Field = std::array<std::array<Material*, (TETRIS_WIDTH + 2)>, (TETRIS_HEIGHT + 1)>;
	}
	namespace scene {
		class BaseScene {
		public:
			BaseScene(int texture_w, int texture_h, const Camera& _cam);
			Camera GetCamera() const;
			void SetInputBuffer(int _input);
			void SetInputBuffer(std::vector<int>& _inputBuffer);
      int GetTextureWidth() const;
      int GetTextureHeight() const;

    protected:
      Renderer* renderer;
      EmbreeRenderScene embreeRenderScene;

		private:
			Camera cam;
			std::vector<int> inputBuffer;
			int texture_w, texture_h;
		};

		class StaticScene : public BaseScene{
		public:
			StaticScene(int texture_w,
				int texture_h,
				const Camera& _cam);

			Image ComputeResultImage() const;

		private:
		};

		class GameScene : public BaseScene {
		public:
			GameScene(int texture_w, int texture_h, const Camera& _cam, float _updatingTimeFrequency, tetris::Controller _cont);

			void AddSceneGameObject(EmbreeRenderScene& embreeScene, vec3 vec, Material* mat, float size);

			Image ComputeResultImage() const;

			std::vector<tetris::TetrisPosition> GetBox();

      void UpdateEmbreeRenderSceneByGrid(EmbreeRenderScene& _);

			bool isUpdateScene();

			void Update();

      void UpdateUpdateFuncTime();
			void UpdateDeltaTime();

			int ElapsedSec() const;
		private:
			float updatingTimeFrequency;
			tetris::Field grid;
      std::chrono::system_clock::time_point updateFuncTime;
      std::chrono::system_clock::time_point deltaTime;
			std::vector<unsigned int> stashMino;
			tetris::Mino* handlingMino;
			tetris::Controller controller;
		};
	}
}
