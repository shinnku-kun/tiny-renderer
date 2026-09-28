#pragma once
//读取TGA模型数据层
#include<vector>
#include"TextureDataStruct.h"
#include"GetExePath.h"//获取exe目录
#include"TGATexLoad.h"//用于TGA贴图类型的读取
class TexLoad
{
public:
	void ToTexLoad(std::vector<TextureData>& AllTextureData);//读取贴图数据并存入数据管理层
	TextureData EveryTexLoad(const fs::path& texturePath);//这里要传入地址的字符串
};