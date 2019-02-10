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

	// ˢ�½���
	void updateView();
	// �����������
	void addVolume(VolumeData<short> v, QString title);
	// ɾ��������
	void deleteVolume(int idx);

	// ���ɶ�ά��Ƭ��ͼ
	cv::Mat generateSlice2d(int plane, double pos, int scale);
	// ���ɶ�ά��Ƭ
	vtkSmartPointer<vtkImageData> generateSlice2d(int plane, double pos);

	// ��ֵ����ȡ
	vtkSmartPointer<vtkPolyData> isoSurface(VolumeData<short> &v, int isoValue);

public slots:
	// �趨��Ⱦ����
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
