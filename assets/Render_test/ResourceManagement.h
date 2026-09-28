#pragma once
//资源管理层，主要管理各种数据，模型数据和贴图数据等
#include<vector>
#include"ModelLoad.h"
#include"ModelDataStruct.h"
#include"TexLoad.h"
#include"TextureDataStruct.h"
class ResourceManagement
{
public:
	ResourceManagement();//初始化资源管理器
	~ResourceManagement();
	void SetUp();//初始化资源管理层
	void Uninstall();//卸载资源管理层

	void LoadModel();//获取模型数据并组装成结构体存入容器中
	void LoadTexture();//获取贴图数据并存入容器中

	//返回容器引用
	const std::vector<ModelData>& GetAllModelData();
	const std::vector<TextureData>& GetAllTextureData();
	const ModelData& GetOneModel(size_t id);
private:
	std::vector<ModelData> AllModelData;//所有模型数据结构体的容器
	std::vector<TextureData> AllTextureData;//所有贴图结构体的容器
	//ModelLoad* Model;//"模型读取"类的指针
	//TexLoad* Tex;//"贴图读取"类的指针
};


