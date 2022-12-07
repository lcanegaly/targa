#include <cstddef>
#include <fstream>


class Targa{
 public:
  Targa(const char* filepath);
  ~Targa();

  const char* data() {return data_;}
  size_t size() {return imageData_;}
  int width() {return imageWidth_;}
  int height() {return imageHeight_;}
  void Print();

 private:
  int LoadImageHeader();
  int LoadImageData(std::ifstream& file);


 private:
  const char* filepath_;
  const char* file_;
  char* header_;
  char* data_;
  
  //consts for the header structer
  static constexpr size_t kHeaderLength_ = 18;
  static constexpr int kIdLengthOffset_ = 0;
  static constexpr int kColorMapTypeOffset_ = 1; 
  static constexpr int kImageTypeOffset_ = 2;
  static constexpr int kColorMapSpecificationOffset_ = 3;
  static constexpr int kImageSpecificationOffset_ = 8;

  size_t imageIdLength_;
  int colorMapType_;
  size_t colorMapData_;
  int colorMapSpecification_;
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
