#include "targa.h"
#include <iostream>
#include <bitset>

namespace Targa {

TgaImage::TgaImage(const char* filepath): 
    filepath_{filepath}
{
  // TODO: Error check file opening read bytes 
  std::ifstream file(filepath_, std::ios::in | std::ios::binary);
  filePtr_ = &file;

  img_buffer_ = header_;
  file.read((char*)img_buffer_, kHeaderLength_);
  if (!LoadImageHeader()){
    LoadImageData();
  }
  file.close();
}

TgaImage::~TgaImage(){
  delete img_buffer_;
}

int TgaImage::LoadImageHeader(){
  imageIdLength_ = Get8Bits();
  colorMapType_ = Get8Bits();
  imageType_ = Get8Bits();

  //TODO handle color maps, for now we skip these 5 bytes.
  if (colorMapType_ > 0)
    return 1;
  colorMapData_ = 0;
  img_buffer_+=5;
  
  originX_ = Get16BitsLe();
  originY_ = Get16BitsLe();
  imageWidth_ = Get16BitsLe();
  imageHeight_ = Get16BitsLe();
  pixelDepth_ = Get8Bits();
  imageDescriptor_ = Get8Bits();
  
  imageData_ = 3 * (imageWidth_ * imageHeight_);
  
  return 0;
}

int TgaImage::LoadImageData(){
  dataOffset_ = kHeaderLength_ + imageIdLength_;  
  img_buffer_ = new unsigned char[imageData_];

  unsigned char pixelBuffer[3];
  for (int i = 0; i < imageWidth_*imageHeight_; i++){
    FormatRGB(pixelBuffer);
    img_buffer_ = img_buffer_+3;
  }
  
  img_buffer_ = img_buffer_-3 * imageWidth_ * imageHeight_;
  
  // file.read((char*)img_buffer_, imageData_);
  return 0;
}

void TgaImage::FormatRGB(unsigned char* pixel_buffer){
 
  for (int i = 2; i >= 0; i-- ){
    filePtr_->read((char*)img_buffer_+i, 1);
  }
  //for (int i = 0; i < 3; i++)
  //  std::cout << std::bitset<8>(img_buffer_[i]) << " ";
  //std::cout << "\n";

  return;
}

unsigned char TgaImage::Get8Bits(){
  unsigned char val = *img_buffer_;
  img_buffer_++;
  return val;
}

int TgaImage::Get16BitsLe(){
  unsigned char b = Get8Bits();
  return (int)b + (int)(Get8Bits() << 8); 
}

void TgaImage::Print(){
  std::cout << "-id length: " << imageIdLength_ << "\n-colorMapType: " << colorMapType_ << "\n-Image type: " << imageType_
    << "\n-X origin: " << originX_ << "\n-Y origin: " << originY_ << "\n-Width: " << imageWidth_ << "\n-Height: " << imageHeight_
    << "\n-Pixel Depth: " << pixelDepth_ << "\n-Img Desc: " << imageDescriptor_ << "\n";

  std::cout << "--- HEADER DATA ---\n";
  for (int i = 1; i <= kHeaderLength_; i++)
    std::cout << std::bitset<8>(header_[i-1]) << ((i%6 == 0 && i > 0) ? " \n" : " ");

  std::cout << "--- PIXEL DATA ---\n";
  for (int i = 1; i <= (imageHeight_ * imageWidth_ * 3); ++i){
    std::cout << std::bitset<8>(img_buffer_[i-1]) << " " << ((i%6 == 0 && i > 0) ? " \n" : " ");
  }
}

}; //end of namespace Targa
