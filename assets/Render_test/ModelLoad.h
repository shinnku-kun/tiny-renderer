#pragma once
//读取模型数据层
#include<vector>
#include"ModelDataStruct.h"//模型数据缓存结构
#include"GetExePath.h"//用于获取exe目录
#include"OBJModelLoad.h"//用于OBJ类型模型的读取
class ModelLoad
{
public:
	void ToModelLoad(std::vector<ModelData>& AllModelData);//读取模型数据并存入数据管理层
	ModelData EveryModelLoad(const fs::path& modelPath);//读取每一个模型并返回其结构体，这里要传入地址的字符串
	MeshData EveryMeshLoad(const fs::path& meshPath);//读取每一个模块并返回其结构体，一样要传入地址的字符串
};
