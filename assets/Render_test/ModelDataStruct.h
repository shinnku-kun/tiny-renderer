#pragma once
#include<vector>
//模型数据结构(一份模型应该有多个模块，方便进行替换)
struct MeshData
{
	std::vector<float> vec_VerList;//存放顶点数据容器指针
	std::vector<int> vec_VerIndex;//存放顶点索引容器指针

	std::vector<float> vec_NorList;//存放顶点法线容器指针
	std::vector<int> vec_NorIndex;//存放法线索引容器指针

	std::vector<float> vec_TanList;//存放顶点切线容器指针
	std::vector<int> vec_TanIndex;//存放切线索引容器指针

	std::vector<float> vec_TexList;//存放顶点纹理容器指针,注意纹理数据是每三个一组
	std::vector<int> vec_TexIndex;//存放纹理索引容器指针

	std::vector<int>vec_SecTanSign;//存放副切线的标志值
};
struct ModelData
{
	std::vector<MeshData> vec_Mesh;//存放多个模块
	int MeshCount=0;//存放模块数量，初始为0
};
