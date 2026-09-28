//读取贴图数据层
#include"TexLoad.h"
void TexLoad::ToTexLoad(std::vector<TextureData>& AllTextureData)//读取贴图数量和地址，调用贴图读取函数并存入数据管理层
{	//这里循环得到每个模块的地址
	//按资源分大目录，类型靠名称
	fs::path exePath = GetExePath();
	fs::path assets = exePath / "assets";
	fs::path textures = assets / "textures";
	for (const auto& model : fs::directory_iterator(textures))//遍历textures下的资源文件夹
	{
		if (model.is_directory())//如果是文件夹
		{
			for (const auto& mesh : fs::directory_iterator(model))//遍历资源文件夹下的每个模块文件夹
			{
				if (mesh.is_directory())//如果是文件夹
				{
					AllTextureData.push_back(EveryTexLoad(mesh.path()));
				}
			}
		}
	}
}
TextureData TexLoad::EveryTexLoad(const fs::path& meshPath)//这里传入模块地址
{
	TextureData texture;
	for (const auto& texturePath : fs::directory_iterator(meshPath))//遍历模块下的所有贴图资源
	{
		if (texturePath.is_regular_file())//如果是普通文件
		{
			std::string suffix = texturePath.path().stem().string();//将不含后缀的文件名转成字符串
			if (texturePath.is_regular_file() && texturePath.path().extension() == ".tga")//如果是tga类型的文件
			{
				if (suffix.find("_diffuse") != std::string::npos || suffix.find("_color") != std::string::npos)//如果找到颜色相关文件名
				{
					ToTGATexLoad(texture.vec_ColorMap, texture.ColorMapWidth, texture.ColorMapHeight, texturePath);
				}
				else if (suffix.find("_normal") != std::string::npos || suffix.find("_nm") != std::string::npos)//如果找到法线相关文件名(注意这里确定是切线空间的法线贴图)
				{
					ToTGATexLoad(texture.vec_NormalMap, texture.NormalMapWidth, texture.NormalMapHeight, texturePath);
				}
				//判断是什么类型的贴图然后调用相应函数并传入相应的容器引用和地址
			}
			//这里根据名称来进行判断是什么类型的贴图，并存入textureData中(其实不同贴图类型的读取应该写在这里)
		}
	}
	return texture;
}
