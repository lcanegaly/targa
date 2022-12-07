#include "targa.h"
#include <iostream>
#include <bitset>

Targa::Targa(const char* filepath): filepath_{filepath}{
   // TODO: Error check file opening read bytes 
  std::ifstream file(filepath_, std::ios::in | std::ios::binary);
  header_ = (char*)malloc(kHeaderLength_);
  file.read(header_, kHeaderLength_);
 if (!LoadImageHeader()){
    LoadImageData(file);
  }
}

Targa::~Targa(){
  delete header_;
  delete data_;
}

int Targa::LoadImageHeader(){
  imageIdLength_ = (unsigned char)header_[kIdLengthOffset_] * sizeof(char);
  colorMapType_ = (unsigned char)header_[kColorMapTypeOffset_];
  imageType_ = (unsigned char)header_[kImageTypeOffset_];
  //TODO check three subfields for color map spec 
  originX_ = (unsigned char)header_[kImageSpecificationOffset_];
  //TODO use constants to set subfield locations
  //TODO LSB ordering
  originY_ = (unsigned char)header_[kImageSpecificationOffset_ + 2]; 
  imageWidth_ = (unsigned char)header_[kImageSpecificationOffset_ + 4];
  imageHeight_ = (unsigned char)header_[kImageSpecificationOffset_ + 6];
  pixelDepth_ = (unsigned char)header_[kImageSpecificationOffset_ + 8];
  imageDescriptor_ = (unsigned char)header_[kImageSpecificationOffset_ + 9];
  imageData_ = 3 * (imageWidth_ * imageHeight_);

  //TODO - Add processing for color map.
  if (colorMapType_ > 0)
    return 1;
  colorMapData_ = 0;

  return 0;
}

int Targa::LoadImageData(std::ifstream& file){
  dataOffset_ = kHeaderLength_ + imageIdLength_ + colorMapData_; 
  data_ = (char*)malloc(imageData_);
  file.read(data_, imageData_);

  return 0;
}

void Targa::Print(){
  std::cout << imageData_ << "id length: " << imageIdLength_ << "\n colorMapType: " << colorMapType_ << "\n Image type: " << imageType_
    << "\n X origin: " << originX_ << "\n Y origin: " << originY_ << "\n Width: " << imageWidth_ << "\n Height: " << imageHeight_
    << "\n Pixel Depth: " << pixelDepth_ << "\n Img Desc: " << imageDescriptor_ << " end..  \n";


  std::cout << "--- HEADER DATA ---\n";
  for (int i = 1; i <= kHeaderLength_; i++)
    std::cout << std::bitset<8>(header_[i-1]) << ((i%6 == 0 && i > 0) ? " \n" : " ");
  std::cout << "--- PIXEL DATA ---\n";
  for (int i = 1; i <= (imageHeight_ * imageWidth_ * 3); ++i){
    std::cout << std::bitset<8>(data_[i-1]) << " " << ((i%6 == 0 && i > 0) ? " \n" : " ");
  }
}

