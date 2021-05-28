#pragma once

#include "ray.h"
#include "random.h"
#include "sceneData.h"
#include "renderTarget.h"
#include "camera.h"

#include "embree3/rtcore.h"

namespace png {
  class Renderer {
  public:
    Renderer(int width, int height, EmbreeRenderScene& pram_scene);
    Renderer operator=(const Renderer& _renderer);
    ~Renderer();
    void Draw(const Camera& pram_cam);
    Image& ResultImage();

    int GetWidth()const { return width; }
    int GetHeight() const { return height; }

  private:
    int width, height;
    EmbreeRenderScene& scene;
    RTCScene sceneHandle;
    RTCDevice deviceHandle;
    RTCIntersectContext context;
    vec3 PathTracing(RTCRayHit& rayhit, int depth, Random& rnd);
    vec3 PrimalRayTracing(RTCRayHit& rayhit);
    vec3 LambertDiffuse(RTCRayHit& rayhit);
    RenderTarget renderTarget;
    int sampleCounter;
    int superSampling;
  };
}
