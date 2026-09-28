# tiny-renderer
个人搭建的渲染器
在Windows单窗口内实现多视口实时渲染

此模型由个人制作

<div align="center"><img width="400" height="300" alt="model1" src="https://github.com/shinnku-kun/tiny-renderer/blob/main/assets/model1.gif" /></div>


以下模型和贴图均由网络下载而来，若有侵权请联系

<div align="center"><img width="400" height="300" alt="model1" src="https://github.com/shinnku-kun/tiny-renderer/blob/main/assets/model2.gif" />
<img width="400" height="300" alt="model1" src="https://github.com/shinnku-kun/tiny-renderer/blob/main/assets/model2_1.gif" />
</div>

功能特性

实现渲染管线整体流程，设计并进行分层，将业务逻辑和渲染逻辑解耦。

渲染实现Blinn-Phong光照，硬阴影；纹理贴图和法线贴图的读取和采样。

仿照ue5中的组件和实例设计了场景管理层。

构建与运行

开发环境:Visual Studio 2022

内部基础运算使用Eigen数学库

打开 render_test.sln -->选 Release -->构建-->将assets文件夹放入.exe同级目录下-->运行



