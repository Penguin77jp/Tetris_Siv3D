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
		using Field = std::array<std::array<bool, (TETRIS_WIDTH + 2)>, (TETRIS_HEIGHT + 1)>;
	}
	namespace scene {
		class BaseScene {
		public:
			BaseScene(int texture_w, int texture_h, const Camera& _cam);
			Camera GetCamera() const;
			virtual void GetEmbreeRenderScene() = 0;
			void SetInputBuffer(int _input);
			void SetInputBuffer(std::vector<int>& _inputBuffer);
			EmbreeRenderScene& GetEmbreeRenderScene();
			virtual Image ComputeResultImage() const = 0;

		private:
			Camera cam;
			std::vector<int> inputBuffer;
			Renderer* renderer;
			EmbreeRenderScene embreeRenderScene;
			int texture_w, texture_h;
		};

		class StaticScene : public BaseScene{
		public:
			StaticScene(int texture_w,
				int texture_h,
				const Camera& _cam,
				EmbreeRenderScene _embreeObjList);

			void GetEmbreeRenderScene(EmbreeRenderScene& _embreeScene);
			Image ComputeResultImage() const;

		private:
		};

		class GameScene : public BaseScene {
		public:
			GameScene(int texture_w, int texture_h, const Camera& _cam, float _updatingTimeFrequency, tetris::Controller _cont);

			void InitEmbreeRenderSceneVector(std::vector<png::vec3>& vec, float& size);

			void InitEmbreeRenderScene(EmbreeRenderScene& embreeScene);

			void AddSceneGameObject(EmbreeRenderScene& embreeScene, vec3 vec, Material* mat, float size);

			void GetEmbreeRenderScene(EmbreeRenderScene& embreeScene);

			Image ComputeResultImage() const;

			std::vector<tetris::TetrisPosition> GetBox();

			bool isUpdateScene();

			void Update();

			void UpdateDeltaTime();

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
