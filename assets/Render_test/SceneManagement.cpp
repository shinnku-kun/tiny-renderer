//场景管理层，管理各种组件和实例，会接收窗口层消息并进行SetUp和Update来实时修改数据
#include"SceneManagement.h"//引入头文件
//管理各类组件
//包括但不限于摄像机组件，光源组件，材质组件，模型组件
//获取修改值更新组件信息
//————————————————————————————————————————————
//场景管理组件
//场景管理类实现
	//构造函数，获取资源管理层的引用并初始化场景管理组件
	ScenceManagement::ScenceManagement(ResourceManagement& ResourceManagement, int Width, int Height):
	AllModelData(ResourceManagement.GetAllModelData()), //获取资源管理层中的所有模型数据的引用
	AllTextureData(ResourceManagement.GetAllTextureData()),//获取资源管理层中的所有贴图数据的引用
	WindowsWidth(Width),WindowsHeight(Height)//获取窗口宽高
	{GetSaveDataIsEmpty();}//创建时判断是否有已缓存数据
	//析构函数，释放所有组件和实例
	ScenceManagement::~ScenceManagement(){ ScenceManagement::Uninstall();}
	//判断保存的数据是否为空
	void ScenceManagement::GetSaveDataIsEmpty()
	{
		
	}
	//获取已保存数据
	void ScenceManagement::Load()
	{
		
	}
	//保存场景数据，下次加载可以直接读取已缓存数据
	void ScenceManagement::Save()
	{

	}
	//初始化场景管理组件及其实例(根据用户输入判断是否获取已保存数据)
	void ScenceManagement:: SetUp()
	{
		//如果确认读取已保存数据并且数据不为空
		if (ComFirmLoad&&(!SaveDataIsEmpty)){Load();}
		else {
			//初始化基础组件并创建相应组件的基础参数
			//摄像机组件
			pnum_CameraCom.push_back(new MainCamera());
			pnum_CameraCom[0]->CameraCreate(WindowsWidth,WindowsHeight);//初始化一个主摄像机组件
			//光源组件
			pnum_LightCom.push_back(new ParaLight());
			pnum_LightCom[0]->LightCreate();//初始化一个平行光组件
			//模型组件
			pnum_ModelCom.push_back(new StaticModel());
			pnum_ModelCom[0]->ModelCreate(&AllModelData[0]);//获取一个模型数据并初始化一个模型组件
			//材质组件
			pnum_TextureCom.push_back(new BaseTex());
			pnum_TextureCom[0]->TextureCreate(nullptr);//初始化一个基础材质组件
			pnum_TextureCom.push_back(new BaseTex());
			pnum_TextureCom[1]->TextureCreate(&AllTextureData[0]);//初始化一个带贴图的基础材质组件


			//初始化基础实例并把相应的基础组件存入
			//创建了两个摄像机实例并修改其实例值
			pnum_CameraInst.push_back(new CameraInstance());
			pnum_CameraInst[0]->CameraCom = pnum_CameraCom[0];//具体设置摄像机实例结构体所需组件
			pnum_CameraInst[0]->IsRender = true;//设置从此摄像机实例处渲染画面
			pnum_CameraInst[0]->UseDefault = false;//设置摄像机是否使用组件默认值，false则需要自己定义相应的摄像机实例参数
			pnum_CameraInst[0]->InstLocation = { 0,2,4 };
			pnum_CameraInst[0]->InstWidth = 1000; pnum_CameraInst[0]->InstHeight = 800;

			pnum_CameraInst.push_back(new CameraInstance());
			pnum_CameraInst[1]->CameraCom = pnum_CameraCom[0];//具体设置摄像机实例结构体所需组件
			pnum_CameraInst[1]->IsRender = true;//设置从此摄像机实例处渲染画面
			pnum_CameraInst[1]->UseDefault = false;//设置摄像机是否使用组件默认值，false则需要自己定义相应的摄像机实例参数
			pnum_CameraInst[1]->InstLocation = { 0,0,4 };
			pnum_CameraInst[1]->InstWidth = 250; pnum_CameraInst[1]->InstHeight = 200;
			pnum_CameraInst[1]->InstX = 0; pnum_CameraInst[1]->InstY = 0;

			//pnum_CameraInst.push_back(new CameraInstance());
			//pnum_CameraInst[2]->CameraCom = pnum_CameraCom[0];//具体设置摄像机实例结构体所需组件
			//pnum_CameraInst[2]->IsRender = true;//设置从此摄像机实例处渲染画面
			//pnum_CameraInst[2]->UseDefault = false;//设置摄像机是否使用组件默认值，false则需要自己定义相应的摄像机实例参数
			//pnum_CameraInst[2]->InstLocation = { 2,2,0 };
			//pnum_CameraInst[2]->InstWidth = 500; pnum_CameraInst[2]->InstHeight = 400;
			//pnum_CameraInst[2]->InstX = 0; pnum_CameraInst[2]->InstY = 400;

			//pnum_CameraInst.push_back(new CameraInstance());
			//pnum_CameraInst[3]->CameraCom = pnum_CameraCom[0];//具体设置摄像机实例结构体所需组件
			//pnum_CameraInst[3]->IsRender = true;//设置从此摄像机实例处渲染画面
			//pnum_CameraInst[3]->UseDefault = false;//设置摄像机是否使用组件默认值，false则需要自己定义相应的摄像机实例参数
			//pnum_CameraInst[3]->InstLocation = { 0,2,2 };
			//pnum_CameraInst[3]->InstWidth = 500; pnum_CameraInst[3]->InstHeight = 400;
			//pnum_CameraInst[3]->InstX = 500; pnum_CameraInst[3]->InstY = 400;

			//光源实例
			pnum_LightInst.push_back(new LightInstance());
			pnum_LightInst[0]->LightCom = pnum_LightCom[0];//具体设置光源实例结构体所需组件
			pnum_LightInst[0]->IsRender = true;//设置光源是否进行渲染
			pnum_LightInst[0]->UseDefault = false;//设置光源是否使用组件默认值，false则需要自己定义相应的光源实例参数

			//材质实例
			pnum_TextureInst.push_back(new TextureInstance());
			pnum_TextureInst[0]->TexCom = pnum_TextureCom[0];//具体设置材质实例结构体所需组件
			pnum_TextureInst[0]->UseDefault = true;//设置材质是否使用组件默认值，false则需要自己定义相应的材质实例参数
			pnum_TextureInst.push_back(new TextureInstance());
			pnum_TextureInst[1]->TexCom = pnum_TextureCom[1];//具体设置材质实例结构体所需组件
			pnum_TextureInst[1]->UseDefault = true;//设置材质是否使用组件默认值，false则需要自己定义相应的材质实例参数

			//模型实例
			pnum_ModelInst.push_back(new ModelInstance());
			pnum_ModelInst[0]->ModelCom = pnum_ModelCom[0];//具体设置模型实例结构体所需组件
			pnum_ModelInst[0]->IsRender = true;//设置模型是否进行渲染
			pnum_ModelInst[0]->UseDefault = false;//设置模型是否使用组件默认值，false则需要自己定义相应的模型实例参数,包括材质实例的下标索引
			pnum_ModelInst[0]->InstMeshTexInstIndex.resize(pnum_ModelCom[0]->MeshCount(),0);//开辟模块相应的材质数量
			pnum_ModelInst[0]->InstMeshTexInstIndex[0] = 1; pnum_ModelInst[0]->InstMeshTexInstIndex[1] = 0;//指定材质实例下标
		}
	}	
	//实例自更新
	void ScenceManagement::UpDate(float dt)
	{
		if (OffsetAngel >= 360)OffsetAngel = 0;
		//旋转相机
		(pnum_CameraInst[0]->InstLocation)[0] = std::sin(OffsetAngel * 0.0174) * Radios;//3.14/180
		(pnum_CameraInst[0]->InstLocation)[2] = std::cos(OffsetAngel * 0.0174) * Radios;
		(pnum_CameraInst[1]->InstLocation)[2] = std::sin(OffsetAngel * 0.0174) * Radios;//3.14/180
		(pnum_CameraInst[1]->InstLocation)[1] = std::cos(OffsetAngel * 0.0174) * Radios;
		//旋转光源
		//pnum_LightInst[0]->InstDirection[0]= std::sin(OffsetAngel * 0.0174);
		//pnum_LightInst[0]->InstDirection[2] = std::cos(OffsetAngel * 0.0174);


		OffsetAngel += 1;//每次增加旋转量
	}
	//释放所有组件和实例(根据用户输入判断是否保存所有数据)
	void ScenceManagement::Uninstall()
	{
		//如果确认保存数据
		if (ComFirmSave){Save();}
		//先删实例，再删组件
		//释放摄像机组件和实例
		for (CameraInstance* p : pnum_CameraInst) { delete p;}pnum_CameraInst.clear();
		for (CameraComponent* p : pnum_CameraCom) { delete p;}pnum_CameraCom.clear();

		//释放光源组件和实例
		for (LightInstance* p : pnum_LightInst) { delete p;}pnum_LightInst.clear();
		for (LightComponent* p : pnum_LightCom) { delete p;}pnum_LightCom.clear();

		//释放材质组件和实例
		for (TextureInstance* p : pnum_TextureInst) { delete p;}pnum_TextureInst.clear();
		for (TextureComponent* p : pnum_TextureCom) { delete p;}pnum_TextureCom.clear();

		//释放模型组件和实例
		for (ModelInstance* p : pnum_ModelInst) { delete p;}pnum_ModelInst.clear();
		for (ModelComponent* p : pnum_ModelCom) { delete p;}pnum_ModelCom.clear();

	}

	//获取用户读取判断
	bool* ScenceManagement::GetComfirmLoad() { return &ComFirmLoad; }
	//获取用户保存判断
	bool* ScenceManagement::GetComfirmSave() { return &ComFirmSave; }
	//提供各组件实例指针的容器的指针
	//std::vector<CameraInstance*>* ScenceManagement::GetCameraInst() { return &pnum_CameraInst; }
	//std::vector<LightInstance*>* ScenceManagement::GetLightInst() { return &pnum_LightInst; }
	//std::vector<ModelInstance*>* ScenceManagement::GetModelInst() { return &pnum_ModelInst; }
	//std::vector<TextureInstance*>* ScenceManagement::GetTexInst() { return &pnum_TextureInst; }
//————————————————————————————————————————————
//摄像机组件
	//··········································
	//主摄像机实现
	MainCamera::MainCamera(){MainCameraSet = new CameraSet();}
	MainCamera::~MainCamera()
	{
		delete MainCameraSet;
		MainCameraSet = nullptr;
	}
	//创建主摄像机组件基础参数
	void MainCamera::CameraCreate(float Width, float Height)
	{
		MainCameraSet->CameraType = 0;//设置主摄像机类型索引
		MainCameraSet->DefaultCameraProjection = 0;//设置主摄像机投影类型
		MainCameraSet->DefaultLocation = { 2.f, 0.5f, 4.f };//设置主摄像机默认位置
		MainCameraSet->DefaultLookLocation = { 0,0,0 };//设置主摄像机观察位置
		MainCameraSet->DefaultUpDir = { 0,1,0 };//设置主摄像机朝上向量
		MainCameraSet->DefaultFar = 100;//设置主摄像机远裁剪面
		MainCameraSet->DefaultNear = 0.1;//设置主摄像机近裁剪面
		MainCameraSet->DefaultFovy = 45;//设置主摄像机垂直视野角度
		MainCameraSet->DefaultWidth = Width;//设置主摄像机宽度
		MainCameraSet->DefaultHeight = Height;//设置主摄像机高度
		MainCameraSet->DefaultX = 0; MainCameraSet->DefaultY = 0;//设置渲染画面的起始位置
	}
	int MainCamera::GetCameraType(){return MainCameraSet->CameraType;}//获取摄像机类型
	CameraSet* MainCamera::GetCamera(){return MainCameraSet;}//获取摄像机基础参数
	//··········································
//————————————————————————————————————————————
//光源组件
	//··········································
	//平行光实现
	ParaLight::ParaLight(){ParaLightSet = new LightSet();}
	ParaLight::~ParaLight()
	{
		delete ParaLightSet;
		ParaLightSet = nullptr;
	}
	//创建光源基础参数
	void ParaLight::LightCreate()
	{
		ParaLightSet->LightType = 0;//设置平行光类型索引，在具体实现渲染中会根据此索引来获取相应参数
		ParaLightSet->DefaultDirection = { 1,1,1 };//平行光方向
		ParaLightSet->DefaultBoxCenter = { 0,0,0 };//平行光包围盒中心
		ParaLightSet->DefaultUpDir = { 0,1,0 };//平行光朝上向量
		ParaLightSet->DefaultNear = 0.1;//近裁剪面
		ParaLightSet->DefaultFar = 100;//远裁剪面
		ParaLightSet->DefaultBoxRadius = 4.f;//平行光包围盒半径
	}
	int ParaLight::GetLightType(){return ParaLightSet->LightType;}//获取光源类型
	LightSet* ParaLight::GetLight(){return ParaLightSet;}//获取光源基础参数
	//··········································
//————————————————————————————————————————————
//材质组件
	//··········································	
	//基础材质实现
	BaseTex::BaseTex(){BaseTexSet = new TextureSet();}
	BaseTex::~BaseTex()
	{
		delete BaseTexSet;
		BaseTexSet = nullptr;
	}
	//创建材质基础参数，可以通过设置一些独有的参数，用于判断和运算
	void BaseTex::TextureCreate(const TextureData* TextureData)
	{
		BaseTexSet->TexType = 0;//材质类型
		BaseTexSet->DefaultColor = { 255,255,255 };//基础颜色设置为白色
		BaseTexSet->DefaultLightingMode = 0;//光照模式，0为Blinn-Phong模式
		if (TextureData != nullptr)//不为空
		{
			//颜色贴图
			BaseTexSet->ColorMapWidth = TextureData->ColorMapWidth;
			BaseTexSet->ColorMapHeight = TextureData->ColorMapHeight;
			BaseTexSet->pvec_ColorMap = &(TextureData->vec_ColorMap);
			//法线贴图
			BaseTexSet->NormalMapWidth = TextureData->NormalMapWidth;
			BaseTexSet->NormalMapHeight = TextureData->NormalMapHeight;
			BaseTexSet->pvec_NormalMap = &(TextureData->vec_NormalMap);
		}
		//基础材质不设置任何其他属性，只设置颜色属性	
	}
	int BaseTex::GetTextureType(){return BaseTexSet->TexType;}//获取材质类型
	bool BaseTex::GetTextureTransparent(){return BaseTexSet->DefaultTransparent;}//获取材质透明与否
	TextureSet* BaseTex::GetTex(){return BaseTexSet;}//获取材质基础参数
	//··········································
//————————————————————————————————————————————
//模型组件
	//··········································
	//静态模型数据实现，按读取的模型分别存放
	StaticModel::StaticModel(){StaticModelSet = new ModelSet();}
	StaticModel::~StaticModel()
	{
		delete StaticModelSet;
		StaticModelSet = nullptr;
	}
	//创建模型基础参数
	void StaticModel::ModelCreate(const ModelData* ModelData)//这里需要传入相应的数据容器指针
	{
		StaticModelSet->ModelType = 0;//设置静态模型类型索引
		StaticModelSet->DefaultLocation = { 0,0,0 };//设置模型组件默认位置
		StaticModelSet->DefaultRotate = { 0,0,0 };//设置模型组件默认旋转角度
		StaticModelSet->DefaultZoom = { 1,1,1 };//设置模型组件默认缩放大小
		//这里需要获取模型的各种数据容器的指针，即数据管理层的容器的指针的一个模型结构体的指针
		if (ModelData != nullptr)//如果数据容器指针不为空
		{
			StaticModelSet->pvec_Mesh = &(ModelData->vec_Mesh);//获取各模块指针，模块内部有各种数据
			StaticModelSet->DefaultMeshTexInstIndex.resize(ModelData->MeshCount,0);//获取模块数量开辟相应的材质实例索引数量并初始化材质实例索引为0
		}
	}
	int StaticModel::GetModelType(){return StaticModelSet->ModelType;}//获取模型类型
	ModelSet* StaticModel::GetModel(){return StaticModelSet;}//获取模型基础参数指针
	//获取模型模块数量，用于模型实例的材质实例索引容器的空间开辟
	int StaticModel::MeshCount(){return StaticModelSet->DefaultMeshTexInstIndex.size();}
	//··········································
//————————————————————————————————————————————