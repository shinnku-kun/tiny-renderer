//读取模型数据层
#include"ModelLoad.h"
void ModelLoad::ToModelLoad(std::vector<ModelData>& AllModelData)//从指定路径目录下读取模型地址，调用模型读取函数并存入数据管理层
{
	fs::path exePath = GetExePath();//获取exe目录
	fs::path assets = exePath / "assets";//获取资源目录
	fs::path modelsPath = assets / "models";//获取所有模型的目录
	for (const auto& model : fs::directory_iterator(modelsPath))//遍历所有模型目录下的所有文件夹
	{
		if (model.is_directory())//如果是文件夹
		{
			AllModelData.push_back(EveryModelLoad(model.path()));//这样可以少复制一次
		}
	}
}
ModelData ModelLoad::EveryModelLoad(const fs::path& modelPath)//读取每一个模型,获取其模块数量和模块数据并返回其结构体，这里要传入地址的字符串
{
	int i = 0;
	ModelData model;
	for (const auto& mesh : fs::directory_iterator(modelPath))//遍历一个模型下的所有模型文件(即所有模块)
	{
		if (mesh.is_regular_file())//如果是普通文件
		{
			model.vec_Mesh.push_back(EveryMeshLoad(mesh.path()));//调用模块读取函数并存入
			i++;
		}
	}
	model.MeshCount = i;
	return model;
}
MeshData ModelLoad::EveryMeshLoad(const fs::path& meshPath)//读取每一个模块(每一个模型)并返回其结构体
{
	MeshData mesh;
	//判断是什么类型的模块然后调用相应函数并传入结构体引用
	if (meshPath.extension() == ".obj")//如果是obj类型的文件
	{
		ToOBJModelLoad(mesh, meshPath);//调用相应函数
	}
	return mesh;
}



