#pragma once
//跨平台获取指定目录
#include<filesystem>
namespace fs = std::filesystem;
fs::path GetExePath();