#pragma once
//消息处理层，用来接收窗口消息并据此修改场景中的实例
#include"SceneManagement.h"
#if defined(_WIN32)
	#include <SDKDDKVer.h>
	#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
	#include<Windows.h>
	class WindowsMessageProcess
	{
	public:
		WindowsMessageProcess(ScenceManagement& ScenceManagement); //获取组件实例指针容器的引用
		~WindowsMessageProcess();
		LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//内部进行窗口的消息处理
		void SetWindowInstance(HINSTANCE& instance);//获取实例句柄指针
		void GetWindowsMessage(MSG& msg);//获取窗口消息
		bool GetbFrameCycle();//返回帧循环判断
	private:
		std::vector<CameraInstance*>* pnum_CameraInst = nullptr;
		std::vector<LightInstance*>* pnum_LightInst = nullptr;
		std::vector<ModelInstance*>* pnum_ModelInst = nullptr;
		std::vector<TextureInstance*>* pnum_TextureInst = nullptr;
		bool bFrameCycle = true;//帧循环判断
		//这两个值会在窗口消息循环判断中进行修改，用于确认是否进行读取和保存
		bool* ComFirmLoad = nullptr;//读取确认的指针，指向场景管理层中的ComFirmLoad
		bool* ComFirmSave = nullptr;//保存确认的指针，指向场景管理层中的ComFirmSave
		HINSTANCE* hInst;//当前实例句柄的指针
	};
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//回调函数调给类内部的成员函数

#endif