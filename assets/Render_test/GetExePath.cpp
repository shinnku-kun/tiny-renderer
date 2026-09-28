#include"GetExePath.h"
//跨平台获取指定目录

#if defined(_WIN32)
	#include<Windows.h>
#endif

fs::path GetExePath()
{
#if defined(_WIN32)
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return fs::path(buffer).parent_path();
#else
	return {}
#endif
}