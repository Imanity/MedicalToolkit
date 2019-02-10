#pragma once

#include <vtkAutoInit.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include <opencv2/opencv.hpp>

#include <QVTKWidget.h>
#include "ui_Viewer3D.h"

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

	// 生成二维切片视图
	cv::Mat generateSlice2d(int plane, double pos, int scale);
	// 生成二维切片
	vtkSmartPointer<vtkImageData> generateSlice2d(int plane, double pos);

	// 等值面提取
	vtkSmartPointer<vtkPolyData> isoSurface(VolumeData<short> &v, int isoValue);

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
	bool sliceFlag = true;
	double slicePos[3];

public:
	std::vector<VolumeData<short>> volumes;
	std::vector<vtkSmartPointer<vtkPolyData>> meshes;
	std::vector<bool> visible;
	std::vector<int> isoValue;
	std::vector<int> WindowWidth;
	std::vector<int> WindowCenter;
	std::vector<QColor> color;
	std::vector<QString> title;
};
