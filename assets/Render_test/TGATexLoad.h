#pragma once
#include"TextureDataStruct.h"//材质结构体
#include<filesystem>
#include <fstream>
//tga图片读取
#include "stb_image.h"
namespace fs = std::filesystem;
void ToTGATexLoad(std::vector<unsigned char>& texture, int& width, int& height, const fs::path& texturePath);
