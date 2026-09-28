#pragma once
//场景管理层，管理各种组件和实例，不进行修改和更新，只进行管理
#include<vector>
#include<iostream>
//Eigen数学库
#include<Eigen/Dense>
#include"ResourceManagement.h"//引入数据管理层，获取数据
//摄像机组件
//··········································
//摄像机默认基础参数声明
struct CameraSet
{
	int CameraType = 0;//摄像机组件默认类型
	int DefaultCameraProjection = 0;//摄像机组件默认投影类型，默认为透视投影类型
	Eigen::Vector3f DefaultLocation = { 2.f, 0.5f, 4.f };//摄像机组件默认世界位置
	Eigen::Vector3f DefaultLookLocation = { 0,0,0 };//摄像机组件默认观察位置
	Eigen::Vector3f DefaultUpDir = { 0,1,0 };//摄像机组件默认朝上向量
	float DefaultFovy = 45;//摄像机默认垂直视野角度
	float DefaultNear = 0.1;//摄像机默认近裁剪面
	float DefaultFar = 1000;//摄像机默认远裁剪面
	float DefaultWidth = 1000;//摄像机默认渲染宽度
	float DefaultHeight = 800;//摄像机默认渲染高度
	float DefaultX = 0; float DefaultY = 0;//摄像机渲染画面的默认起始位置
};
//··········································
//摄像机组件基类声明
class CameraComponent
{
public:
	virtual void CameraCreate(float Width, float Height) = 0;//纯虚函数实现
	virtual CameraSet* GetCamera() = 0;
	virtual int GetCameraType() = 0;//获取摄像机类型
	virtual ~CameraComponent() = default;
};
//··········································
	//主摄像机声明
	class MainCamera :public CameraComponent
	{
	public:
		MainCamera();
		MainCamera(const MainCamera&) = delete;//取消拷贝构造函数
		MainCamera& operator=(const MainCamera&) = delete;//取消赋值操作
		~MainCamera();
		void CameraCreate(float Width, float Height);//创建主摄像机组件基础参数
		int GetCameraType();//获取摄像机类型
		CameraSet* GetCamera();//获取摄像机参数
	private:
		CameraSet* MainCameraSet;//主摄像机参数
	};
//··········································
//摄像机实例声明
struct CameraInstance
{
	CameraComponent* CameraCom;//默认组件信息
	bool IsRender = false;//是否从此摄像机实例渲染画面，默认为否
	bool UseDefault = true;//是否使用组件默认值而不重新设定实例的值,默认为是，即使用组件默认值
	int InstCameraProjection = 0;//实例投影类型
	Eigen::Vector3f InstLocation = { 2.f, 0.5f, 4.f };//摄像机组件世界位置
	Eigen::Vector3f InstLookLocation = { 0,0,0 };//摄像机组件观察位置
	Eigen::Vector3f InstUpDir = { 0,1,0 };//摄像机组件朝上向量
	float InstFovy = 45;//实例视野信息
	float InstNear = 0.1;//实例近裁剪面信息
	float InstFar = 1000;//实例远裁剪面
	float InstWidth = 1000;//摄像机实例渲染宽度
	float InstHeight = 800;//摄像机实例渲染高度
	float InstX = 0; float InstY = 0;//摄像机渲染画面的起始位置
};
//··········································
//————————————————————————————————————————————
//光源组件
//··········································
//光源基础参数声明
struct LightSet
{
	int LightType = 0;//光源类型
	//后续可以考虑用字典(map)容器根据光源类型在子类中删除不用的参数
	Eigen::Vector3f DefaultLocation = { 0,0,0 };//光源默认位置
	float DefaultNear = 0.1;//光源默认近裁剪面
	float DefaultFar = 100;//光源默认远裁剪面
	//以下为平行光独有参数
	Eigen::Vector3f DefaultDirection = { 1,1,1 };//光源默认方向，这里为指向光源方向
	Eigen::Vector3f DefaultBoxCenter = { 0,0,0 };//包围盒默认中心位置
	Eigen::Vector3f DefaultUpDir = { 0,1,0 };//平行光默认朝上向量
	float DefaultBoxRadius = 4.f;//包围盒默认半径
};
//光源组件基类声明
class LightComponent
{
public:
	virtual void LightCreate() = 0;//纯虚函数实现
	virtual LightSet* GetLight() = 0;
	virtual int GetLightType() = 0;//获取光源类型
	virtual ~LightComponent() = default;
};
//··········································
	//平行光声明
	class ParaLight :public LightComponent
	{
	public:
		ParaLight();
		ParaLight(const ParaLight&) = delete;//取消拷贝构造函数
		ParaLight& operator=(const ParaLight&) = delete;//取消赋值操作
		~ParaLight();
		void LightCreate();//创建光源基础参数
		int GetLightType();//获取平行光类型
		LightSet* GetLight();//获取光源基础参数
	private:
		LightSet* ParaLightSet;//平行光参数
	};
//··········································
//光源实例声明
struct LightInstance
{
	LightComponent* LightCom;//组件信息
	bool IsRender = true;//是否渲染此光源，默认为是
	bool UseDefault = true;//是否使用组件默认值而不重新设定实例的值,默认为是，即使用组件默认值
	Eigen::Vector3f InstLocation = { 0,0,0 };//光源位置信息
	float InstNear = 0.1;//光源近裁剪面
	float InstFar = 100;//光源远裁剪面
	//以下为平行光独有参数
	Eigen::Vector3f InstDirection = { 1,1,1 };//光源方向信息,平行光专有
	Eigen::Vector3f InstBoxCenter = { 0,0,0 };//包围盒中心
	Eigen::Vector3f InstUpDir = { 0,1,0 };//平行光朝上向量
	float InstBoxRadius = 4.f;//包围盒半径
};
//··········································
//————————————————————————————————————————————
//材质组件
//··········································
//材质基础参数声明(光照计算方式通过材质类型来确定)
struct TextureSet
{
	int TexType = 0;//设置材质类型
	int DefaultLightingMode = 0;//设置默认光照模式,0为布林冯模式
	Eigen::Vector3f DefaultColor = { 255,255,255 };//设置默认基础颜色
	Eigen::Vector3f DefaultEmissive = { 0,0,0 };//设置默认基础自发光颜色
	float DefaultMetallic = 0;//金属度
	float DefaultRoughness = 0;//粗糙度
	float DefaultAo = 1;//环境光遮蔽
	float DefaultOpacity = 0;//不透明度
	float DefaultHeight = 0;//高度位移
	float DefaultIor = 1;//折射率
	float DefaultSpecular = 0;//高光强度
	float DefaultClearcoat = 0;//清漆层
	float DefaultClearcoatRoughness = 0;//清漆粗糙度
	float DefaultSheen=0;//光泽(布料)
	Eigen::Vector3f DefaultSheenColor = { 0,0,0 };//光泽颜色
	float DefaultAnisotropy=0;//各向异性
	float DefaultAnisotropyRotation=0;//各项异性方向
	float DefaultSubsurface=0;//次表面散射
	Eigen::Vector3f DefaultSubsurfaceColor = { 0,0,0 };//次表面颜色
	float DefaultTransmission=0;//透射(玻璃)
	float DefaultThickness=0;//厚度
	float DefaultAlphaCutoff = 0;//Alpha 裁剪阈值
	float DefaultBlendMode = 0;//混合模式
	bool DefaultDoubleSided = false;//是否双面渲染
	bool DefaultTransparent = false;//是否透明
	//存放材质的不同贴图的指针
	const std::vector<unsigned char>* pvec_ColorMap = nullptr;//颜色贴图容器指针
	int ColorMapWidth = 0; int ColorMapHeight = 0;//颜色贴图大小
	const std::vector<unsigned char>* pvec_NormalMap = nullptr;//法线贴图容器指针
	int NormalMapWidth = 0; int NormalMapHeight = 0;//法线贴图大小	
	const std::vector<unsigned char>* pvec_MetallicRoughnessMap = nullptr;//粗糙度和金属度贴图,G为粗糙度，B为金属度
	int MetallicRoughnessWidth = 0; int MetallicRoughnessHeight = 0;
	const std::vector<unsigned char>* pvec_OcclusionMap = nullptr;//AO(环境光遮蔽)贴图,单存R通道
	int OcclusionWidth = 0; int OcclusionHeight = 0;
	const std::vector<unsigned char>* pvec_EmissiveMap = nullptr;//自发光贴图
	int EmissiveWidth = 0; int EmissiveHeight = 0;
};
//··········································
//材质组件基类声明
class TextureComponent
{
public://纯虚函数实现
	virtual void TextureCreate(const TextureData* TextureData) = 0;
	virtual TextureSet* GetTex() = 0;
	virtual int GetTextureType() = 0;//获取材质类型
	virtual bool GetTextureTransparent() = 0;//获取材质是否透明
	virtual ~TextureComponent() = default;
};
//··········································
	//基础材质声明
	class BaseTex :public TextureComponent
	{
	public:
		BaseTex();
		BaseTex(const BaseTex&) = delete;//取消拷贝构造函数
		BaseTex& operator=(const BaseTex&) = delete;//取消赋值操作
		~BaseTex();
		void TextureCreate(const TextureData* TextureData);//创建材质基础参数，可以通过设置一些独有的参数，用于判断和运算
		int GetTextureType();//获取材质类型
		bool GetTextureTransparent();//获取材质是否透明
		TextureSet* GetTex();//获取材质基础参数
	private:
		TextureSet* BaseTexSet;//基础材质各类参数
	};
//··········································
//材质实例声明
struct TextureInstance
{
	TextureComponent* TexCom;//组件信息
	bool UseDefault = true;//是否使用组件默认值而不重新设定实例的值,默认为是，即使用组件默认值
	int InstLightingMode = 0;//设置实例光照模式
	Eigen::Vector3f InstColor = { 255,255,255 };//设置材质实例基础颜色
	Eigen::Vector3f InstEmissive = { 0,0,0 };//设置基础自发光颜色
	float InstMetallic = 0;//金属度
	float InstRoughness=0;//粗糙度
	float InstAo=1;//环境光遮蔽
	float InstOpacity=0;//不透明度
	float InstHeight=0;//高度位移
	float InstIor=1;//折射率
	float InstSpecular=0;//高光强度
	float InstClearcoat=0;//清漆层
	float InstClearcoatRoughness=0;//清漆粗糙度
	float InstSheen=0;//光泽(布料)
	Eigen::Vector3f InstSheenColor = { 0,0,0 };//光泽颜色
	float InstAnisotropy=0;//各向异性
	float InstAnisotropyRotation=0;//各项异性方向
	float InstSubsurface=0;//次表面散射
	Eigen::Vector3f InstSubsurfaceColor = {0,0,0};//次表面颜色
	float InstTransmission=0;//透射(玻璃)
	float InstThickness=0;//厚度
	float InstAlphaCutoff=0;//Alpha 裁剪阈值
	float InstBlendMode=0;//混合模式
	bool InstDoubleSided=false;//是否双面渲染
	bool InstTransparent=false;//是否透明
};
//··········································
//————————————————————————————————————————————
//模型组件
//··········································
//模型基础参数声明
struct ModelSet
{
	int ModelType = 0;//设置模型类型
	const std::vector<MeshData>* pvec_Mesh = nullptr;//各个模块
	Eigen::Vector3f DefaultLocation = { 0,0,0 };//模型默认位置
	Eigen::Vector3f DefaultRotate = { 0,0,0 };//模型默认旋转角度
	Eigen::Vector3f DefaultZoom = { 1,1,1 };//模型默认缩放大小
	std::vector<int> DefaultMeshTexInstIndex;//各个模块的默认材质实例索引
};
//··········································
//模型组件基类声明
class ModelComponent
{
public://纯虚函数实现
	virtual void ModelCreate(const ModelData* ModelData) = 0;
	virtual ModelSet* GetModel() = 0;
	virtual int GetModelType() = 0;
	virtual int MeshCount() = 0;//获取模型的模块数量给模型实例
	virtual ~ModelComponent() = default;
};
//··········································
	//静态模型数据声明，按读取的模型分别存放
	class StaticModel :public ModelComponent
	{
	public:
		StaticModel();
		StaticModel(const StaticModel&) = delete;//取消拷贝构造函数
		StaticModel& operator=(const StaticModel&) = delete;//取消赋值操作
		~StaticModel();
		void ModelCreate(const ModelData* ModelData);//创建模型基础参数
		int GetModelType();//获取模型类型
		ModelSet* GetModel();//获取模型基础参数
		int MeshCount();//获取模型的模块数量给模型实例
	private:
		ModelSet* StaticModelSet;//模型参数
	};
//··········································
//模型实例声明
struct ModelInstance
{
	ModelComponent* ModelCom;//组件信息
	bool IsRender = true;//是否渲染此模型，默认为是
	bool UseDefault = true;//是否使用组件默认值而不重新设定实例的值,默认为是，即使用组件默认值
	Eigen::Vector3f InstLocation = { 0,0,0 };//位置信息
	Eigen::Vector3f InstRotate = { 0,0,0 };//旋转信息
	Eigen::Vector3f InstZoom = { 1,1,1 };//模型缩放大小
	std::vector<int> InstMeshTexInstIndex;//模块材质实例索引(需要通过MeshCount获取模块数量并开辟空间)
};
//··········································
//————————————————————————————————————————————







//————————————————————————————————————————————
//场景管理组件
//场景管理类声明
class ScenceManagement
{
public:
	ScenceManagement(ResourceManagement& ResourceManagement,int Width,int Height);
	~ScenceManagement();
	void SetUp();//初始化
	void Load();//读取已保存数据
	void Save();//保存数据
	void Uninstall();//卸载
	void UpDate(float dt);//测试更新
	bool* GetComfirmLoad();//获取用户读取判断
	bool* GetComfirmSave();//获取用户保存判断
	void GetSaveDataIsEmpty();//判断保存的数据是否为空
	//提供各组件实例指针的容器的指针
	std::vector<CameraInstance*>* GetCameraInst() { return &pnum_CameraInst; };
	std::vector<LightInstance*>* GetLightInst() { return &pnum_LightInst; };
	std::vector<ModelInstance*>* GetModelInst() { return &pnum_ModelInst; };
	std::vector<TextureInstance*>* GetTexInst() { return &pnum_TextureInst; };
private:
	float Radios = 6; float OffsetAngel = 10;//Update测试用数据
	int WindowsWidth; int WindowsHeight;//窗口宽高
	//存放用户的判断
	bool ComFirmLoad=false;//默认不读取
	bool ComFirmSave=true;//默认保存
	bool SaveDataIsEmpty = true;//缓存数据是否为空
	//存放模型数据和贴图数据指针的引用,引用必须在构造类时就赋值，否则会出错
	const std::vector<ModelData>& AllModelData;
	const std::vector<TextureData>& AllTextureData;
	//通过容器保存各个组件的各个子类的指针，通过继承和多态实现同一组件不同类型的读取
	std::vector<CameraComponent*> pnum_CameraCom;
	std::vector<LightComponent*> pnum_LightCom;
	std::vector<ModelComponent*> pnum_ModelCom;
	std::vector<TextureComponent*> pnum_TextureCom;
	//通过容器保存不同组件的多个实例
	std::vector<CameraInstance*> pnum_CameraInst;
	std::vector<LightInstance*> pnum_LightInst;
	std::vector<ModelInstance*> pnum_ModelInst;
	std::vector<TextureInstance*> pnum_TextureInst;
};
//————————————————————————————————————————————
