#include"AcutalRender_CPU.h"
//实际渲染层，这里实现cpu渲染，只实现渲染，不修改任何实例数据
#if defined(_WIN32)
	//cpu实现渲染
	//有参构造
	CPURender::CPURender(ScenceManagement& ScenceManagement)://获取组件实例指针容器的引用
			pnum_CameraInst(*ScenceManagement.GetCameraInst()),
			pnum_LightInst(*ScenceManagement.GetLightInst()),
			pnum_ModelInst(*ScenceManagement.GetModelInst()),
			pnum_TextureInst(*ScenceManagement.GetTexInst()){}
	CPURender:: ~CPURender()
		{ 
			CurrentFrame.Destory();//清理画布 
			LastFrame.Destory(); //清理画布 
			ModelMat.clear();//清理M矩阵
			CameraVP.clear(); //清理摄像机的VP矩阵
			CameraColorBuffer.clear();//清理摄像机颜色缓冲区
			CameraDepthBuffer.clear();//清理摄像机深度缓冲区
			LightVP.clear(); //清理光源的VP矩阵
			ShadowZbuffer.clear(); //清理阴影缓冲区
			PixelColorBuffer.clear();
			PixelNormalBuffer.clear();
			PixelVelocityBuffer.clear();
			FinallyPixelColorBuffer.clear();
		}
	//渲染前初始化需要的参数,包括画布，缓冲区等
	void CPURender::Setup(HWND hWnd,int width,int height)
		{
			CurrentFrame.Create(width, height);//创建当前帧的画布
			LastFrame.Create(width, height);//创建上一帧的画布
			hdcWindow = GetDC(hWnd);//获取窗口DC 
			WindowsWidth = width;WindowsHeight = height;//获取窗口宽高
			ModelMat.resize(pnum_ModelInst.size());//设置模型M矩阵的数量
			ModelInverseTransposeMat.resize(pnum_ModelInst.size());//设置模型逆转置M矩阵的数量
			CameraVP.resize(pnum_CameraInst.size());//设置摄像机VP矩阵的数量
			CameraColorBuffer.resize(pnum_CameraInst.size());//设置摄像机颜色缓冲区的数量
			CameraDepthBuffer.resize(pnum_CameraInst.size());//设置摄像机深度缓冲区的数量
			ResetCameraBuffer();//重置摄像机的深度缓冲区和颜色缓冲区
			LightVP.resize(pnum_LightInst.size());//设置光源位置的VP矩阵数量
			ShadowZbuffer.resize(pnum_LightInst.size());//设置阴影缓冲区的数量
			ResetShadow();//重置置每个光源的阴影深度缓冲区大小和值
			//设置后处理的缓冲区大小
			PixelColorBuffer.resize(width * height,0);//设置后处理颜色缓冲区，初始为0
			PixelNormalBuffer.resize(width * height);
			PixelVelocityBuffer.resize(width * height);
			FinallyPixelColorBuffer.resize(width * height);
		}
	//前向渲染管线
	void CPURender::ForwardRender()
	{
		GetModelM();//获取所有模型的M矩阵
		GetCameraVP();//获取所有摄像机的VP矩阵
		GetLightVP();//获取所有光源位置的VP矩阵,如果光源位置未动，则记录一次就足够了
		GetShadowMap();//获取所有阴影贴图，阴影贴图也是，只要光源没有发生变化，则不需要每帧执行
		//前向渲染实现
		for (int j = 0; j < pnum_ModelInst.size(); ++j)//遍历模型实例
		{
			if (pnum_ModelInst[j]->IsRender)//确认渲染此模型	
			{
				Model = pnum_ModelInst[j]->ModelCom->GetModel();//获取模型组件
				if (pnum_ModelInst[j]->UseDefault)//如果启用模型默认值
				{
					ModelPara.MeshTexInstIndex = Model->DefaultMeshTexInstIndex;
				}
				else//使用实例值
				{
					ModelPara.MeshTexInstIndex = pnum_ModelInst[j]->InstMeshTexInstIndex;
				}
				for (int k = 0; k < Model->pvec_Mesh->size(); ++k)//遍历模块
				{
					TextureInst = pnum_TextureInst[ModelPara.MeshTexInstIndex[k]];//获取模块对应的材质实例指针
					Texture = TextureInst->TexCom->GetTex();//获取材质组件
					if (TextureInst->UseDefault)//如果启用材质默认值
					{
						TexturePara.LightingMode = Texture->DefaultLightingMode;//光照模式
						TexturePara.Color = Texture->DefaultColor;//基础颜色
						TexturePara.Emissive = Texture->DefaultEmissive;//设置基础自发光颜色
						TexturePara.Metallic = Texture->DefaultMetallic;//金属度
						TexturePara.Roughness = Texture->DefaultRoughness;//粗糙度
						TexturePara.Ao = Texture->DefaultAo;//环境光遮蔽
						TexturePara.Opacity = Texture->DefaultOpacity;//不透明度
						TexturePara.Height = Texture->DefaultHeight;//高度位移
						TexturePara.Ior = Texture->DefaultIor;//折射率
						TexturePara.Specular = Texture->DefaultSpecular;//高光强度
						TexturePara.Clearcoat = Texture->DefaultClearcoat;//清漆层
						TexturePara.ClearcoatRoughness = Texture->DefaultClearcoatRoughness;//清漆粗糙度
						TexturePara.Sheen = Texture->DefaultSheen;//光泽(布料)
						TexturePara.SheenColor = Texture->DefaultSheenColor;//光泽颜色
						TexturePara.Anisotropy = Texture->DefaultAnisotropy;//各向异性
						TexturePara.AnisotropyRotation = Texture->DefaultAnisotropyRotation;//各项异性方向
						TexturePara.Subsurface = Texture->DefaultSubsurface;//次表面散射
						TexturePara.SubsurfaceColor = Texture->DefaultSubsurfaceColor;//次表面颜色
						TexturePara.Transmission = Texture->DefaultTransmission;//透射(玻璃)
						TexturePara.Thickness = Texture->DefaultThickness;//厚度
						TexturePara.AlphaCutoff = Texture->DefaultAlphaCutoff;//Alpha 裁剪阈值
						TexturePara.BlendMode = Texture->DefaultBlendMode;//混合模式
						TexturePara.DoubleSided = Texture->DefaultDoubleSided;//是否双面渲染
						TexturePara.Transparent = Texture->DefaultTransmission;//是否透明
					}
					else//使用实例值
					{
						TexturePara.LightingMode = TextureInst->InstLightingMode;//光照模式
						TexturePara.Color = TextureInst->InstColor;//基础颜色
						TexturePara.Emissive = TextureInst->InstEmissive;//设置基础自发光颜色
						TexturePara.Metallic = TextureInst->InstMetallic;//金属度
						TexturePara.Roughness = TextureInst->InstRoughness;//粗糙度
						TexturePara.Ao = TextureInst->InstAo;//环境光遮蔽
						TexturePara.Opacity = TextureInst->InstOpacity;//不透明度
						TexturePara.Height = TextureInst->InstHeight;//高度位移
						TexturePara.Ior = TextureInst->InstIor;//折射率
						TexturePara.Specular = TextureInst->InstSpecular;//高光强度
						TexturePara.Clearcoat = TextureInst->InstClearcoat;//清漆层
						TexturePara.ClearcoatRoughness = TextureInst->InstClearcoatRoughness;//清漆粗糙度
						TexturePara.Sheen = TextureInst->InstSheen;//光泽(布料)
						TexturePara.SheenColor = TextureInst->InstSheenColor;//光泽颜色
						TexturePara.Anisotropy = TextureInst->InstAnisotropy;//各向异性
						TexturePara.AnisotropyRotation = TextureInst->InstAnisotropyRotation;//各项异性方向
						TexturePara.Subsurface = TextureInst->InstSubsurface;//次表面散射
						TexturePara.SubsurfaceColor = TextureInst->InstSubsurfaceColor;//次表面颜色
						TexturePara.Transmission = TextureInst->InstTransmission;//透射(玻璃)
						TexturePara.Thickness = TextureInst->InstThickness;//厚度
						TexturePara.AlphaCutoff = TextureInst->InstAlphaCutoff;//Alpha 裁剪阈值
						TexturePara.BlendMode = TextureInst->InstBlendMode;//混合模式
						TexturePara.DoubleSided = TextureInst->InstDoubleSided;//是否双面渲染
						TexturePara.Transparent = TextureInst->InstTransparent;//是否透明
					}
					//解引用访问内部成员,遍历三角形，进行渲染
					for (int f = 0; f < (*Model->pvec_Mesh)[k].vec_VerIndex.size() / 3; ++f)
					{
						//图元装配(这里略过)
						//顶点着色器输入准备阶段
						//纹理
						Eigen::Vector2f Tex1 = {
								(*Model->pvec_Mesh)[k].vec_TexList[(*Model->pvec_Mesh)[k].vec_TexIndex[f * 3] * 3],
								(*Model->pvec_Mesh)[k].vec_TexList[(*Model->pvec_Mesh)[k].vec_TexIndex[f * 3] * 3 + 1] };
						Eigen::Vector2f Tex2 = {
								(*Model->pvec_Mesh)[k].vec_TexList[(*Model->pvec_Mesh)[k].vec_TexIndex[f * 3 + 1] * 3],
								(*Model->pvec_Mesh)[k].vec_TexList[(*Model->pvec_Mesh)[k].vec_TexIndex[f * 3 + 1] * 3 + 1] };
						Eigen::Vector2f Tex3 = {
								(*Model->pvec_Mesh)[k].vec_TexList[(*Model->pvec_Mesh)[k].vec_TexIndex[f * 3 + 2] * 3],
								(*Model->pvec_Mesh)[k].vec_TexList[(*Model->pvec_Mesh)[k].vec_TexIndex[f * 3 + 2] * 3 + 1] };
						//世界坐标，用于获取屏幕空间像素的实际顶点位置
						Eigen::Vector4f Wor1 = {
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3] * 3],
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3] * 3 + 2],1.f };
						Eigen::Vector4f Wor2 = {
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3 + 1] * 3],
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3 + 1] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3 + 1] * 3 + 2],1 };
						Eigen::Vector4f Wor3 = {
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3 + 2] * 3],
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3 + 2] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3 + 2] * 3 + 2],1 };
						//变换到世界空间
						Wor1 = ModelMat[j] * Wor1;
						Wor2 = ModelMat[j] * Wor2;
						Wor3 = ModelMat[j] * Wor3;
						//法线
						Eigen::Vector4f Nor1 = {
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3] * 3],
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3] * 3 + 2],1.f };
						Eigen::Vector4f Nor2 = {
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3 + 1] * 3],
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3 + 1] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3 + 1] * 3 + 2],1.f };
						Eigen::Vector4f Nor3 = {
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3 + 2] * 3],
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3 + 2] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_NorList[(*Model->pvec_Mesh)[k].vec_NorIndex[f * 3 + 2] * 3 + 2],1.f };
						//变换到世界空间，法线需要逆转置矩阵转换
						Nor1 = ModelInverseTransposeMat[j] * Nor1;
						Nor2 = ModelInverseTransposeMat[j] * Nor2;
						Nor3 = ModelInverseTransposeMat[j] * Nor3;
						//切线
						Eigen::Vector4f Tan1 = {
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3] * 3],
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3] * 3 + 2],1 };
						Eigen::Vector4f Tan2 = {
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 1] * 3],
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 1] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 1] * 3 + 2],1 };
						Eigen::Vector4f Tan3 = {
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 2] * 3],
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 2] * 3 + 1],
								(*Model->pvec_Mesh)[k].vec_TanList[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 2] * 3 + 2],1 };
						//变换到世界空间
						Tan1 = ModelMat[j] * Tan1;
						Tan2 = ModelMat[j] * Tan2;
						Tan3 = ModelMat[j] * Tan3;
						//副切线的正负标志
						float sign1 = (*Model->pvec_Mesh)[k].vec_SecTanSign[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3]];
						float sign2 = (*Model->pvec_Mesh)[k].vec_SecTanSign[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 1]];
						float sign3 = (*Model->pvec_Mesh)[k].vec_SecTanSign[(*Model->pvec_Mesh)[k].vec_TanIndex[f * 3 + 2]];
						for (int i = 0; i < pnum_CameraInst.size(); ++i)//遍历摄像机实例
						{
							if (pnum_CameraInst[i]->IsRender)//确认渲染此摄像机
							{
								Camera = pnum_CameraInst[i]->CameraCom->GetCamera();//获取摄像机组件指针
								if (pnum_CameraInst[i]->UseDefault)//如果启用默认值
								{
									CameraPara.CameraProjection = Camera->DefaultCameraProjection;//摄像机投影类型
									CameraPara.Location = Camera->DefaultLocation;//位置
									CameraPara.Width = Camera->DefaultWidth;//宽
									CameraPara.Height = Camera->DefaultHeight;//高
									CameraPara.X = Camera->DefaultX; CameraPara.Y = Camera->DefaultY;//起始位置
								}
								else//使用实例值
								{
									CameraPara.CameraProjection = pnum_CameraInst[i]->InstCameraProjection;//投影类型
									CameraPara.Location = pnum_CameraInst[i]->InstLocation;//位置
									CameraPara.Width = pnum_CameraInst[i]->InstWidth;//宽
									CameraPara.Height = pnum_CameraInst[i]->InstHeight;//高
									CameraPara.X = pnum_CameraInst[i]->InstX; CameraPara.Y = pnum_CameraInst[i]->InstY;//起始位置
								}
								//顶点着色阶段
								Eigen::Vector4f Ver1 = CameraVP[i].ProMat * CameraVP[i].ViewMat * Wor1;
								Eigen::Vector4f Ver2 = CameraVP[i].ProMat * CameraVP[i].ViewMat * Wor2;
								Eigen::Vector4f Ver3 = CameraVP[i].ProMat * CameraVP[i].ViewMat * Wor3;
								//先计算w的倒数
								float cw1 = 1 / Ver1[3];
								float cw2 = 1 / Ver2[3];
								float cw3 = 1 / Ver3[3];
								//透视除法阶段，如果摄像机是正交,则不做，同时后面的透视矫正插值也需要换成线性插值，不过因为计算方面没影响就不换了
								if (CameraPara.CameraProjection == 0)//0为透视投影类型
								{
									Ver1[0] = Ver1[0] * cw1; Ver1[1] = Ver1[1] * cw1; Ver1[2] = Ver1[2] * cw1;
									Ver2[0] = Ver2[0] * cw2; Ver2[1] = Ver2[1] * cw2; Ver2[2] = Ver2[2] * cw2;
									Ver3[0] = Ver3[0] * cw3; Ver3[1] = Ver3[1] * cw3; Ver3[2] = Ver3[2] * cw3;
								}
								//裁剪阶段
								if (Ver1[3] <= 0 || Ver2[3] <= 0 || Ver3[3] <= 0) { continue; }//对w分量进行判断，如果w分量为负数，说明其位置在摄像机后面，不进行绘制
								//视锥裁剪阶段，即将视锥外的顶点剔除，如果三个顶点都在视锥体外，直接丢弃
								if (Ver1[0] < -1 && Ver2[0] < -1 && Ver3[0] < -1)continue;
								if (Ver1[0] > 1 && Ver2[0] > 1 && Ver3[0] > 1)continue;
								if (Ver1[1] < -1 && Ver2[1] < -1 && Ver3[1] < -1)continue;
								if (Ver1[1] > 1 && Ver2[1] > 1 && Ver3[1] > 1)continue;
								if (Ver1[2] < -1 && Ver2[2] < -1 && Ver3[2] < -1)continue;
								if (Ver1[2] > 1 && Ver2[2] > 1 && Ver3[2] > 1)continue;
								//如果部分在视锥体外，则需要新的裁剪算法

								//视口变换阶段，变换到摄像机屏幕上
								float x1 = (Ver1[0] + 1) * 0.5 * (CameraPara.Width);
								float y1 = (1 - Ver1[1]) * 0.5 * (CameraPara.Height);
								float z1 = (Ver1[2] + 1) * 0.5;

								float x2 = (Ver2[0] + 1) * 0.5 * (CameraPara.Width);
								float y2 = (1 - Ver2[1]) * 0.5 * (CameraPara.Height);
								float z2 = (Ver2[2] + 1) * 0.5;

								float x3 = (Ver3[0] + 1) * 0.5 * (CameraPara.Width);
								float y3 = (1 - Ver3[1]) * 0.5 * (CameraPara.Height);
								float z3 = (Ver3[2] + 1) * 0.5;
								//光栅化阶段
								float erase = ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1));
								float CDerase = 1 / erase;//三角形面积倒数
								if (std::abs(erase) < 1e-6f) continue;
								//包围盒
								int minX = std::max(std::min(std::min(x1, x2), x3), 0.f);
								int maxX = std::min(std::max(std::max(x1, x2), x3), float(CameraPara.Width - 1));
								int minY = std::max(std::min(std::min(y1, y2), y3), 0.f);
								int maxY = std::min(std::max(std::max(y1, y2), y3), float(CameraPara.Height - 1));
								#pragma omp parallel for collapse(2)
								for (int x = minX; x <= maxX; ++x)
								{
									for (int y = minY; y <= maxY; ++y)
									{
										//获取重心坐标分量
										float C1 = ((x2 - float(x)) * (y3 - float(y)) - (y2 - float(y)) * (x3 - float(x))) * CDerase;
										float C2 = ((float(x) - x1) * (y3 - y1) - (float(y) - y1) * (x3 - x1)) * CDerase;
										float C3 = ((x2 - x1) * (float(y) - y1) - (y2 - y1) * (float(x) - x1)) * CDerase;
										//像素是否在三角形内 
										if (C1 >= 0 && C2 >= 0 && C3 >= 0)
										{
											//插值1/w
											float invW = C1 * cw1 + C2 * cw2 + C3 * cw3;
											//插值深度,深度可以不进行深度矫正插值计算，因为深度没有收到透视除法的影响
											float DepthInter = C1 * z1 + C2 * z2 + C3 * z3;

											//插值世界坐标(从顶点转成像素)
											Eigen::Vector4f WorldInter = (C1 * cw1 * Wor1 + C2 * cw2 * Wor2 + C3 * cw3 * Wor3) / invW;
											//插值法线
											Eigen::Vector3f NoverW = Nor1.head<3>() * (C1 * cw1) + Nor2.head<3>() * (C2 * cw2) + Nor3.head<3>() * (C3 * cw3);
											Eigen::Vector3f NormalInter = NoverW / invW;
											NormalInter.normalize();//法线归一化
											//插值纹理
											Eigen::Vector2f ToverW = Tex1 * (C1 * cw1) + Tex2 * (C2 * cw2) + Tex3 * (C3 * cw3);
											Eigen::Vector2f TextureInter = ToverW / invW;
											//插值副切线标志
											float SignInter = sign1 * (C1 * cw1) + sign2 * (C2 * cw2) + sign3 * (C3 * cw3);
											SignInter /= invW; SignInter = (SignInter >= 0) ? 1 : -1;
											//插值TBN矩阵(实际就是插值一下切线，因为法线已经插值好了，副切线可以通过计算求出来)
											Eigen::Vector3f NInter = NormalInter;
											Eigen::Vector3f TInter = Tan1.head<3>() * (C1 * cw1) + Tan2.head<3>() * (C2 * cw2) + Tan3.head<3>() * (C3 * cw3);
											TInter /= invW; TInter.normalize();//归一化
											TInter = TInter - NormalInter * (NormalInter.dot(TInter));//正交化
											//组装TBN矩阵，TBN矩阵是为了把切线空间的法线转到世界空间中，所以要的是法线在每个世界轴上的分量的和，即将xyz映射到世界XYZ轴上
											Eigen::Vector3f BInter = NInter.cross(TInter) * SignInter;//副切线
											Eigen::Matrix3f TBN; TBN << TInter, BInter, NInter;
											//到此是光栅化，也就是插值了需要的像素属性，实际渲染器中光栅化流程是固定的
											//像素着色器阶段
											switch (TexturePara.LightingMode)//根据光照模式来确定所需参数，用来计算像素颜色
											{
												//光照模式为Blinn-Phong，此光照模型中镜面反射不带有颜色
											case 0:
											{
												Eigen::Vector3f ReNormal = NormalInter;//最终计算的结果在没有法线贴图前直接使用模型自带法线
												Eigen::Vector3f CamToVex = (CameraPara.Location - WorldInter.head<3>()).normalized();//摄像机朝顶点的视线(结果朝向摄像机)
												float Diff = 0.5; Eigen::Vector3f DiffColor = { 255,255,255 };//默认漫反射系数和颜色
												float Ecoe = 64.0f; float Mirr = 0.1;//默认镜面反射指数和系数
												int BlinnR = 0; int BlinnG = 0; int BlinnB = 0;//默认像素RGB颜色
												switch (Texture->TexType)//根据材质类型来决定计算颜色的方式，材质通过各种不同计算方式和数据来影响计算光照模型所需要的参数
												{
													//基础材质类型,只修改颜色
												case 0:
												{
													//比较深度值，其实应该会根据材质的透明属性来决定是否写入以及透明效果对颜色缓冲区的影响,后续需要补充
													//深度值会直接影响模型之间的遮挡关系
													//如果是地面一类的模型可以通过关闭深度写入以及增加深度偏移来防止其出现遮挡问题，或者再加一层按斜率偏移
													if (DepthInter > (CameraDepthBuffer[i])[y * CameraPara.Width + x])continue;
													(CameraDepthBuffer[i])[y * CameraPara.Width + x] = DepthInter;//如果在前面(就是深度小的),存入深度缓冲区

													DiffColor = TexturePara.Color;//获取材质基础颜色
													//颜色贴图读取
													if (Texture->ColorMapWidth > 0 && Texture->ColorMapHeight > 0)//颜色贴图大小不为0
													{
														//颜色贴图
														int Texu = (TextureInter[0]) * (Texture->ColorMapWidth);
														int Texv = (1 - TextureInter[1]) * (Texture->ColorMapHeight);
														//范围钳制
														Texu = std::clamp(Texu, 0, Texture->ColorMapWidth - 1);
														Texv = std::clamp(Texv, 0, Texture->ColorMapHeight - 1);
														float R = (*Texture->pvec_ColorMap)[(Texv * Texture->ColorMapWidth + Texu) * 4];
														float G = (*Texture->pvec_ColorMap)[(Texv * Texture->ColorMapWidth + Texu) * 4 + 1];
														float B = (*Texture->pvec_ColorMap)[(Texv * Texture->ColorMapWidth + Texu) * 4 + 2];
														//根据uv坐标采样漫反射颜色
														DiffColor = { R,G,B }; DiffColor *= 2;//加一倍，更加鲜艳一些
													}
													//法线贴图读取
													if (Texture->NormalMapWidth > 0 && Texture->NormalMapHeight > 0)//法线贴图大小不为0
													{
														//法线贴图
														int NorStu = (TextureInter[0]) * (Texture->NormalMapWidth);
														int NorStv = (1 - TextureInter[1]) * (Texture->NormalMapHeight);
														NorStu = std::clamp(NorStu, 0, Texture->NormalMapWidth - 1);
														NorStv = std::clamp(NorStv, 0, Texture->NormalMapHeight - 1);
														float NorX = (*Texture->pvec_NormalMap)[(NorStv * Texture->NormalMapWidth + NorStu) * 4];
														float NorY = (*Texture->pvec_NormalMap)[(NorStv * Texture->NormalMapWidth + NorStu) * 4 + 1];
														float NorZ = (*Texture->pvec_NormalMap)[(NorStv * Texture->NormalMapWidth + NorStu) * 4 + 2];
														//根据uv坐标采样法线,规范化到-1到1之间
														Eigen::Vector3f NorSticker =
														{ (NorX / 255.f) * 2.f - 1.f,
															(NorY / 255.f) * 2.f - 1.f,
															(NorZ / 255.f) * 2.f - 1.f
														};
														//使用TBN矩阵将法线贴图中的法线转到世界空间中并存入最终的法线向量中
														ReNormal = TBN * NorSticker;
													}
													for (int lightindex = 0; lightindex < pnum_LightInst.size(); ++lightindex)//遍历光源实例，计算各种参数
													{
														//光源VP矩阵已经计算出来了，所以直接用就好
														if (pnum_LightInst[lightindex]->IsRender)//是否渲染
														{
															//阴影实现,将世界坐标转到光源的坐标系下
															Eigen::Vector4f ParaClip = LightVP[lightindex].ProMat * LightVP[lightindex].ViewMat * WorldInter;
															//这里要通过光源类型分别进行计算，得到标准空间向量和光照向此像素的方向
															Eigen::Vector3f ParaNDC = { 1,1,1 }; Eigen::Vector3f SunLight = { 1,1,1 };
															switch (pnum_LightInst[lightindex]->LightCom->GetLightType())
															{
															case 0://平行光，直接得到标准空间向量和光源照射方向
															{
																ParaNDC = { ParaClip[0],ParaClip[1],ParaClip[2] };//直接得到标准空间向量
																if (pnum_LightInst[lightindex]->UseDefault)//使用默认值
																{
																	SunLight = (pnum_LightInst[lightindex]->LightCom->GetLight()->DefaultDirection).normalized();
																}
																else//使用实例值
																{
																	SunLight = pnum_LightInst[lightindex]->InstDirection.normalized();
																}
																break;
															}
															}
															Eigen::Vector2f ParaUV = { (ParaNDC[0] + 1) * 0.5,(1 - ParaNDC[1]) * 0.5 };//采样位置,注意y轴的值也需要翻转
															float ParaDepth = (ParaNDC[2] + 1) * 0.5;//此像素的平行光投影深度
															float Shadow = 1.0;//基础阴影强度，就是正常无阴影
															//判断是否在范围内
															if (ParaUV[0] <= 1 && ParaUV[0] >= 0 && ParaUV[1] <= 1 && ParaUV[1] >= 0 && ParaDepth >= 0 && ParaDepth <= 1)
															{
																//获取实际采样位置
																int ParaU = ParaUV[0] * ShadowWidth;
																int ParaV = ParaUV[1] * ShadowHeight;
																//钳制范围
																ParaU = std::clamp(ParaU, 0, (int)ShadowWidth - 1);
																ParaV = std::clamp(ParaV, 0, (int)ShadowHeight - 1);
																float ParaSampDepth = ShadowZbuffer[lightindex][ParaV * ShadowWidth + ParaU];//采样深度
																//float Bias = 0.005;//离散值
																float Bias = std::max(0.05 * (1 - (SunLight).dot(ReNormal)), 0.005);//slope-scaled,后续需要了解
																//对深度进行比较，实际深度如果大于采样深度，那么产生阴影
																if (ParaDepth - Bias > ParaSampDepth) { Shadow = 0.5; }//设置阴影强度
															}
															//Diff = std::max(0.f, (SunLight).dot(ReNormal)) * Shadow;//漫反射系数
															Diff = ((SunLight).dot(ReNormal) + 1.) * 0.5; Diff *= Diff * Shadow;//漫反射系数,半兰伯特（Half-Lambert）尝试
															Mirr = std::max(0.f, std::pow(((SunLight + CamToVex).normalized()).dot(ReNormal), Ecoe)) * Shadow;//镜面反射系数

															//累加光照下的颜色
															BlinnR += int(std::clamp(DiffColor[0] * Diff + 255.f * Mirr, 0.f, 255.f));
															BlinnG += int(std::clamp(DiffColor[1] * Diff + 255.f * Mirr, 0.f, 255.f));
															BlinnB += int(std::clamp(DiffColor[2] * Diff + 255.f * Mirr, 0.f, 255.f));
														}
													}
													break;
												}
												}
												//钳制最终像素颜色在0到255范围内
												BlinnR = std::clamp(BlinnR, 0, 255); BlinnG = std::clamp(BlinnG, 0, 255); BlinnB = std::clamp(BlinnB, 0, 255);
												//根据不同材质类型以不同方式进行计算，得到并设置像素颜色
												CameraColorBuffer[i][y * CameraPara.Width + x] = (0 << 24) | BlinnR << 16 | BlinnG << 8 | BlinnB;
												break;
											}
											//光照模式为PBR
											case 1:
											{
												//声明PBR计算所需参数

												//根据材质类型分别采用不同方式进行参数计算

												//最终得到像素颜色
												break;
											}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		PostProcess();//后处理
		Draw();//往画布上绘制颜色
		ResetCameraBuffer();
		ResetShadow();//重置阴影缓冲
		FrameCount++;//帧计数器
	}
	//延迟渲染管线(延迟渲染天然包含前向渲染，因为需要渲染透明物体)
	void CPURender::DeferredRender()
		{
			//获取所有摄像机的MVP矩阵
			GetCameraVP();
			//获取所有光源位置的MVP矩阵
			GetLightVP();
			//获取所有阴影贴图
			GetShadowMap();
			//延迟渲染实现，根据透明属性来决定是使用前向还是延迟



			//后处理
			PostProcess();
			//往画布上绘制颜色
			Draw();
			//重置阴影缓冲
			ResetShadow();
		}
	//光线追踪实现
	void CPURender::RayTracing()
		{
		
		
		}
	//混合渲染管线(混合光栅化和光线追踪)
	void CPURender::HybridRender()
		{
			
		}
	//获取所有模型的M矩阵和其逆矩阵
	void CPURender::GetModelM()
		{
			//遍历模型
			for (int i = 0; i < pnum_ModelInst.size(); ++i)
			{
				//确认渲染此模型
				if (pnum_ModelInst[i]->IsRender)
				{	
					Model = pnum_ModelInst[i]->ModelCom->GetModel();//获取模型组件
					if (pnum_ModelInst[i]->UseDefault)//确认使用默认值
					{
						ModelPara.Location = Model->DefaultLocation;//模型位置
						ModelPara.Rotate = Model->DefaultRotate;//模型旋转角度
						ModelPara.Zoom = Model->DefaultZoom;//模型缩放大小
					}
					else//使用实例值
					{
						ModelPara.Location = pnum_ModelInst[i]->InstLocation;//模型位置
						ModelPara.Rotate = pnum_ModelInst[i]->InstRotate;//模型旋转角度
						ModelPara.Zoom = pnum_ModelInst[i]->InstZoom;//模型缩放大小
					}
					switch (Model->ModelType)
					{
						case 0://静态模型
						{
							ModelMat[i] = GetModelMat(ModelPara.Location, ModelPara.Rotate, ModelPara.Zoom);//传入参数
							ModelInverseTransposeMat[i] = ModelMat[i].inverse().transpose();//逆转置
							break;
						}
					}
				}
			}
		}
	//获取所有摄像机VP矩阵
	void CPURender::GetCameraVP()
		{
			//遍历摄像机实例，计算每个摄像机的mvp矩阵
			for (int i = 0; i < pnum_CameraInst.size(); ++i)
			{   
				//如果此摄像机实例进行渲染
				if (pnum_CameraInst[i]->IsRender)
				{
					Camera = pnum_CameraInst[i]->CameraCom->GetCamera();//获取摄像机组件
					if (pnum_CameraInst[i]->UseDefault)//如果使用默认值
					{
						CameraPara.CameraProjection = Camera->DefaultCameraProjection;//投影类型
						CameraPara.Location=Camera->DefaultLocation;//摄像机组件世界位置
						CameraPara.LookLocation=Camera->DefaultLookLocation;//摄像机组件观察位置
						CameraPara.UpDir=Camera->DefaultUpDir;//摄像机组件朝上向量
						CameraPara.Fovy = Camera->DefaultFovy;//视野
						CameraPara.Near = Camera->DefaultNear;//近裁剪面
						CameraPara.Far = Camera->DefaultFar;//远裁剪面
						CameraPara.Width = Camera->DefaultWidth;//渲染宽度
						CameraPara.Height = Camera->DefaultHeight;//渲染高度
						CameraPara.X = Camera->DefaultX; CameraPara.Y = Camera->DefaultY;//渲染起始点
					}
					else//如果不使用默认值，则使用实例中设置的值
					{
						CameraPara.CameraProjection = pnum_CameraInst[i]->InstCameraProjection;//投影类型
						CameraPara.Location = pnum_CameraInst[i]->InstLocation;//摄像机组件世界位置
						CameraPara.LookLocation = pnum_CameraInst[i]->InstLookLocation;//摄像机组件观察位置
						CameraPara.UpDir = pnum_CameraInst[i]->InstUpDir;//摄像机组件朝上向量
						CameraPara.Fovy = pnum_CameraInst[i]->InstFovy;//视野
						CameraPara.Near = pnum_CameraInst[i]->InstNear;//近裁剪面
						CameraPara.Far = pnum_CameraInst[i]->InstFar;//远裁剪面
						CameraPara.Width = pnum_CameraInst[i]->InstWidth;//渲染宽度
						CameraPara.Height = pnum_CameraInst[i]->InstHeight;//渲染高度
						CameraPara.X = pnum_CameraInst[i]->InstX; CameraPara.Y = pnum_CameraInst[i]->InstY;//渲染起始点
					}	
					switch (Camera->CameraType)//根据类型分别计算
					{	
						case 0://主摄像机
						{
							//计算视图矩阵
							CameraVP[i].ViewMat = GetViewMat(CameraPara.Location, CameraPara.LookLocation, CameraPara.UpDir);//传入参数
							//计算投影矩阵
							if (CameraPara.CameraProjection == 0)//根据类型来决定是透视还是正交，0为透视
							{
								CameraVP[i].ProMat = GetPerProMat(CameraPara.Fovy, CameraPara.Width / CameraPara.Height, CameraPara.Near, CameraPara.Far);//传入参数
							}
							else//正交投影
							{
								CameraVP[i].ProMat = GetOrtProMat(CameraPara.X, CameraPara.Width + CameraPara.X, CameraPara.Y, CameraPara.Height + CameraPara.Y, -CameraPara.Near, -CameraPara.Far);//传入参数
							}
							break;
						}
					}
				}
			}
		}
	//获取所有光源VP矩阵
	void CPURender::GetLightVP()
		{	
			//遍历所有光源，计算光源矩阵
			for (int i = 0; i < pnum_LightInst.size(); ++i)
			{
				//如果此光源实例进行渲染
				if (pnum_LightInst[i]->IsRender)
				{
					Light = pnum_LightInst[i]->LightCom->GetLight();//获取光源组件
					if (pnum_LightInst[i]->UseDefault)//使用默认值
					{
						LightPara.BoxCenter = Light->DefaultBoxCenter;//包围盒中心
						LightPara.BoxRadius = Light->DefaultBoxRadius;//包围盒半径
						LightPara.Location = Light->DefaultLocation;//位置
						LightPara.Direction = Light->DefaultDirection;//光源的方向信息，且为指向光源的方向
						LightPara.UpDir = Light->DefaultUpDir;//平行光朝上向量
						LightPara.Near = Light->DefaultNear;//近裁剪面
						LightPara.Far = Light->DefaultFar;//远裁剪面
					}
					else//如果不使用默认值，则使用实例中设置的值
					{
						LightPara.BoxCenter = pnum_LightInst[i]->InstBoxCenter;//包围盒中心
						LightPara.BoxRadius = pnum_LightInst[i]->InstBoxRadius;//包围盒半径
						LightPara.Location = pnum_LightInst[i]->InstLocation;//位置
						LightPara.Direction = pnum_LightInst[i]->InstDirection;//光源的方向信息，且为指向光源的方向
						LightPara.UpDir = pnum_LightInst[i]->InstUpDir;//光源朝上向量
						LightPara.Near = pnum_LightInst[i]->InstNear;//近裁剪面
						LightPara.Far = pnum_LightInst[i]->InstFar;//远裁剪面
					}
					switch (Light->LightType)//根据类型来计算
					{	
						case 0://平行光，因为是平行光所以只需要传入包围盒中心，半径和光源的方向
						{	
							LightPara.Direction.normalize();//归一化光照方向
							//因为这里存的光照方向是指向光源的方向，所以需要加，也就是从中心朝光源位置走,设置光源位置距离中心位置有直径远
							LightPara.Location = LightPara.BoxCenter + LightPara.Direction * LightPara.BoxRadius * 2;
							//计算视图矩阵
							LightVP[i].ViewMat = GetViewMat(LightPara.Location,LightPara.BoxCenter,LightPara.UpDir);//传入参数
							//计算正交投影矩阵(因为光源的矩阵类型和光源类型是相对应的)
							LightVP[i].ProMat = GetOrtProMat(-LightPara.BoxRadius,LightPara.BoxRadius,-LightPara.BoxRadius, LightPara.BoxRadius,LightPara.Near,LightPara.Far);//传入参数
							break;
						}
					}
				}
			}
		}
	//获取所有阴影深度贴图
	void CPURender::GetShadowMap()
		{
			for (int i = 0; i < pnum_LightInst.size(); ++i)//遍历光源
			{
				if (pnum_LightInst[i]->IsRender)//确认渲染此光源
				{
					//获取光源类型，不需要获取光源以及是否启用默认值，因为光源的信息基本就是为了计算VP矩阵的，而不是计算深度贴图
					int LightType = pnum_LightInst[i]->LightCom->GetLightType();
					for (int j = 0; j < pnum_ModelInst.size(); ++j)//遍历模型
					{
						if (pnum_ModelInst[j]->IsRender)//确认渲染此模型
						{						
							Model = pnum_ModelInst[j]->ModelCom->GetModel();//获取此模型
							for (int k = 0; k < Model->pvec_Mesh->size(); ++k)//遍历模块,进行简化版的渲染实现
							{
								//解引用访问内部成员,遍历三角形，从光源方向获取深度并存入阴影缓冲区中
								for (int f = 0; f < (*Model->pvec_Mesh)[k].vec_VerIndex.size() / 3; ++f)
								{
									//图元装配(这里略过，直接给到其次坐标)
									//顶点着色
									Eigen::Vector4f Ver1 = {
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3] * 3],
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3] * 3+1],
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3] * 3+2],
										1.f};
									Eigen::Vector4f Ver2 = {
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3+1] * 3],
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3+1] * 3 + 1],
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3+1] * 3 + 2],
										1.f};
									Eigen::Vector4f Ver3 = {
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3+2] * 3],
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3+2] * 3 + 1],
										(*Model->pvec_Mesh)[k].vec_VerList[(*Model->pvec_Mesh)[k].vec_VerIndex[f * 3+2] * 3 + 2],
										1.f};
									Ver1 = LightVP[i].ProMat * LightVP[i].ViewMat * ModelMat[j] * Ver1;
									Ver2 = LightVP[i].ProMat * LightVP[i].ViewMat * ModelMat[j] * Ver2;
									Ver3 = LightVP[i].ProMat * LightVP[i].ViewMat * ModelMat[j] * Ver3;
									switch (LightType)//根据光源类型判断计算方式
									{	
										//平行光，不用做透视除法
										case 0:
										{
											//裁剪
											if (Ver1[0] < -1 && Ver2[0] < -1 && Ver3[0] < -1)continue;
											if (Ver1[0] > 1 && Ver2[0] > 1 && Ver3[0] > 1)continue;
											if (Ver1[1] < -1 && Ver2[1] < -1 && Ver3[1] < -1)continue;
											if (Ver1[1] > 1 && Ver2[1] > 1 && Ver3[1] > 1)continue;
											if (Ver1[2] < -1 && Ver2[2] < -1 && Ver3[2] < -1)continue;
											if (Ver1[2] > 1 && Ver2[2] > 1 && Ver3[2] > 1)continue;
											//视口变换，平行光不用做透视除法
											float x1 = (Ver1[0] + 1) * 0.5 * ShadowWidth;
											float y1 = (1 - Ver1[1]) * 0.5 * ShadowHeight;
											float z1 = (Ver1[2] + 1) * 0.5;
											float x2 = (Ver2[0] + 1) * 0.5 * ShadowWidth;
											float y2 = (1 - Ver2[1]) * 0.5 * ShadowHeight;
											float z2 = (Ver2[2] + 1) * 0.5;
											float x3 = (Ver3[0] + 1) * 0.5 * ShadowWidth;
											float y3 = (1 - Ver3[1]) * 0.5 * ShadowHeight;
											float z3 = (Ver3[2] + 1) * 0.5;
											//光栅化
											float erase = ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1));
											if (std::abs(erase) < 1e-6f) continue;
											//包围盒
											int minX = std::max(std::min(std::min(x1, x2), x3), 0.f);
											int maxX = std::min(std::max(std::max(x1, x2), x3), float(ShadowWidth - 1));
											int minY = std::max(std::min(std::min(y1, y2), y3), 0.f);
											int maxY = std::min(std::max(std::max(y1, y2), y3), float(ShadowHeight - 1));
											//重心分量判断
											#pragma omp parallel for collapse(2)
											for (int x = minX; x <= maxX; ++x)
											{
												for (int y = minY; y <= maxY; ++y)
												{
													//获取重心坐标分量
													//轮流替换三角形中的三个顶点
													float C1 = ((x2 - float(x)) * (y3 - float(y)) - (y2 - float(y)) * (x3 - float(x))) / erase;
													float C2 = ((float(x) - x1) * (y3 - y1) - (float(y) - y1) * (x3 - x1)) / erase;
													float C3 = ((x2 - x1) * (float(y) - y1) - (y2 - y1) * (float(x) - x1)) / erase;
													//像素是否在三角形内 
													if (C1 >= 0 && C2 >= 0 && C3 >= 0)
													{
														float DepthInter = C1 * z1 + C2 * z2 + C3 * z3;//插值深度
														//像素着色
														if (DepthInter > (ShadowZbuffer[i])[y * ShadowWidth + x])continue;//比较深度值
														(ShadowZbuffer[i])[y * ShadowWidth + x] = DepthInter;//如果在前面(就是深度小的),存入深度缓冲区
													}
												}
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	//后处理
	void CPURender::PostProcess()
		{
			std::fill(PixelColorBuffer.begin(), PixelColorBuffer.end(), 0x00000000);//窗口屏幕背景色
			//设置后处理的颜色缓冲区
			for (int i = 0; i < pnum_CameraInst.size(); ++i)//每个摄像机实例的颜色缓冲区
			{
				if (pnum_CameraInst[i]->IsRender)//确认渲染此摄像机
				{
					Camera = pnum_CameraInst[i]->CameraCom->GetCamera();//获取摄像机组件
					if (pnum_CameraInst[i]->UseDefault)//如果启用默认值
					{
						CameraPara.Width = Camera->DefaultWidth;//宽
						CameraPara.Height = Camera->DefaultHeight;//高
						CameraPara.X = Camera->DefaultX; CameraPara.Y = Camera->DefaultY;//起始位置
					}
					else//使用实例值
					{
						CameraPara.Width = pnum_CameraInst[i]->InstWidth;//宽
						CameraPara.Height = pnum_CameraInst[i]->InstHeight;//高
						CameraPara.X = pnum_CameraInst[i]->InstX; CameraPara.Y = pnum_CameraInst[i]->InstY;//起始位置
					}	
					//钳制范围保证在窗口范围内，如果摄像机屏幕比窗口屏幕大，需要从摄像机屏幕能在窗口显示的位置开始
					int CameraLeft = std::max(0.f, -CameraPara.X);
					int CameraTop = std::max(0.f, -CameraPara.Y);
					int CameraRight = std::min((int)WindowsWidth - CameraPara.X, CameraPara.Width);
					int CameraBottom = std::min((int)WindowsHeight - CameraPara.Y, CameraPara.Height);
					if (CameraLeft >= CameraRight || CameraTop >= CameraBottom)continue;
					#pragma omp parallel for
					for (int y = CameraTop; y <= CameraBottom - 1; ++y)//遍历摄像机的行
					{

						std::memcpy(&PixelColorBuffer[(size_t)(CameraPara.Y + y) * WindowsWidth + CameraPara.X + CameraLeft],//窗口屏幕的像素位置
									&CameraColorBuffer[i][(size_t)y * CameraPara.Width + CameraLeft],//摄像机屏幕的像素位置
									(size_t)(CameraRight - CameraLeft) * sizeof(UINT32));//字节数
					}
				}
			}
			#pragma omp parallel for
			//目前先直接将所有摄像机的颜色缓冲区中的颜色传给最终绘制的颜色缓冲区，后面实现更复杂的情况
			for (int i = 0; i < WindowsWidth * WindowsHeight; ++i)
			{
				FinallyPixelColorBuffer[i] = PixelColorBuffer[i];
			}
		}
	//将颜色绘制到当前帧的画布上
	void CPURender::Draw()
		{
			CurrentPixels = CurrentFrame.Pixels();
			#pragma omp parallel for
			for (int i = 0; i < WindowsWidth * WindowsHeight; ++i)
			{
				CurrentPixels[i] = FinallyPixelColorBuffer[i];
			}
		}
	//呈现当前帧画面并交换当前帧和上一帧的画布
	void CPURender::Present()
		{
			//呈现当前帧画面
			BitBlt(hdcWindow, 0, 0, WindowsWidth, WindowsHeight, CurrentFrame.hMemDC, 0, 0, SRCCOPY);
			//交换当前帧和上一帧
			std::swap(CurrentFrame, LastFrame);
		}
	//重置摄像机的颜色缓冲区和深度缓冲区
	void CPURender::ResetCameraBuffer()
	{
		for (int i = 0; i < pnum_CameraInst.size(); ++i)//遍历摄像机实例
		{
			if (pnum_CameraInst[i]->IsRender)//确认渲染此摄像机
			{
				Camera = pnum_CameraInst[i]->CameraCom->GetCamera();//获取摄像机组件指针
				if (pnum_CameraInst[i]->UseDefault)//如果启用默认值
				{
					CameraPara.CameraProjection = Camera->DefaultCameraProjection;//摄像机投影类型
					CameraPara.Location = Camera->DefaultLocation;//位置
					CameraPara.Width = Camera->DefaultWidth;//宽
					CameraPara.Height = Camera->DefaultHeight;//高
					CameraPara.X = Camera->DefaultX; CameraPara.Y = Camera->DefaultY;//起始位置
				}
				else//使用实例值
				{
					CameraPara.CameraProjection = pnum_CameraInst[i]->InstCameraProjection;//投影类型
					CameraPara.Location = pnum_CameraInst[i]->InstLocation;//位置
					CameraPara.Width = pnum_CameraInst[i]->InstWidth;//宽
					CameraPara.Height = pnum_CameraInst[i]->InstHeight;//高
					CameraPara.X = pnum_CameraInst[i]->InstX; CameraPara.Y = pnum_CameraInst[i]->InstY;//起始位置
				}
				//resize(n, val) 里的 val 只作用于新增元素
				CameraDepthBuffer[i].assign(CameraPara.Width * CameraPara.Height, 1.0f);//重置摄像机的深度缓冲区大小和值
				if (i % 3 == 0)
				{
					CameraColorBuffer[i].assign(CameraPara.Width * CameraPara.Height, 0x0000F0F0);//重置摄像机的颜色缓冲区大小和值
				}
				else
				{
					CameraColorBuffer[i].assign(CameraPara.Width * CameraPara.Height, 0x0000F00F);//重置摄像机的颜色缓冲区大小和值
				}
			}
		}
	}
	//重置深度阴影缓冲
	void CPURender::ResetShadow()
		{
			//重置置每个光源的阴影深度缓冲区大小和值
			for (int i = 0; i < pnum_LightInst.size(); ++i)
			{
				ShadowZbuffer[i].assign(ShadowWidth * ShadowHeight, 1.f);
			}
		}
	//计算M模型矩阵,传入参数分别为位置，方向，大小
	Eigen::Matrix4f GetModelMat(Eigen::Vector3f const& Location, Eigen::Vector3f const& Rotate, Eigen::Vector3f const& Zoom)
	{
		//将欧拉角转向四元数
		float radian = 0.00872;//(3.14/180)/2
		float cx = std::cos(Rotate[0] * radian), sx = std::sin(Rotate[0] * radian);
		float cy = std::cos(Rotate[1] * radian), sy = std::sin(Rotate[1] * radian);
		float cz = std::cos(Rotate[2] * radian), sz = std::sin(Rotate[2] * radian);
		Eigen::Vector4f Qua = { cz * cy * cx + sz * sy * sx,
								cz * cy * sx - sz * sy * cx,
								cz * sy * cx + sz * cy * sx,
								sz * cy * cx - cz * sy * sx };//w,x,y,z,注意w在前面
		Qua.normalize();//归一化四元数
		float xx = Qua[1] * Qua[1], yy = Qua[2] * Qua[2], zz = Qua[3] * Qua[3];
		float xy = Qua[1] * Qua[2], xz = Qua[1] * Qua[3], yz = Qua[2] * Qua[3];
		float wx = Qua[0] * Qua[1], wy = Qua[0] * Qua[2], wz = Qua[0] * Qua[3];
		//缩放，旋转，移动
		Eigen::Matrix4f Mat{{(1 - 2 * (yy + zz)) * Zoom[0],2 * (xy - wz) * Zoom[1],2 * (xz + wy) * Zoom[2],Location[0]},
							{2 * (xy + wz) * Zoom[0],(1 - 2 * (xx + zz)) * Zoom[1],2 * (yz - wx) * Zoom[2],Location[1] },
							{2 * (xz - wy) * Zoom[0],2 * (yz + wx) * Zoom[1],(1 - 2 * (xx + yy)) * Zoom[2],Location[2]},
							{0.f,0.f,0.f,1.f}};
		return Mat;
	}
	//计算V视图矩阵，传入参数分别为世界位置，观察位置，朝上向量
	Eigen::Matrix4f GetViewMat(Eigen::Vector3f const& Location, Eigen::Vector3f const& LookLocation, Eigen::Vector3f const& UpDir)
	{
		Eigen::Vector3f Z = (LookLocation - Location).normalized();
		Eigen::Vector3f X = (Z.cross(UpDir.normalized()));
		////
		if (X.squaredNorm() < 1e-8f) {
			// UpDir 与视线平行，换一个不平行的辅助轴
			X = Z.cross(std::abs(Z.y()) < 0.9f ? Eigen::Vector3f::UnitX()
				: Eigen::Vector3f::UnitY());
		}
		////
		X.normalize();
		Eigen::Vector3f Y = (X.cross(Z)).normalized();
		//减去摄像机在摄像机坐标系下的位置，因为变换到摄像机坐标系下的顶点需要和摄像机位置之间的相对位置
		Eigen::Matrix4f Mat{{X[0],X[1],X[2],-X.dot(Location)},
							{Y[0],Y[1],Y[2],-Y.dot(Location)},
							{ -Z[0],-Z[1],-Z[2],Z.dot(Location) },
							{ 0.f,0.f,0.f,1.f }};
		//在从世界空间转向摄像机空间后，因为摄像机视线方向朝屏幕内，而实际的z轴正方向应该朝屏幕外
		return Mat;
	}
	//计算P透视投影矩阵,传入参数分别为垂直视野角度，宽高比，近裁剪面，远裁剪面
	Eigen::Matrix4f GetPerProMat(float const& Fovy, float const& Aspect, float const& Near, float const& Far)
	{
		float TanFovy = std::tan(Fovy * 0.00872);//(3.14/180)/2
		Eigen::Matrix4f Mat{{ 1 / (TanFovy * Aspect),0.f,0.f,0.f},
							{0.f,1 / (TanFovy),0.f,0.f},
							{0.f,0.f,-((Near + Far) / (Far - Near)),-((2 * Near * Far) / (Far - Near))},
							{0.f,0.f,-1.f,0.f } };
		return Mat;
	}
	//计算P正交投影矩阵,传入 参数分别为左边界，右边界，上边界，下边界，近裁剪面，远裁剪面
	Eigen::Matrix4f GetOrtProMat(float const& Left, float const& Right, float const& Bottom, float const& Top, float const& Near, float const& Far)
	{
		Eigen::Matrix4f Mat{{2 / (Right - Left),0.f,0.f,-(Right + Left) / (Right - Left)},
							{0.f,2 / (Top - Bottom),0.f,-(Top + Bottom) / (Top - Bottom)},
							{0.f,0.f,-2 / (Far - Near),-(Far + Near) / (Far - Near)},
							{0.f,0.f,0.f,1.f} };

		return Mat;
	}

#endif