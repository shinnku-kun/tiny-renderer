//资源管理层，主要管理各种数据，模型数据和贴图数据等
#include"ResourceManagement.h"
ResourceManagement::ResourceManagement() {}//初始化资源管理器
ResourceManagement::~ResourceManagement() { Uninstall(); }

void ResourceManagement::SetUp()//初始化资源管理层
{
	LoadModel();
	LoadTexture();
}
void ResourceManagement::LoadModel()//获取模型数据并组装成结构体存入容器中
{
	//Model = new ModelLoad();
	ModelLoad Model;
	Model.ToModelLoad(AllModelData);
}
void ResourceManagement::LoadTexture()//获取贴图数据并存入容器中
{
	//Tex = new TexLoad();
	TexLoad Tex;
	Tex.ToTexLoad(AllTextureData);
}
void ResourceManagement::Uninstall()//卸载资源管理层
{
	AllModelData.clear();
	AllTextureData.clear();
	//delete Model; Model = nullptr;
	//delete Tex; Tex = nullptr;
}
const ModelData& ResourceManagement::GetOneModel(size_t id)
{
	return AllModelData[id];
}
const std::vector<ModelData>& ResourceManagement::GetAllModelData(){return AllModelData;}//返回引用
const std::vector<TextureData>& ResourceManagement::GetAllTextureData(){return AllTextureData;}//返回引用






