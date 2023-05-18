#include "targa.h"
#include <iostream>
#include <bitset>

namespace Targa {

TgaImage::TgaImage(const char* filepath): 
    filepath_{filepath}, img_buffer_{0}, colorMap_{0},
    colorMapBuffer_{0}, colorIndexBuffer_{0}, handleColorIndexBuffer_{NULL},
    handleColorMapBuffer_{NULL}, handleImageBuffer_{NULL}
{

  // TODO: Error check file opening read bytes 
  std::ifstream file(filepath_, std::ios::in | std::ios::binary);
  filePtr_ = &file;
  if (file.rdstate() == std::ios::failbit){
    std::cout << "no file can be opened\n";
    //TODO - fix these temporary test values.
    //this is setting a default blank buffer
    imageData_ = 3 * (512 * 512);
    imageWidth_ = 512;
    imageHeight_ = 512;
    CreateBuffer();

    return;
  }

  img_buffer_ = header_;
  file.read((char*)img_buffer_, kHeaderLength_);
  if (!LoadImageHeader()){
    LoadImageData();
  }
  file.close();
}

TgaImage::~TgaImage(){
  delete handleColorMapBuffer_;
  delete handleColorIndexBuffer_;
  delete handleImageBuffer_;
}

int TgaImage::CreateBuffer(){
  img_buffer_ = new unsigned char[imageData_];
  handleImageBuffer_ = img_buffer_;
  
  if (img_buffer_)
    return 1;
  return 0;
}

int TgaImage::LoadImageHeader(){
  imageIdLength_ = Get8Bits();
  colorMapType_ = Get8Bits();
  imageType_ = Get8Bits();

  colorMapFirstIndex_ = Get16BitsLe();
  colorMapLength_ = Get16BitsLe();
  colorMapEntrySize_ = Get8Bits();
  
  originX_ = Get16BitsLe();
  originY_ = Get16BitsLe();
  imageWidth_ = Get16BitsLe();
  imageHeight_ = Get16BitsLe();
  pixelDepth_ = Get8Bits();
  imageDescriptor_ = Get8Bits();

  //TODO, extract this out to a function. 
  switch(imageType_){
    case 1:
      colorMap_= new pixel[colorMapLength_];
      if (colorMapEntrySize_/8 > 3) {
        imageData_ = 4 * (imageWidth_ * imageHeight_);
      } else {
        imageData_ = 3 * (imageWidth_ * imageHeight_);
      }
    break;
    case 2:
      imageData_ = (pixelDepth_/8) * (imageWidth_ * imageHeight_);
      break; 
    default:
      std::cout << "error"; 
  }
  return 0;
}

int TgaImage::LoadImageData(){
  CreateBuffer();
    //TODO, extract this out to a function. 
  switch(imageType_){
    case 1:
      colorMapBuffer_ = new unsigned char[colorMapLength_];
      handleColorMapBuffer_ = colorMapBuffer_;  
      filePtr_->read((char*)colorMapBuffer_, colorMapLength_*(colorMapEntrySize_/8)); 
      if (colorMapEntrySize_/8 > 3) {
        for (int i = 0; i < colorMapLength_; i++){
          colorMap_[i].b = Get8Bits(colorMapBuffer_);
          colorMap_[i].g = Get8Bits(colorMapBuffer_);
          colorMap_[i].r = Get8Bits(colorMapBuffer_);
          colorMap_[i].a = Get8Bits(colorMapBuffer_);
        }       
      } else {
        for (int i = 0; i < colorMapLength_; i++){
          colorMap_[i].b = Get8Bits(colorMapBuffer_);
          colorMap_[i].g = Get8Bits(colorMapBuffer_);
          colorMap_[i].r = Get8Bits(colorMapBuffer_);
          colorMap_[i].a = 0;
        }
      }
      colorIndexBuffer_ = new unsigned char[imageWidth_*imageHeight_];
      handleColorIndexBuffer_ = colorIndexBuffer_; 
      filePtr_->read((char*)colorIndexBuffer_, imageWidth_*imageHeight_); 
      break;
    case 2:
      std::cout << "case 2 image\n";
      break; 
    default:
      std::cout << "error";
  }

  //TODO - extract this out into a function
  if (imageType_ == 1) {
    filePtr_->read((char*)img_buffer_, imageData_);
    int pixels = imageWidth_ * imageHeight_;
    int channels = colorMapEntrySize_/8;
    int imgBufferIndex = 0;
    for (int i = 0; i < pixels; i++){
      int mapIndex = Get8Bits(colorIndexBuffer_);
      img_buffer_[imgBufferIndex++] = colorMap_[mapIndex].r;
      img_buffer_[imgBufferIndex++] = colorMap_[mapIndex].g;
      img_buffer_[imgBufferIndex++] = colorMap_[mapIndex].b;
      if (channels > 3)
        img_buffer_[imgBufferIndex++] = colorMap_[mapIndex].a;
    }
  } else {
    if (pixelDepth_/8 == 3)
      FormatRGB();
    else if(pixelDepth_/8 == 4)
      FormatRGBA();
  }
  // file.read((char*)img_buffer_, imageData_);
  return 0;
}

void TgaImage::FormatRGBA(){
  //BGRA RGB
  for (int i = 0; i < imageWidth_*imageHeight_; i++){
    for (int j = 2; j >= 0; j-- ){
      filePtr_->read((char*)img_buffer_+j, 1);
    }
    filePtr_->read((char*)img_buffer_+3, 1);
    img_buffer_ = img_buffer_+4;
  }
  img_buffer_ = img_buffer_-4 * imageWidth_ * imageHeight_;
  return;
}

void TgaImage::FormatRGB(){
  for (int i = 0; i < imageWidth_*imageHeight_; i++){
    for (int i = 2; i >= 0; i-- ){
      filePtr_->read((char*)img_buffer_+i, 1);
    }
    img_buffer_ = img_buffer_+3;
  }
  img_buffer_ = img_buffer_-3 * imageWidth_ * imageHeight_;
  return;
}

unsigned char TgaImage::Get8Bits(){
  unsigned char val = *img_buffer_;
  img_buffer_++;
  return val;
}

unsigned char TgaImage::Get8Bits(unsigned char*& buffer){
  unsigned char val = *buffer;
  buffer++;
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
  for (int i = 1; i <= (imageHeight_ * imageWidth_ * pixelDepth_/8); ++i){
    std::cout << std::bitset<8>(img_buffer_[i-1]) << " " << ((i%6 == 0 && i > 0) ? " \n" : " ");
  }
}

}; //end of namespace Targa
