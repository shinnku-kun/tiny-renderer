//消息处理层，用来接收窗口消息并据此修改场景中的实例
#include"MessageProcessing.h"
#if defined(_WIN32)
	WindowsMessageProcess::WindowsMessageProcess(ScenceManagement& ScenceManagement)://获取组件实例指针容器的指针
			pnum_CameraInst(ScenceManagement.GetCameraInst()),
			pnum_LightInst(ScenceManagement.GetLightInst()),
			pnum_ModelInst(ScenceManagement.GetModelInst()),
			pnum_TextureInst(ScenceManagement.GetTexInst()),
			//这两个值会在窗口消息循环判断中进行修改，用于确认是否进行读取和保存
			ComFirmLoad(ScenceManagement.GetComfirmLoad()),
			ComFirmSave(ScenceManagement.GetComfirmSave()){}
	WindowsMessageProcess::~WindowsMessageProcess(){}
	LRESULT WindowsMessageProcess::HandleMessage(HWND hWnd, UINT message,WPARAM wParam,LPARAM lParam)//内部进行窗口的消息处理
	{//后续只要在这里进行消息处理就行了
			switch (message)
			{
			case WM_CLOSE:
			{
				DestroyWindow(hWnd);
				break;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			break;
			case WM_DESTROY:
				PostQuitMessage(0);
				bFrameCycle = false;//退出帧循环
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			return 0;
	}
	void WindowsMessageProcess::GetWindowsMessage(MSG& msg)//获取窗口消息
	{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
	}
	void WindowsMessageProcess::SetWindowInstance(HINSTANCE& instance) { hInst = &instance; }//获取实例句柄指针
	bool WindowsMessageProcess::GetbFrameCycle() { return bFrameCycle; }//返回帧循环判断


	//回调函数调给类内部的消息处理成员函数
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WindowsMessageProcess* self = (WindowsMessageProcess*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!self)
		{
			if (message != WM_NCCREATE)return DefWindowProc(hWnd, message, wParam, lParam);

			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			self = (WindowsMessageProcess*)cs->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)self);
		}
		return self->HandleMessage(hWnd, message, wParam, lParam);
	}
#endif
