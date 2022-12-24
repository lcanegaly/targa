#pragma once
#include <cstddef>
#include <fstream>

namespace Targa
{

class Image{
 public:
  virtual const unsigned char* data() = 0;
  virtual size_t size() = 0;
  virtual int width() = 0;
  virtual int height() = 0;
  virtual void Print() = 0;
};


class TgaImage : public Image{
 public:
  TgaImage(const char* filepath);
  ~TgaImage();

  const unsigned char* data() override {return img_buffer_;}
  size_t size() override {return imageData_;}
  int width() override {return imageWidth_;}
  int height() override {return imageHeight_;}
  void Print() override;

 private:
  int LoadImageHeader();
  int LoadImageData();
  unsigned char Get8Bits();
  int Get16BitsLe(); 
  void FormatRGB(unsigned char*);
  
 private:
  std::ifstream* filePtr_;
  const char* filepath_;
  const char* file_;
  static constexpr size_t kHeaderLength_ = 18;
  unsigned char header_[kHeaderLength_];
  unsigned char* img_buffer_;

  size_t imageIdLength_;
  int colorMapType_;
  size_t colorMapData_;
  int imageType_;
  size_t imageData_;
  int dataOffset_;

  int originX_;
  int originY_;
  int imageWidth_;
  int imageHeight_;
  int pixelDepth_;
  int imageDescriptor_;
};

} //end namespace Targa
