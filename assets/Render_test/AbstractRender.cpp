//抽象渲染层，主要管理渲染的形式，即用什么进行渲染,相当于实际渲染层和场景管理层的交换区，用来确定实际渲染的接口API
#include"AbstractRender.h"
//抽象渲染流程，后续想换成硬件渲染直接添加子类即可
#if defined(_WIN32)
	//————————————————————————————————————————————
	//CPU类实现
		//创建实际渲染管线的类，将各个组件实例指针的容器存入此类的私有变量成员中
		AbstractRender_CPU::AbstractRender_CPU(ScenceManagement& ScenceManagement) :RenderPipeline(new CPURender(ScenceManagement)) {}
		AbstractRender_CPU::~AbstractRender_CPU()
		{
			delete RenderPipeline;
			RenderPipeline = nullptr;
		}
		void AbstractRender_CPU::SetUp(HWND hwnd, int width, int height)//实际渲染管线初始化
		{
			RenderPipeline->Setup(hwnd,width,height);
		}
		void AbstractRender_CPU::ForwardRender()//前向渲染管线
		{
			RenderPipeline->ForwardRender();
		}
		void AbstractRender_CPU::DeferredRender()//延迟渲染管线
		{
			RenderPipeline->DeferredRender();
		}
		void AbstractRender_CPU::RayTracing()//光线追踪
		{
			RenderPipeline->RayTracing();
		}
		void AbstractRender_CPU::HybridRender()//混合渲染管线
		{
			RenderPipeline->HybridRender();
		}
		void AbstractRender_CPU::Present()//呈现画面
		{
			RenderPipeline->Present();
		}
	//————————————————————————————————————————————



#endif