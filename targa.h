#pragma once
#include <cstddef>
#include <fstream>
#include <iostream>

namespace Targa
{

struct pixel{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

class Image{
 public:
  virtual const unsigned char* data() = 0;
  virtual ~Image() = default;
  virtual size_t size() = 0;
  virtual int width() = 0;
  virtual int height() = 0;
  virtual void Print() = 0;
};

class TgaImage : public Image{
 public:
  TgaImage(const char* filepath);
  ~TgaImage();
  TgaImage(const TgaImage&) {
    std::cout << "COPY CONSTRUCTOR \n";
  }

  const unsigned char* data() override {return img_buffer_;}
  size_t size() override {return imageData_;}
  int width() override {return imageWidth_;}
  int height() override {return imageHeight_;}
  void Print() override;

 private:
  int LoadImageHeader();
  int LoadImageData();
  unsigned char Get8Bits();
  unsigned char Get8Bits(unsigned char*& buffer);
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
  int imageType_;
  size_t imageData_;
  int dataOffset_;
  
  int colorMapFirstIndex_;
  int colorMapLength_; 
  int colorMapEntrySize_;
  pixel* colorMap_;
  unsigned char* colorMapBuffer_;
  unsigned char* colorIndexBuffer_;

  //Handles to allocated buffers. 
  //We incremenet the buffer pointers, so we need these to call delete on the original address.
  unsigned char* handleColorMapBuffer_;
  unsigned char* handleColorIndexBuffer_;
  unsigned char* handleImageBuffer_;

  int originX_;
  int originY_;
  int imageWidth_;
  int imageHeight_;
  int pixelDepth_;
  int imageDescriptor_;
};

} //end namespace Targa
