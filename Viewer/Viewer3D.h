#pragma once

#include <vtkAutoInit.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

#include <opencv2/opencv.hpp>

#include <QVTKWidget.h>
#include "ui_Viewer3D.h"

#include "MouseInteractorSytle.h"

#include "../VolumeData/VolumeData.h"

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);

enum RENDERING_MODE {
	VOLUME_RENDERING,
	MESH_RENDERING
};

enum SLICE_PLANE {
	SAGITTAL_PLANE,
	CORONAL_PLANE,
	TRANSVERSE_PLANE
};

class Viewer3D : public QVTKWidget {
	Q_OBJECT

public:
	Viewer3D(QWidget *parent = Q_NULLPTR);
	~Viewer3D();

	// 刷新界面
	void updateView();
	// 添加新体数据
	void addVolume(VolumeData<short> v, QString title);
	// 删除体数据
	void deleteVolume(int idx);
	// 添加新DSA图像
	void addDSAImage(VolumeData<short> v, QString title);
	// 删除DSA图像
	void deleteDSAImage(int idx);

	// 生成二维切片视图
	cv::Mat generateSlice2d(int plane, double pos, int scale);
	// 生成二维切片
	vtkSmartPointer<vtkImageData> generateSlice2d(int plane, double pos);

	// 等值面提取
	vtkSmartPointer<vtkPolyData> isoSurface(VolumeData<short> &v, int isoValue, bool skipConnectivityFilter = false);

	// 生成DSA单帧图像
	cv::Mat generateDSA2d(int image_idx, int pos_idx);
	cv::Mat generateDSA2d(int image_idx, int pos_idx, int scale);

public slots:
	// 设定渲染参数
	void setAmbient(int v);
	void setDiffuse(int v);
	void setSpecular(int v);
	void setRenderMode(int v);
	void setVisible(int v);
	void setIsoValue(int i, int v);

private:
	Ui::Viewer3D ui;

	vtkSmartPointer<vtkRenderer> renderer;

	RENDERING_MODE renderingMode = MESH_RENDERING;
	double ambient = 0.0, diffuse = 0.8, specular = 0.2;
	bool isFirstRead = true;

public:
	double lenX = 0, lenY = 0, lenZ = 0;
	bool axesFlag = true;
	bool sliceFlag = false;
	double slicePos[3];
	int currentFrame = 0;

	vtkSmartPointer<MouseInteractorStyle> mouse_style;

public:
	// 三维体数据及显示参数
	std::vector<VolumeData<short>> volumes;
	std::vector<vtkSmartPointer<vtkPolyData>> meshes;
	std::vector<bool> visible;
	std::vector<int> isoValue;
	std::vector<int> WindowWidth;
	std::vector<int> WindowCenter;
	std::vector<QColor> color;
	std::vector<QString> title;

	// 二维DSA数据及显示参数
	std::vector<VolumeData<short>> dsaImages;
	std::vector<QString> dsaTitles;
};
