#include "renderTarget.h"

//#include <stb_image_write.h>
#include <cmath>
#include <iostream>

png::RenderTarget::RenderTarget(const uint16_t w, const uint16_t h)
  : width(w), height(h),
  image(width, height),
  image_dataF(4 * width * height, 0.0f),
  image_data(image_dataF.size(), 0) {
}

Image& png::RenderTarget::ComputeImage(int division) {
  for (int y = 0; y < image.height(); ++y) {
    for (int x = 0; x < image.width(); ++x) {
      float r = image_dataF[y * width * 4 + x * 4];
      float g = image_dataF[y * width * 4 + x * 4 + 1];
      float b = image_dataF[y * width * 4 + x * 4 + 2];
      image[y][x] = ColorF(r / division, g / division, b / division);
    }
  }
  return image.gammaCorrect(2.2f);
}

/*
void png::RenderTarget::Update(Image& _image) {
#ifdef _RELEASE
#pragma omp parallel for
#endif
  for (int i = 0; i < image_dataF.size(); ++i) {
    float val = image_dataF[i];
    if (val > 1.0f) {
      val = 1.0f;
    }
    _image[i][i] = ColorF();
    //image_data[i] = (unsigned char)(255 * std::pow(val, 1.0f / 2.2f));
  }

}
*/

void png::RenderTarget::AddImageData(int index, float data) {
  image_dataF[index] += data;
}
void png::RenderTarget::SetImageData(int index, float data) {
  image_dataF[index] = data;
}

void png::RenderTarget::Init() {
  image_data.resize(4 * width * height);
  image_dataF.resize(image_data.size());
  for (int i = 0; i < image_data.size(); ++i) {
    image_data[i] = image_dataF[i] = 0;
  }
}

void png::RenderTarget::WriteImage(const char* fileName) {
  std::vector<unsigned char> tmp(4 * width * height, 0);
  for (int i = 0; i < width * height * 4; ++i) {
    //float val = std::powf(image_dataF[i] / sampleCounter, 1.0 / 2.2f);
    //if (val > 1) {
    //  val = 1.0f;
    //}
    //tmp[i] = (unsigned char)(255 * val);
  }
  //stbi_write_png(fileName, width, height, 4, &tmp[0], 0);
  std::cout << "saved " << fileName << std::endl;
}
