 // WindowsPlatform.cpp : 定义windows窗口应用程序的入口点。
#include <cstdint>
#include "WindowsPlatform.h"
#include"ResourceManagement.h"//资源管理层
#include"SceneManagement.h"//场景管理层
#include"MessageProcessing.h"//消息处理层
#include"AbstractRender.h"//抽象渲染层
#define MAX_LOADSTRING 100


#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

// 全局变量:  
const wchar_t* szTitle = L"Render";// 标题栏文本
const wchar_t* szWindowClass = L"RenderWindow";// 主窗口类名
HWND hWnd;//实例窗口句柄
int WindowsWidth = 1000;//窗口宽度
int WindowsHeight = 800;//窗口高度
int  exitCode = 0;//退出码
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, WindowsMessageProcess*);





int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    timeBeginPeriod(1);   // main 开头:定时器精度提到 1ms
    MyRegisterClass(hInstance);//注册窗口

    //缓冲尺寸必须来自客户区，不是窗口
    RECT rc;
    GetClientRect(hWnd, &rc);
    int w = rc.right - rc.left;//宽    
    int h = rc.bottom - rc.top;//高
    //——————————————————————————————————————————————————
    //创建资源管理层
    ResourceManagement* Resource = new ResourceManagement();
    //初始化资源管理层
    Resource->SetUp();
    MSG msg;
    //——————————————————————————————————————————————————
    //创建场景管理层，传入资源管理层资源的引用，场景管理层只管理实例，不修改实例(只进行初始化或者数据读取)，修改实例和模型通过其他层来实现)
    ScenceManagement* Scene = new ScenceManagement(*Resource,w,h);
    //初始化场景管理层
    Scene->SetUp();//放在判断是否读取数据后面
    //——————————————————————————————————————————————————
    //创建消息处理层，用来接收用户消息并对用户消息进行反应，也是将场景管理层的引用传入并获取其内部的各个实例，方便修改
    WindowsMessageProcess* MessageProcess = new WindowsMessageProcess(*Scene);
    //传入实例句柄，后面可能会用到
    MessageProcess->SetWindowInstance(hInstance);
    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow,MessageProcess))
    {
        return FALSE;
    }
    //——————————————————————————————————————————————————
    //这里后续可以进行物理模拟层的初始化，将场景管理层的引用传入并获取其内部的各个实例，用来进行修改实现


    //——————————————————————————————————————————————————
    //创建抽象渲染层，创建CPU子类并传入场景管理层
    AbstractRender* Render = new AbstractRender_CPU(*Scene);
    //初始化实际渲染层数据
    Render->SetUp(hWnd,WindowsWidth,WindowsHeight);//传入窗口句柄和画布的大小
    //——————————————————————————————————————————————————
    const float FRAME_TIME = 1.f / 60.0f;  // 60 FPS

    using SteadyClock = std::chrono::steady_clock;
    auto lastTime = SteadyClock::now();
    float frameTimeSum = 0.f;// 累计帧时间
    int   frameCount = 0;// 帧计数
    float statTimer = 0.f;// 统计计时

    //uint32_t lastTime = GetTickCount();//获取循环前的时间(千毫秒级)
    bool bFrameCycle = true;//帧循环判断
    //帧循环
    while (bFrameCycle)
    {   
        // 主消息循环，获取按键消息修改并传递给消息处理层
        while (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
        {   
            if (msg.message == WM_QUIT)
            {
                bFrameCycle = false;//退出帧循环
                exitCode = (int)msg.wParam;// 取回 PostQuitMessage 的参数
                break;
            }
            MessageProcess->GetWindowsMessage(msg);//获取窗口消息，根据窗口消息修改场景组件和实例
            bFrameCycle = MessageProcess->GetbFrameCycle();//获取帧循环判断
        }
        if (!bFrameCycle) { break; }
        //uint32_t now = GetTickCount();//获取现在的时间(千毫秒级)
        SteadyClock::time_point now = (SteadyClock::now());
        float dt = std::chrono::duration<float>(now - lastTime).count();
       // float dt = (now - lastTime) / 1000.f;//插值转成秒级
        //如果时间差大于每秒的六十分之一，则进行渲染
        if (dt >= FRAME_TIME)
        {
            //测试Update
            Scene->UpDate(dt);
            //抽象渲染层进行实际渲染调用
            Render->ForwardRender();
            //呈现画布并进行交换
            Render->Present();

            frameTimeSum += dt * 1000.f;
            frameCount++;
            statTimer += dt;
            if (statTimer >= 1.f)//攒满一秒算平均
            {
                float avg = frameTimeSum / frameCount;
                wchar_t buf[128];
                swprintf_s(buf, 128, L"Render_test | 平均帧时间: %.2f ms | 约 %.0f FPS", avg, 1000.f / avg);
                SetWindowTextW(hWnd, buf);//直接写在窗口标题栏上
                frameTimeSum = 0.f; frameCount = 0; statTimer = 0.f;
            }


            lastTime = now;
        }
        else
        {
            Sleep(1);
        }
    }
    //——————————————————————————————————————————————————
    //释放抽象渲染层
    delete Render; Render = nullptr;
    //释放消息处理层
    delete MessageProcess; MessageProcess = nullptr;
    //释放物理模拟层
    
    //释放场景管理层
    delete Scene; Scene = nullptr;
    //释放资源管理层
    delete Resource; Resource = nullptr;

    timeEndPeriod(1);     // main 结尾
    return exitCode;
}
//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};          //零初始化，所有未显式赋值的成员归零

    wcex.cbSize = sizeof(WNDCLASSEXW);   // 用 W 版本，不混用 WNDCLASSEX

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = nullptr;   //没有菜单，显式写 nullptr
    wcex.lpszClassName  = szWindowClass;

    if (!RegisterClassExW(&wcex)) {
        DWORD err = GetLastError();   // 注册失败时能看到具体原因，而不是静默返回 0
        // 断点或日志输出 err
        return 0;
    }

    return RegisterClassExW(&wcex);
}
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, WindowsMessageProcess* MessageProcess)
{
   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WindowsWidth, WindowsHeight, nullptr, nullptr, hInstance, MessageProcess);//传入消息处理层的地址
   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}



