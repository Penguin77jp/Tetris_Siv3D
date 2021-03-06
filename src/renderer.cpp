#define NOMINMAX
#include "renderer.h"

#include <vector>
#include <numeric>
#include <random>
#include <numbers>
#include <cmath>
#include <memory>

//namespace {
//  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
//
//  struct Vertex {
//    float x, y, z;
//  };
//
//  struct PolygonIndex {
//    unsigned int v0, v1, v2;
//  };
//}

png::Renderer::Renderer(int width, int height, EmbreeRenderScene& pram_scene)
  : width(width), height(height)
  , scene(pram_scene)
  , renderTarget(RenderTarget(width, height))
  , sampleCounter(0)
  , superSampling(1) {

  deviceHandle = rtcNewDevice(nullptr);
  sceneHandle = rtcNewScene(deviceHandle);
  RTCGeometry geometryHandle = rtcNewGeometry(deviceHandle, RTC_GEOMETRY_TYPE_TRIANGLE);

  std::vector<vec3> vec_geometoryList;
  std::vector<std::vector<unsigned int>> vec_polygonIndex;
  scene.GetVertex(vec_geometoryList, vec_polygonIndex);

  std::vector<Vertex> geometryVertices;
  std::vector<PolygonIndex> geometryPolygons;

  for (int i = 0; i < vec_geometoryList.size(); ++i) {
    geometryVertices.push_back({ vec_geometoryList[i].x,vec_geometoryList[i].y,vec_geometoryList[i].z });
  }

  rtcSetSharedGeometryBuffer(geometryHandle,
    RTC_BUFFER_TYPE_VERTEX,
    0,
    RTC_FORMAT_FLOAT3,
    &geometryVertices[0],
    0,
    sizeof(Vertex),
    geometryVertices.size());




  for (int i = 0; i < vec_polygonIndex.size(); ++i) {
    geometryPolygons.push_back({ vec_polygonIndex[i][0],vec_polygonIndex[i][1],vec_polygonIndex[i][2] });
  }

  rtcSetSharedGeometryBuffer(geometryHandle,
    RTC_BUFFER_TYPE_INDEX,
    0,
    RTC_FORMAT_UINT3,
    &geometryPolygons[0],
    0,
    sizeof(PolygonIndex),
    geometryPolygons.size());

  rtcCommitGeometry(geometryHandle);
  rtcAttachGeometry(sceneHandle, geometryHandle);
  rtcReleaseGeometry(geometryHandle);
  rtcCommitScene(sceneHandle);

  std::cout << "constracter" << std::endl;
  rtcInitIntersectContext(&context);
}

png::Renderer png::Renderer::operator=(const png::Renderer& _renderer) {
  std::cout << "operator=" << std::endl;
  return Renderer(_renderer.width, _renderer.height, _renderer.scene);
}


png::Renderer::~Renderer() {
  std::cout << "deleted" << std::endl;
  rtcReleaseScene(sceneHandle);
  rtcReleaseDevice(deviceHandle);
}

Image& png::Renderer::ResultImage() {
  return renderTarget.ComputeImage(sampleCounter);
}

png::vec3 png::Renderer::PathTracing(RTCRayHit& rayhit, int depth, Random& rnd) {
  constexpr int kDepth = 5; // ????????????????????????????????????????????????????????????
  constexpr int kDepthLimit = 64;

  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return scene.sceneLight.GetColor(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y ,rayhit.ray.dir_z });
  }

  //next
  RTCRayHit newRayhit;
  png::vec3 hitPoint = vec3{ rayhit.ray.org_x,rayhit.ray.org_y,rayhit.ray.org_z } + vec3::Normalize(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y, rayhit.ray.dir_z }) * rayhit.ray.tfar;
  vec3 normal_n = vec3::Normalize(vec3{ rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z });
  vec3 objColor = scene.GetMaterial(rayhit.hit.primID)->GetColor();
  newRayhit.ray.org_x = hitPoint.x;
  newRayhit.ray.org_y = hitPoint.y;
  newRayhit.ray.org_z = hitPoint.z;

  vec3 dir = scene.GetMaterial(rayhit.hit.primID)->GetDirection(
    vec3::Normalize(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y, rayhit.ray.dir_z }),
    normal_n,
    rnd
  );

  newRayhit.ray.dir_x = dir.x;
  newRayhit.ray.dir_y = dir.y;
  newRayhit.ray.dir_z = dir.z;
  newRayhit.ray.tnear = 1.0e-4f;
  newRayhit.ray.tfar = FLOAT_INFINITY;
  newRayhit.ray.flags = false;
  newRayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

  double russian_roulette_probability = std::max(objColor.x, std::max(objColor.y, objColor.z));
  if (depth > kDepthLimit) {
    russian_roulette_probability *= std::pow(0.5, depth - kDepthLimit);
  }
  if (depth > kDepth) {
    if (rnd.next01() >= russian_roulette_probability) {
      return scene.GetMaterial(rayhit.hit.primID)->GetEmission();
    }
  }
  else {
    russian_roulette_probability = 1.0;
  }
  vec3 weight = objColor / russian_roulette_probability;
  vec3 incoming_radiance = PathTracing(newRayhit, depth + 1, rnd);
  return scene.GetMaterial(rayhit.hit.primID)->GetEmission() + weight * incoming_radiance;
}

png::vec3 png::Renderer::PrimalRayTracing(RTCRayHit& rayhit) {
  return png::vec3(0, 0, 0);
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return png::vec3{ };
  }

  return scene.GetMaterial(rayhit.hit.primID)->GetColor() * std::clamp(vec3::Dot(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z), vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z)), 0.0f, 1.0f);
}

png::vec3 png::Renderer::LambertDiffuse(RTCRayHit& rayhit) {
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return scene.sceneLight.GetColor(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y ,rayhit.ray.dir_z });
  }

  return scene.GetMaterial(rayhit.hit.primID)->GetColor() * std::abs(vec3::Dot(vec3::Normalize(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z)), vec3::Normalize(vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z))));
}


void png::Renderer::Draw(const Camera& pram_cam) {
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  const int width = renderTarget.GetWidth();
  const int height = renderTarget.GetHeight();
  const float fovx = pram_cam.fov;
  const float fovy = pram_cam.fov * width / height;

#ifdef _DEBUG
#else
  //#pragma omp parallel for num_threads(8)
#pragma omp parallel for
#endif
  for (int32_t y = 0; y < height; ++y) {
    RTCRayHit rayhit;
    //org
    png::vec3 org = pram_cam.origin;
    rayhit.ray.org_x = org.x;
    rayhit.ray.org_y = org.y;
    rayhit.ray.org_z = org.z;
    rayhit.ray.tnear = 0.0f;
    Random rnd{ (unsigned int)((y + 1) * width * height * sampleCounter) };
    for (int32_t x = 0; x < width; ++x) {
      vec3 l_camX = -pram_cam.l_camX;
      vec3 l_camY = pram_cam.l_camY;
      vec3 l_camZ = pram_cam.l_camZ;
      png::vec3 color;
      for (int s = 0; s < superSampling; ++s) {
        vec3 dir = vec3::Normalize(l_camX * fovx * (2.0f * (x + rnd.next01()) / width - 1.0f) + l_camY * fovy * (2.0f * (y + rnd.next01()) / height - 1.0f) + l_camZ);
        rayhit.ray.dir_x = dir.x;
        rayhit.ray.dir_y = dir.y;
        rayhit.ray.dir_z = dir.z;

        rayhit.ray.tfar = FLOAT_INFINITY;
        rayhit.ray.flags = false;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        const int camType = pram_cam.type;
        if (camType == 0) {
          color += PathTracing(rayhit, 0, rnd) / superSampling;
        }
        else if (camType == 1) {
          color += PrimalRayTracing(rayhit) / superSampling;
        }
        else if (camType == 2) {
          color += LambertDiffuse(rayhit) / superSampling;
        }
      }
      const int indexY = height - y - 1;
      const int index = x * 4 + indexY * width * 4;

      renderTarget.AddImageData(index, color.x);
      renderTarget.AddImageData(index + 1, color.y);
      renderTarget.AddImageData(index + 2, color.z);
    }
  }
  sampleCounter++;
}
