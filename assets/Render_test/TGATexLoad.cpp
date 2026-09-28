//TGA贴图读取
#include"TGATexLoad.h"
void ToTGATexLoad(std::vector<unsigned char>& texture,int& width,int& height, const fs::path& texturePath)
{
    //读取贴图
    std::ifstream infile(texturePath, std::ios::binary);//打开文件，以二进制模式读取
    if (!infile.is_open())
    {
        throw std::runtime_error("文件解析失败");
    }
    //从 infile 的第一个字符开始，逐个读到文件末尾，存进 buffer
    std::vector<unsigned char>buffer((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>()); 

    int c;//原通道数
    unsigned char* data = stbi_load_from_memory(buffer.data(), (int)buffer.size(), &width, &height, &c, 4);//从内存里的字节解码图片
    if (!data)
    {
        throw std::runtime_error("文件解析失败");
    }
    texture.resize(width * height*4);
    for (size_t i = 0; i < width * height; ++i)//存入容器中
    {
        texture[i * 4] = data[i * 4];
        texture[i * 4 + 1] = data[i * 4 + 1];
        texture[i * 4 + 2] = data[i * 4 + 2];
        texture[i * 4 + 3] = data[i * 4 + 3];
    }
    stbi_image_free(data);//释放
}
