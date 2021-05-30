#include "ray.h"
#include "gui.h"
#include "random.h"
#include "texture.h"
#include "material.h"
#include "renderer.h"
#include "renderTarget.h"
#include "gameScene.h"
#include "mino.h"
#include "controller.h"

#include <iostream>

/* befo

void Init_NiceScene(png::EmbreeRenderScene& scene) {
  using namespace png;
  scene.list.emplace_back(std::make_shared<Box>(Box{ vec3{+0.0f,+0.0f,+5.0f},new MaterialReflect(vec3{1.0f,0.1f,0.1f},0.7f,0.0f) }));
  scene.list.emplace_back(std::make_shared<Box>(Box{ vec3{+5.0f,+0.0f,+0.0f},new MaterialReflect(vec3{0.1f,1.0f,0.1f},0.7f,0.0f) }));
  scene.list.emplace_back(std::make_shared<Box>(Box{ vec3{-5.0f,+0.0f,+0.0f},new MaterialReflect(vec3{0.1f,0.1f,1.0f},0.7f,0.0f) }));
  scene.list.emplace_back(std::make_shared<Box>(Box{ vec3{+0.0f,+0.0f,-5.0f},new MaterialReflect(vec3{1.0f,1.0f,0.1f},0.7f,0.0f) }));

  //box
  {
	png::Material* tmp_mat = new png::MaterialReflect(png::vec3{ 1.0f,1.0f,1.0f }, 0.05f, 0.0f);
	scene.list.emplace_back(std::make_shared<Box>(
	  Box{ png::vec3{+00.0f,-25,+00.0f}, tmp_mat ,25.0f }));
  }

  scene.sceneLight = png::SceneLight(
	new png::Texture("private src/601265265.835475.jpg")
	//new png::Texture("private src/601265293.010277.jpg")
  );
}

void TestScene(png::EmbreeRenderScene& scene) {
  using namespace png;
  if (true) {
	scene.list.emplace_back(std::make_shared<Triangle>(Triangle{
	  vec3{-1.0f,+0.0f,+0.0f},
	  vec3{+1.0f,+0.0f,+0.0f},
	  vec3{+0.0f,+2.0f,+0.0f},
	  new MaterialReflect(vec3{1.0f,0.1f,0.1f},0.7f,0.0f) }));
  }
  if (true) {
	scene.list.emplace_back(std::make_shared<Triangle>(Triangle{
	  vec3{-1.0f,-1.0f,+0.0f},
	  vec3{+1.0f,-1.0f,+0.0f},
	  vec3{+0.0f,-2.0f,+0.0f},
	  new MaterialReflect(vec3{0.2f,1.0f,0.2f},0.7f,0.0f) }));
  }

  auto light = new png::vec3(1.0, 1.0, 1.0);
  scene.sceneLight = png::SceneLight(
	light, 1.0
	//new png::Texture("private src/601265265.83547A5.jpg")
	//new png::Texture("private src/601265293.010277.jpg")
  );
}

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr float TEXTURE_SIZE = 0.5f;
constexpr int TEXTURE_WIDTH = WINDOW_WIDTH * TEXTURE_SIZE;
constexpr int TEXTURE_HEIGHT = WINDOW_HEIGHT * TEXTURE_SIZE;


int main(int, char**) {

  //game scene
  namespace pScene = png::scene;
  png::EmbreeRenderScene scene;
  //TestScene(scene);
  //Init_NiceScene(scene);
  pScene::StaticScene  staticScene(
	png::Camera{
	  0,
	  png::vec3{-2.6f,+0.5f,+2.6f},
	  png::vec3{+0.0f,+0.0f,+0.0f},
	  0.726,
	  png::vec3{+0.0f,+1.0f,+0.0f},
	},
	scene);

  pScene::GameScene gameScene(
	png::Camera{
	  0,
	  png::vec3{+0.0f,+2.0f,-20.0f},
	  png::vec3{+0.0f,+0.0f,+0.0f},
	  0.5,
	  png::vec3{+0.0f,+1.0f,+0.0f},
	}
  );
  gameScene.InitEmbreeRenderScene(scene);

  auto renderTarget = png::RenderTarget(TEXTURE_WIDTH, TEXTURE_HEIGHT);
  auto renderer = new png::Renderer(scene);
  //png::RenderData renderData(TEXTURE_WIDTH, TEXTURE_HEIGHT, scene); <- いらなそう


  //png::GUI gui(WINDOW_WIDTH, WINDOW_HEIGHT);

  while (true) {
	if (gameScene.isUpdateScene()) {
	  gameScene.GetEmbreeRenderScene(scene);
	  delete renderer;
	  renderer = new png::Renderer(scene);
	  renderTarget.Init();
	}
	renderer->Draw(renderTarget,gameScene.GetCamera());
	renderTarget.Update();
	//gui.Update(renderTarget, (pScene::BaseScene*)&gameScene);
  }

  return 0;
}
*/


# include <Siv3D.hpp> // OpenSiv3D v0.4.3

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr float TEXTURE_SIZE = 0.25f;
constexpr int TEXTURE_WIDTH = WINDOW_WIDTH * TEXTURE_SIZE;
constexpr int TEXTURE_HEIGHT = WINDOW_HEIGHT * TEXTURE_SIZE;

void Main()
{
	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));
	Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// 大きさ 60 のフォントを用意
	const Font font(60);

	//game scene
	namespace pScene = png::scene;
	png::EmbreeRenderScene scene;
	//TestScene(scene);
	//Init_NiceScene(scene);
	pScene::GameScene gameScene(
		TEXTURE_WIDTH, TEXTURE_HEIGHT,
		png::Camera{
		  0,
		  png::vec3{+0.0f,+2.0f,-20.0f},
		  png::vec3{+0.0f,+0.0f,+0.0f},
		  0.5,
		  png::vec3{+0.0f,+1.0f,+0.0f},
		},
		1,
		png::tetris::Controller()
		);
	gameScene.InitEmbreeRenderScene(scene);

	//auto renderTarget = png::RenderTarget(TEXTURE_WIDTH, TEXTURE_HEIGHT);
	auto renderer = new png::Renderer(TEXTURE_WIDTH, TEXTURE_HEIGHT, scene);

	DynamicTexture texture(renderer->ResultImage());

	while (System::Update())
	{
		// テキストを画面の中心に描く
		font(U"Hello, Siv3D!🐣{}"_fmt(1.0f / Scene::DeltaTime())).drawAt(Scene::Center(), Palette::Black);

		// マウスカーソルに追従する半透明の赤い円を描く
		Circle(Cursor::Pos(), 40).draw(ColorF(1, 0, 0, 0.5));

		gameScene.GetEmbreeRenderScene(scene);
		//delete renderer;
		//renderer = new png::Renderer(TEXTURE_WIDTH, TEXTURE_HEIGHT, scene);

	  //renderer->Draw(gameScene.GetCamera());
	  //texture.fill(renderer->ResultImage());
	  //texture.scaled(1.0f/TEXTURE_SIZE).draw();

	  //tetris
		auto box = png::tetris::TetrisPosition2vec3(gameScene.GetBox());
		float scale = 40;
		float size = scale - 20;
		for (int i = 0; i < box.size(); ++i) {
			Rect(
				(box[i].x - png::tetris::TETRIS_WIDTH / 2) * scale + WINDOW_WIDTH / 2
				, -(box[i].y - png::tetris::TETRIS_HEIGHT / 2) * scale + WINDOW_HEIGHT / 2
				, size
				, size
			).draw();
		}
		if (gameScene.isUpdateScene()) {
			gameScene.UpdateDeltaTime();
		}
	}
}

//
// = アドバイス =
// Debug ビルドではプログラムの最適化がオフになります。
// 実行速度が遅いと感じた場合は Release ビルドを試しましょう。
// アプリをリリースするときにも、Release ビルドにするのを忘れないように！
//
// 思ったように動作しない場合は「デバッグの開始」でプログラムを実行すると、
// 出力ウィンドウに詳細なログが表示されるので、エラーの原因を見つけやすくなります。
//
// = お役立ちリンク =
//
// OpenSiv3D リファレンス
// https://siv3d.github.io/ja-jp/
//
// チュートリアル
// https://siv3d.github.io/ja-jp/tutorial/basic/
//
// よくある間違い
// https://siv3d.github.io/ja-jp/articles/mistakes/
//
// サポートについて
// https://siv3d.github.io/ja-jp/support/support/
//
// Siv3D ユーザコミュニティ Slack への参加
// https://siv3d.github.io/ja-jp/community/community/
//
// 新機能の提案やバグの報告
// https://github.com/Siv3D/OpenSiv3D/issues
//

