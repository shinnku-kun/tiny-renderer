//OBJ模型读取
#include"OBJModelLoad.h"
void ToOBJModelLoad(MeshData& mesh, const fs::path& meshPath)
{
    std::vector<float>  vec_stViceTanList;//副切线数据
    std::vector<int>    vec_nViceTanIndex;//副切线索引
    //读取模型顶点数据
    //创建输入流对象，打开文件
    std::ifstream infile(meshPath);
    if (!infile.is_open())
    {
        throw std::runtime_error("文件解析失败");
    }
    std::string line;
    std::string skip;
    //读取每行数据,获取顶点，纹理以及法线的数据和索引
    while (std::getline(infile, line))
    {
        //跳过空行
        if (line.empty()) continue;
        //把当前这一行字符串丢进 stringstream 流中
        std::stringstream ss(line);
        //存入顶点数据
        if (line[0] == 'v')
        {
            //是顶点坐标
            if (line[1] == ' ')
            {
                //读取顶点数据
                float VerData;
                //>>读取会自动中断在空格处
                ss >> skip;
                ss >> VerData;
                mesh.vec_VerList.push_back(VerData);//存入容器中
                ss >> VerData;
                mesh.vec_VerList.push_back(VerData);
                ss >> VerData;
                mesh.vec_VerList.push_back(VerData);
            }
            else if (line[1] == 'n')
            {
                //读取法线数据
                float NorData;
                ss >> skip;
                ss >> NorData;
                mesh.vec_NorList.push_back(NorData);
                ss >> NorData;
                mesh.vec_NorList.push_back(NorData);
                ss >> NorData;
                mesh.vec_NorList.push_back(NorData);
            }
            else if (line[1] == 't')
            {
                //读取纹理数据
                float TexData;
                ss >> skip;
                ss >> TexData;
                mesh.vec_TexList.push_back(TexData);
                ss >> TexData;
                mesh.vec_TexList.push_back(TexData);
                if (!(ss >> TexData))//如果读取不成功，返回0
                {
                    TexData = 0;
                }
                mesh.vec_TexList.push_back(TexData);
            }
        }
        //存入面索引
        else if (line[0] == 'f')
        {
            //std::string stRea;//获取每段数据
            //ss >> skip;
            //while (ss >> stRea)
            //{
            //    if (stRea.empty()) break;

            //    //获取‘/’符号的位置
            //    size_t nSkPos1 = stRea.find('/');
            //    size_t nSkPos2 = stRea.find('/', nSkPos1 + 1);//获取第二个/的位置，从第一个的后一位开始

            //    //将数据存入字符串中
            //    std::string stVerIndexStr = stRea.substr(0, nSkPos1);
            //    //转整型
            //    int nVerIndex = std::stoi(stVerIndexStr) - 1;
            //    //将索引存入
            //    mesh.vec_VerIndex.push_back(nVerIndex);

            //    //判断有没有纹理的索引
            //    if (nSkPos1 + 1 != nSkPos2)
            //    {
            //        std::string stTexIndexStr = stRea.substr(nSkPos1 + 1, nSkPos2 - (nSkPos1 + 1));
            //        int nTexIndex = std::stoi(stTexIndexStr) - 1;
            //        mesh.vec_TexIndex.push_back(nTexIndex);
            //    }
            //    else
            //    {
            //       mesh.vec_TexIndex.push_back(0);//没有索引默认存入0
            //    }
            //    //判断有没有法线的索引
            //    if (nSkPos2 != std::string::npos)
            //    {
            //        std::string stNorIndexStr = stRea.substr(nSkPos2 + 1);//法线数据在最后
            //        int nNorIndex = std::stoi(stNorIndexStr) - 1;
            //        mesh.vec_NorIndex.push_back(nNorIndex);
            //    }
            //    else
            //    {
            //        mesh.vec_NorIndex.push_back(0);
            //    }
            //}

            ss >> skip;
            //先把这一个面的所有顶点组收齐(N个顶点,常见N=3或4)
            struct FaceGroup { int v, t, n; };
            std::vector<FaceGroup> face;
            std::string stRea;
            while (ss >> stRea)
            {
                size_t nSkPos1 = stRea.find('/');
                size_t nSkPos2 = (nSkPos1 == std::string::npos) ? std::string::npos
                    : stRea.find('/', nSkPos1 + 1);
                FaceGroup g;
                g.v = std::stoi(stRea.substr(0, nSkPos1)) - 1;
                //区分v/vt/vn 与 v//vn 两种情况
                if (nSkPos1 != std::string::npos && nSkPos1 + 1 != nSkPos2)
                    g.t = std::stoi(stRea.substr(nSkPos1 + 1, nSkPos2 - nSkPos1 - 1)) - 1;
                else
                    g.t = 0;
                if (nSkPos2 != std::string::npos)
                    g.n = std::stoi(stRea.substr(nSkPos2 + 1)) - 1;
                else
                    g.n = 0;
                face.push_back(g);
            }
            //扇形三角化:N个顶点 -> N-2个三角形,(0,1,2)(0,2,3)...
            for (size_t i = 1; i + 1 < face.size(); ++i)
            {
                const FaceGroup& a = face[0];
                const FaceGroup& b = face[i];
                const FaceGroup& c = face[i + 1];
                mesh.vec_VerIndex.push_back(a.v); mesh.vec_VerIndex.push_back(b.v); mesh.vec_VerIndex.push_back(c.v);
                mesh.vec_TexIndex.push_back(a.t); mesh.vec_TexIndex.push_back(b.t); mesh.vec_TexIndex.push_back(c.t);
                mesh.vec_NorIndex.push_back(a.n); mesh.vec_NorIndex.push_back(b.n); mesh.vec_NorIndex.push_back(c.n);
            }
        }
        //去除注释
        else if (line[0] == '#' || line[0] == 'm' || line[0] == 'u' ||
            line[0] == 'g' || line[0] == 's')
        {
            continue;
        }
    }
    infile.close();

    //计算切线的数据和索引，计算副切线的正负标志
    mesh.vec_TanList.resize(mesh.vec_NorList.size(),0);//切线数据和法线数据一一对应
    vec_stViceTanList.resize(mesh.vec_NorList.size(),0);//副切线也是
    mesh.vec_TanIndex = mesh.vec_NorIndex;//切线和法线的索引应该一致
    vec_nViceTanIndex = mesh.vec_NorIndex;//副切线也是
    Eigen::Vector3f T;//存放切线数据
    Eigen::Vector3f B;//存放副切线数据
    //遍历三角形，计算并存入顶点的切线数据
    for (int index = 0; index < (mesh.vec_VerIndex.size() / 3); ++index)
    {
        //顶点数据
        Eigen::Vector3f VecWor1 = { mesh.vec_VerList[mesh.vec_VerIndex[index * 3] * 3],
                                    mesh.vec_VerList[mesh.vec_VerIndex[index * 3] * 3 + 1],
                                    mesh.vec_VerList[mesh.vec_VerIndex[index * 3] * 3 + 2] };
        Eigen::Vector3f VecWor2 = { mesh.vec_VerList[mesh.vec_VerIndex[index * 3 + 1] * 3],
                                    mesh.vec_VerList[mesh.vec_VerIndex[index * 3 + 1] * 3 + 1],
                                    mesh.vec_VerList[mesh.vec_VerIndex[index * 3 + 1] * 3 + 2] };
        Eigen::Vector3f VecWor3 = { mesh.vec_VerList[mesh.vec_VerIndex[index * 3 + 2] * 3],
                                    mesh.vec_VerList[mesh.vec_VerIndex[index * 3 + 2] * 3 + 1],
                                    mesh.vec_VerList[mesh.vec_VerIndex[index * 3 + 2] * 3 + 2] };
        //纹理数据
        Eigen::Vector2f VecTex1 = { mesh.vec_TexList[mesh.vec_TexIndex[index * 3] * 3] ,
                                    mesh.vec_TexList[mesh.vec_TexIndex[index * 3] * 3 + 1] };
        Eigen::Vector2f VecTex2 = { mesh.vec_TexList[mesh.vec_TexIndex[index * 3 + 1] * 3] ,
                                    mesh.vec_TexList[mesh.vec_TexIndex[index * 3 + 1] * 3 + 1] };
        Eigen::Vector2f VecTex3 = { mesh.vec_TexList[mesh.vec_TexIndex[index * 3 + 2] * 3] ,
                                    mesh.vec_TexList[mesh.vec_TexIndex[index * 3 + 2] * 3 + 1] };
        //uv空间中的u即为切线方向，v即为副切线方向
        //在三角形上，位置变化和 UV 变化是线性对应的，每当uv发生变化，位置也会发生变化
        Eigen::Vector3f e1 = VecWor2 - VecWor1;
        Eigen::Vector3f e2 = VecWor3 - VecWor1;

        Eigen::Vector2f duv1 = VecTex2 - VecTex1;
        Eigen::Vector2f duv2 = VecTex3 - VecTex1;

        //计算面的切线并存入
        T = (e1 * duv2[1] - e2 * duv1[1]) / (duv1[0] * duv2[1] - duv2[0] * duv1[1]);
        //将切线顶点数据存入容器中
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3] * 3] += T[0];//x
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3] * 3 + 1] += T[1];//y
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3] * 3 + 2] += T[2];//z

        mesh.vec_TanList[mesh.vec_TanIndex[index * 3 + 1] * 3] += T[0];
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3 + 1] * 3 + 1] += T[1];
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3 + 1] * 3 + 2] += T[2];

        mesh.vec_TanList[mesh.vec_TanIndex[index * 3 + 2] * 3] += T[0];
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3 + 2] * 3 + 1] += T[1];
        mesh.vec_TanList[mesh.vec_TanIndex[index * 3 + 2] * 3 + 2] += T[2];


        //计算面的副切线并存入
        B = (e2 * duv1[0] - e1 * duv2[0]) / (duv1[0] * duv2[1] - duv2[0] * duv1[1]);
        //将副切线顶点数据存入容器中
        vec_stViceTanList[vec_nViceTanIndex[index * 3] * 3] += B[0];
        vec_stViceTanList[vec_nViceTanIndex[index * 3] * 3 + 1] += B[1];
        vec_stViceTanList[vec_nViceTanIndex[index * 3] * 3 + 2] += B[2];

        vec_stViceTanList[vec_nViceTanIndex[index * 3+1] * 3] += B[0];
        vec_stViceTanList[vec_nViceTanIndex[index * 3+1] * 3 + 1] += B[1];
        vec_stViceTanList[vec_nViceTanIndex[index * 3+1] * 3 + 2] += B[2];

        vec_stViceTanList[vec_nViceTanIndex[index * 3+2] * 3] += B[0];
        vec_stViceTanList[vec_nViceTanIndex[index * 3+2] * 3 + 1] += B[1];
        vec_stViceTanList[vec_nViceTanIndex[index * 3+2] * 3 + 2] += B[2];
    }
    //遍历每个切线数据，让其归一化，正交化,同时把每个顶点的副切线的正负判断存入
    mesh.vec_SecTanSign.resize((vec_stViceTanList.size() / 3), 1);//初始化每个标志值为1
    for (int index = 0; index < (mesh.vec_TanList.size() / 3); ++index)
    {
        Eigen::Vector3f N = { mesh.vec_NorList[index * 3],mesh.vec_NorList[index * 3 + 1],mesh.vec_NorList[index * 3 + 2] };
        B = { vec_stViceTanList[index * 3],vec_stViceTanList[index * 3 + 1],vec_stViceTanList[index * 3 + 2] };
        T = { mesh.vec_TanList[index * 3], mesh.vec_TanList[index * 3 + 1], mesh.vec_TanList[index * 3 + 2] };
        T.normalize(); //归一化
        T = (T - N * (N.dot(T))).normalized();//正交化
        mesh.vec_TanList[index * 3] = T[0];
        mesh.vec_TanList[index * 3 + 1] = T[1];
        mesh.vec_TanList[index * 3 + 2] = T[2];
        //进行副切线的正负判断,主要用T和N求出来的B和实际上计算出来的B点积一下，正为1，负为-1
        if (B.dot((N.cross(T))) < 0) mesh.vec_SecTanSign[index] = -1;
    }
    //将数据转换成顶点属性



}
