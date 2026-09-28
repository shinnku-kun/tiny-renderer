#pragma once
//抽象渲染层，主要管理渲染的形式，即用什么进行渲染,相当于实际渲染层和场景管理层的交换区，用来确定实际渲染的接口API
#include "SceneManagement.h"//引入场景管理层头文件，获取场景管理组件
#include"AcutalRender_CPU.h"//引入实际渲染层(CPU)，传递场景管理组件并调用实际渲染管线
#if defined(_WIN32)
	#include <SDKDDKVer.h>
	#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
	#include<Windows.h>
	//————————————————————————————————————————————
	//抽象渲染基类声明
	//抽象渲染流程，后续想换成硬件渲染直接添加子类即可
	class AbstractRender
	{
	public:
		virtual void SetUp(HWND hwnd,int width,int height) = 0;//初始化实际渲染层
		virtual void ForwardRender() = 0;//前向渲染
		virtual void DeferredRender() = 0;//延迟渲染
		virtual void RayTracing() = 0;//光线追踪
		virtual void HybridRender() = 0;//混合渲染
		virtual void Present() = 0;//呈现画面
		virtual ~AbstractRender() = default;
	};
	//————————————————————————————————————————————
	//CPU类声明
	class AbstractRender_CPU :public AbstractRender
	{
	public:
		//创建实际渲染管线的类，将各个组件实例指针的容器存入此类的私有变量成员中
		AbstractRender_CPU(ScenceManagement& ScenceManagement);
		~AbstractRender_CPU();
		void SetUp(HWND hwnd, int width, int height);//实际渲染管线初始化
		void ForwardRender();//前向渲染管线
		void DeferredRender();//延迟渲染管线
		void RayTracing();//光线追踪
		void HybridRender();//混合渲染管线
		void Present();//呈现画面
	private:
		CPURender* RenderPipeline;//实际渲染管线指针
	};
	//————————————————————————————————————————————


#endif