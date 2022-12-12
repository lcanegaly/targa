#include <cstddef>
#include <fstream>

class Targa{
 public:
  Targa(const char* filepath);
  ~Targa();

  const unsigned char* data() {return img_buffer_;}
  size_t size() {return imageData_;}
  int width() {return imageWidth_;}
  int height() {return imageHeight_;}
  void Print();

 private:
  int LoadImageHeader();
  int LoadImageData(std::ifstream& file);
  unsigned char Get8Bits();
  int Get16BitsLe(); 

 private:
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
