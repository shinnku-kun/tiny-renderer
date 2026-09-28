#pragma once
#include<vector>
//材质数据结构
struct TextureData
{
	std::vector<unsigned char> vec_ColorMap;//存放颜色贴图容器
	int ColorMapWidth = 0;int ColorMapHeight = 0;//颜色贴图大小
	std::vector<unsigned char> vec_NormalMap;//存放法线贴图容器(默认存放的是切线空间下的法线贴图)
	int NormalMapWidth = 0; int NormalMapHeight = 0;//法线贴图大小
	std::vector<unsigned char> vec_MetallicRoughnessMap;//粗糙度和金属度贴图,G为粗糙度，B为金属度
	int MetallicRoughnessWidth = 0; int MetallicRoughnessHeight = 0;
	std::vector<unsigned char> vec_OcclusionMap;//AO(环境光遮蔽)贴图,单存R通道
	int OcclusionWidth = 0; int OcclusionHeight = 0;
	std::vector<unsigned char> vec_EmissiveMap;//自发光贴图
	int EmissiveWidth = 0; int EmissiveHeight = 0;
};