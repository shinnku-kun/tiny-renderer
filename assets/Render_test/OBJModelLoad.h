#pragma once
#include"ModelDataStruct.h"//模型结构体
#include<filesystem>
#include <fstream>
//Eigen数学库
#include<Eigen/Dense>
namespace fs = std::filesystem;
void ToOBJModelLoad(MeshData& mesh, const fs::path& meshPath);
