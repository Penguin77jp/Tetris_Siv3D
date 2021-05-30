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
    int BaseScene::GetTextureWidth() const { return texture_w; }
    int BaseScene::GetTextureHeight() const { return texture_h; }




    StaticScene::StaticScene(
      int texture_w,
      int texture_h,
      const Camera& _cam)
      : BaseScene(texture_w, texture_h, _cam) {}

    Image StaticScene::ComputeResultImage() const {
      return Image(1);
    }



    GameScene::GameScene(int texture_w, int texture_h, const Camera& _cam, float _updatingTimeFrequency, tetris::Controller _cont)
      : updatingTimeFrequency(_updatingTimeFrequency)
      , updateFuncTime(std::chrono::system_clock::now())
      , deltaTime(std::chrono::system_clock::now())
      , BaseScene(texture_w, texture_h, _cam)
      , handlingMino(NULL)
      , controller(_cont) {
      for (int y = 0; y < TETRIS_HEIGHT + 1; ++y) {
        for (int x = 0; x < TETRIS_WIDTH + 2; ++x) {
          if (x == 0 || x == TETRIS_WIDTH + 1 || (y == 0 && x != 0 && x != TETRIS_WIDTH + 1)) {
            grid[y][x] = new MaterialReflect(vec3(1.0f, 0.2f, 0.2f), 0.8f, 0);
            embreeRenderScene.list.emplace_back(std::make_shared<Box>(
              Box(vec3(-(float)TETRIS_WIDTH / 2, -(float)TETRIS_HEIGHT, 0), grid[y][x], 1.0f)
              ));
          }
          else {
            grid[y][x] = NULL;
          }
        }
      }
      auto light = new png::vec3(1.0, 1.0, 1.0);
      embreeRenderScene.sceneLight = new png::SceneLight(
        //new s3d::Image(Emoji(U"🐈"))
        new Texture("R0010057_20210515162730.JPG")
        //light, 0.2
      );

      renderer = new Renderer(texture_w, texture_h, embreeRenderScene);
    }

    void GameScene::UpdateEmbreeRenderSceneByGrid(EmbreeRenderScene& in_embreeRenderScene) {
      in_embreeRenderScene.list.erase(in_embreeRenderScene.list.begin()
        , in_embreeRenderScene.list.end());

      //grid
      for (int y = 0; y < TETRIS_HEIGHT + 1; ++y) {
        for (int x = 0; x < TETRIS_WIDTH + 2; ++x) {
          if (grid[y][x] != NULL) {
            embreeRenderScene.list.emplace_back(std::make_shared<Box>(
              Box(vec3(-(float)TETRIS_WIDTH / 2 + x, -(float)TETRIS_HEIGHT + y, 0), grid[y][x], 0.5f)
              ));
          }
        }
      }
      // handlingMino
      {
        if (handlingMino != NULL) {
          auto tmpPosi = handlingMino->GetPosi();
          for (int i = 0; i < tmpPosi.size(); ++i) {
            embreeRenderScene.list.emplace_back(std::make_shared<Box>(
              Box(
                vec3(-(float)TETRIS_WIDTH / 2 + tmpPosi[i].x,
                  -(float)TETRIS_HEIGHT + tmpPosi[i].y,
                  0),
                handlingMino->GetMaterial(),
                0.5f)
              ));
          }
        }
      }

      //light
      embreeRenderScene.list.emplace_back(
        std::make_shared<Box>(
          Box(
            vec3(0,
              0,
              50),
            new MaterialReflect(vec3(0.2f, 1.0f, 0.2f), 0.0f, 1.0f),
            10.0f)
          )
      );
    }


    void GameScene::AddSceneGameObject(EmbreeRenderScene& embreeScene, vec3 vec, Material* mat, float size) {
      embreeScene.list.emplace_back(std::make_shared<Box>(
        Box(vec, mat, size)
        ));
    }

    Image GameScene::ComputeResultImage() const {
      renderer->Draw(GetCamera());
      return renderer->ResultImage();
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
              //grid[tmpPosi[i].y][tmpPosi[i].x] = true;
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
                      grid[y][x] = NULL;
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

    void GameScene::UpdateUpdateFuncTime() {
      updateFuncTime = std::chrono::system_clock::now();
    }

    void GameScene::UpdateDeltaTime() {
      deltaTime = std::chrono::system_clock::now();
    }

    void GameScene::Update() {
      if (handlingMino == NULL || controller.InputKeyDown() == 1 || isUpdateScene()) {
        {
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
                  grid[tmpPosi[i].y][tmpPosi[i].x] = handlingMino->GetMaterial();
                }
                /*auto hoge = new MaterialReflect(vec3(0,0,0),0,0);
                for (int i = 1; i < TETRIS_WIDTH + 2; ++i) {
                  grid[1][i] = hoge;
                }*/

                //delete line
                for (int y = 1; y < grid.size();) {
                  int cell = 0;
                  for (int x = 0; x < grid[0].size(); ++x) {
                    if (grid[y][x] != NULL) cell++;
                  }
                  if (cell == grid[0].size()) {
                    for (int x = 0; x < grid[0].size(); ++x) {
                      if (grid[y][x] != NULL) {
                        grid[y][x] = NULL;
                      }
                      if (y == grid.size() - 1) {
                        grid[y][x] == NULL;
                      }
                      else {
                        grid[y][x] = grid[y + 1][x];
                      }
                    }
                  }
                  else y++;
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

          UpdateEmbreeRenderSceneByGrid(embreeRenderScene);
        }
        delete renderer;
        renderer = new Renderer(GetTextureWidth(), GetTextureHeight(), embreeRenderScene);
      }
    }

    int GameScene::ElapsedSec() const {
      auto time = (int)std::chrono::duration_cast<std::chrono::seconds>(updateFuncTime - deltaTime).count();
      return time;
    }
  }
}
