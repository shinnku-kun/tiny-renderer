#pragma once	
#define NOMINMAX
//实际渲染层，这里实现cpu渲染，只实现渲染，不修改任何实例数据
#include "SceneManagement.h"//引入场景管理层头文件，获取管理层中的实例数据用于渲染
#include"ModelDataStruct.h"//引入模型结构
#include"TextureDataStruct.h"//引入贴图结构
#include<Eigen/Dense>
#include <utility>
#if defined(_WIN32)
	#include <SDKDDKVer.h>
	#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
	#include<Windows.h>	
	#include <algorithm>
	//VP矩阵结构体
	struct VPMat
	{
		Eigen::Matrix4f ViewMat{{ 1.f,0.f,0.f,0.f },
								{ 0.f,1.f,0.f,0.f },
								{ 0.f,0.f,1.f,0.f },
								{ 0.f,0.f,0.f,1.f }};//视图矩阵
		Eigen::Matrix4f ProMat{ { 1.f,0.f,0.f,0.f },
								{ 0.f,1.f,0.f,0.f },
								{ 0.f,0.f,1.f,0.f },
								{ 0.f,0.f,0.f,1.f } };//投影矩阵
	};
	//摄像机参数结构体
	struct CameraParameter
	{
		int CameraProjection;//投影类型
		Eigen::Vector3f Location;//摄像机组件世界位置
		Eigen::Vector3f LookLocation;//摄像机组件观察位置
		Eigen::Vector3f UpDir;//摄像机组件朝上向量
		float Fovy;//视野信息
		float Near;//近裁剪面信息
		float Far;//远裁剪面
		float Width;//摄像机渲染宽度
		float Height;//摄像机渲染高度
		float X; float Y;//摄像机渲染画面的起始位置
	};
	//光源参数结构体
	struct LightParameter
	{
		Eigen::Vector3f Location;//光源位置信息
		float Near;//光源近裁剪面
		float Far;//光源远裁剪面
		//以下为平行光独有参数
		Eigen::Vector3f Direction;//光源方向信息
		Eigen::Vector3f BoxCenter;//包围盒中心
		Eigen::Vector3f UpDir;//平行光默认朝上向量
		float BoxRadius;//包围盒半径
	};
	//材质参数结构体
	struct TexParameter
	{
		int LightingMode;//光照模式
		Eigen::Vector3f Color;//设置材质实例基础颜色
		Eigen::Vector3f Emissive;//设置基础自发光颜色
		float Metallic;//金属度
		float Roughness;//粗糙度
		float Ao;//环境光遮蔽
		float Opacity;//不透明度
		float Height;//高度位移
		float Ior;//折射率
		float Specular;//高光强度
		float Clearcoat;//清漆层
		float ClearcoatRoughness;//清漆粗糙度
		float Sheen;//光泽(布料)
		Eigen::Vector3f SheenColor;//光泽颜色
		float Anisotropy;//各向异性
		float AnisotropyRotation;//各项异性方向
		float Subsurface;//次表面散射
		Eigen::Vector3f SubsurfaceColor;//次表面颜色
		float Transmission;//透射(玻璃)
		float Thickness;//厚度
		float AlphaCutoff;//Alpha 裁剪阈值
		float BlendMode;//混合模式
		bool DoubleSided;//是否双面渲染
		bool Transparent;//是否透明
	};
	//模型参数结构体
	struct ModelParameter
	{
		Eigen::Vector3f Location = { 0,0,0 };//模型位置
		Eigen::Vector3f Rotate = { 0,0,0 };//模型旋转角度
		Eigen::Vector3f Zoom = { 1,1,1 };//模型缩放大小
		std::vector<int> MeshTexInstIndex;//各个模块的材质实例索引
	};
	//画布结构体
	struct Canvas
	{
		HDC hMemDC = nullptr;//内存设备
		HBITMAP hDibBitmap = nullptr;
		void* pPixelBuffer = nullptr;
		int width = 0;
		int height = 0;
		bool Create(int w, int h)//创建画布，画布创建不需要窗口句柄，只有呈现时需要
		{
			width = w;
			height = h;

			BITMAPINFO bmi = {};
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = w;
			bmi.bmiHeader.biHeight = -h;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;

			HDC hdcScreen = GetDC(NULL);
			hDibBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pPixelBuffer, NULL, 0);
			ReleaseDC(NULL, hdcScreen);

			if (!hDibBitmap)return false;

			hMemDC = CreateCompatibleDC(NULL);
			SelectObject(hMemDC, hDibBitmap);

			return true;
		}
		void Destory()
		{
			if (hMemDC) { DeleteDC(hMemDC); hMemDC = nullptr; }
			if (hDibBitmap) { DeleteObject(hDibBitmap); hDibBitmap = nullptr; }
			pPixelBuffer = nullptr;
		}
		UINT32* Pixels() { return (UINT32*)pPixelBuffer; }
	};
	//cpu实现渲染
	class CPURender
	{
	public:
		//有参构造
		CPURender(ScenceManagement& ScenceManagement);//获取组件实例指针容器的引用
		~CPURender();
		//渲染前初始化需要的参数,包括画布，缓冲区等
		void Setup(HWND hWnd, int width, int height);
		//前向渲染管线
		void ForwardRender();
		//延迟渲染管线(延迟渲染天然包含前向渲染，因为需要渲染透明物体)
		void DeferredRender();
		//光线追踪实现
		void RayTracing();
		//混合渲染管线(混合光栅化和光线追踪)
		void HybridRender();
		//获取所有模型的M矩阵
		void GetModelM();
		//获取所有摄像机VP矩阵
		void GetCameraVP();
		//获取所有光源VP矩阵
		void GetLightVP();
		//获取所有阴影深度贴图
		void GetShadowMap();
		//后处理
		void PostProcess();
		//将颜色绘制到当前帧的画布上
		void Draw();
		//呈现当前帧画面并交换当前帧和上一帧的画布
		void Present();
		//重置摄像机的颜色缓冲区和深度缓冲区
		void ResetCameraBuffer();
		//重置深度阴影缓冲
		void ResetShadow();

	private:
		//实例容器引用
		const std::vector<CameraInstance*>& pnum_CameraInst;
		const std::vector<LightInstance*>& pnum_LightInst;
		const std::vector<ModelInstance*>& pnum_ModelInst;
		const std::vector<TextureInstance*>& pnum_TextureInst;
		//各组件参数
		CameraParameter CameraPara;//摄像机参数
		CameraSet* Camera;//摄像机组件
		LightParameter LightPara;//光源参数
		LightSet* Light;//光源组件
		TextureInstance* TextureInst;//材质实例指针,因为材质实例是需要根据模块的材质索引来获取的，所以需要一个实例指针来存放获取的材质实例
		TexParameter TexturePara;//材质参数
		TextureSet* Texture;//材质组件
		ModelParameter ModelPara;//模型参数
		ModelSet* Model;//模型组件
		//渲染所需参数
		std::vector<Eigen::Matrix4f> ModelMat;//所有模型的m矩阵
		std::vector<Eigen::Matrix4f> ModelInverseTransposeMat;//所有模型的逆转置m矩阵
		std::vector<VPMat> CameraVP;//所有摄像机的vp矩阵
		//颜色缓冲和深度缓冲必须每个摄像机都在循环中清理才行
		std::vector<std::vector<UINT32>> CameraColorBuffer;//所有摄像机的颜色缓冲区
		std::vector<std::vector<float>> CameraDepthBuffer;//所有摄像机的深度缓冲区
		std::vector<VPMat> LightVP;//所有光源的vp矩阵
		std::vector<std::vector<float>> ShadowZbuffer;//所有光源的阴影深度缓冲区
		int ShadowWidth = 1024; int ShadowHeight = 1024;//阴影的分辨率
		//后处理需要的缓冲区，大小就是屏幕像素数量
		std::vector<UINT32 > PixelColorBuffer;//窗口的颜色缓冲区
		std::vector<Eigen::Vector3f>PixelNormalBuffer;//法线缓冲区,应该会改成所有摄像机的法线缓冲区
		std::vector<Eigen::Vector2f>PixelVelocityBuffer;//速度缓冲区，应该会改成所有摄像机的速度缓冲区
		std::vector<UINT32> FinallyPixelColorBuffer;//最终绘制到窗口画面的颜色缓冲区,除了第0帧外每帧在修改前会存放上一帧的颜色，也可以当作历史帧缓冲区使用
		Canvas CurrentFrame;//当前帧
		UINT32* CurrentPixels;//当前帧绘制
		Canvas LastFrame;//上一帧
		UINT32* LastPixels;//当前帧绘制
		uint64_t FrameCount = 0;//帧计数器
		//窗口相关参数
		HDC hdcWindow;//目标 DC
		int WindowsWidth; int WindowsHeight;//窗口宽高
	};
	//计算M模型矩阵
	Eigen::Matrix4f GetModelMat( Eigen::Vector3f const& Location, Eigen::Vector3f const& Rotate, Eigen::Vector3f const& Zoom);
	//计算V视图矩阵
	Eigen::Matrix4f GetViewMat(Eigen::Vector3f const& Location, Eigen::Vector3f const& LookLocation, Eigen::Vector3f const& UpDir);
	//计算P透视投影矩阵
	Eigen::Matrix4f GetPerProMat(float const& Fovy, float const& Aspect, float const& Near, float const& Far);
	//计算P正交投影矩阵
	Eigen::Matrix4f GetOrtProMat(float const& Left, float const& Right, float const& Bottom, float const& Top, float const& Near, float const& Far);

#endif