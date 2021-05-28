#pragma once

#include <Siv3D.hpp>
#include <vector>

namespace png {
  class RenderTarget {
  public:
    RenderTarget(const uint16_t w, const uint16_t h);
    void AddImageData(int index, float data);
    void SetImageData(int index, float data);
    void Init();
    void WriteImage(const char* fileName);
    Image& ComputeImage(int division);

    //getter
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

  private:
    uint16_t width, height;
    Image image;
    std::vector<float> image_dataF;
    std::vector<unsigned char> image_data;
  };
}
