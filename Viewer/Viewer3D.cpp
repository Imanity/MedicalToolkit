#include "Viewer3D.h"

#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageImport.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkMarchingCubes.h>
#include <vtkStripper.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkNew.h>
#include <vtkSmoothPolyDataFilter.h>

Viewer3D::Viewer3D(QWidget *parent) : QVTKWidget(parent) {
	ui.setupUi(this);

	this->renderer = vtkSmartPointer<vtkRenderer>::New();
	this->GetRenderWindow()->AddRenderer(this->renderer);
	this->GetRenderWindow()->Render();
}

Viewer3D::~Viewer3D() {
}

void Viewer3D::updateView() {
	this->renderer->RemoveAllViewProps();

	if (renderingMode == VOLUME_RENDERING) {
		for (int i = 0; i < volumes.size(); ++i) {
			if (!visible[i])
				continue;

			vtkSmartPointer<vtkImageImport> image_import = vtkSmartPointer<vtkImageImport>::New();
			image_import->SetDataSpacing(volumes[i].dx, volumes[i].dy, volumes[i].dz);
			image_import->SetDataOrigin(0, 0, 0);
			image_import->SetWholeExtent(0, volumes[i].nx - 1, 0, volumes[i].ny - 1, 0, volumes[i].nz - 1);
			image_import->SetDataExtentToWholeExtent();
			image_import->SetDataScalarTypeToUnsignedShort();
			image_import->SetNumberOfScalarComponents(1);
			image_import->SetImportVoidPointer(volumes[i].data);
			image_import->Update();

			vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
			volumeMapper->SetInputConnection(image_import->GetOutputPort());

			vtkSmartPointer<vtkColorTransferFunction> volumeColor = vtkSmartPointer<vtkColorTransferFunction>::New();
			volumeColor->AddRGBPoint(0, 0.0, 0.0, 0.0);
			volumeColor->AddRGBPoint(200, (double)color[i].red() / 255.0, 
				(double)color[i].green() / 255.0, (double)color[i].blue() / 255.0);

			vtkSmartPointer<vtkPiecewiseFunction> volumeScalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
			volumeScalarOpacity->AddPoint(0, 0.0);
			volumeScalarOpacity->AddPoint(200, (double)color[i].alpha() / 255.0);

			vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
			volumeGradientOpacity->AddPoint(0, 0.0);
			volumeGradientOpacity->AddPoint(200, 1.0);

			vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
			volumeProperty->SetColor(volumeColor);
			volumeProperty->SetScalarOpacity(volumeScalarOpacity);
			volumeProperty->SetGradientOpacity(volumeGradientOpacity);
			volumeProperty->SetInterpolationTypeToLinear();
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(ambient);
			volumeProperty->SetDiffuse(diffuse);
			volumeProperty->SetSpecular(specular);

			vtkSmartPointer<vtkVolume> volumeVTK = vtkSmartPointer<vtkVolume>::New();
			volumeVTK->SetMapper(volumeMapper);
			volumeVTK->SetProperty(volumeProperty);
			renderer->AddViewProp(volumeVTK);
		}
	} else if (renderingMode == MESH_RENDERING) {
		for (int i = 0; i < meshes.size(); ++i) {
			if (!visible[i])
				continue;

			vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			meshMapper->SetInputData(meshes[i]);
			meshMapper->ScalarVisibilityOff();

			vtkSmartPointer<vtkActor> meshActor = vtkSmartPointer<vtkActor>::New();
			meshActor->SetMapper(meshMapper);
			meshActor->GetProperty()->SetColor((double)color[i].red() / 255.0, (double)color[i].green() / 255.0, (double)color[i].blue() / 255.0);
			meshActor->GetProperty()->SetOpacity((double)color[i].alpha() / 255.0);
			meshActor->GetProperty()->SetAmbient(ambient);
			meshActor->GetProperty()->SetDiffuse(diffuse);
			meshActor->GetProperty()->SetSpecular(specular);

			this->renderer->AddActor(meshActor);
		}
	}

	if (isFirstRead) {
		this->renderer->ResetCamera();
		isFirstRead = false;
	}
	this->GetRenderWindow()->Render();
	this->update();
}

void Viewer3D::addVolume(VolumeData<short> v, QString title) {
	this->title.push_back(title);
	volumes.push_back(v);
	isoValue.push_back(200);
	meshes.push_back(isoSurface(v, 200));
	WindowCenter.push_back(200);
	WindowWidth.push_back(800);
	color.push_back(QColor(255, 255, 255, 255));
	visible.push_back(true);

	lenX = (v.dx * v.nx) > lenX ? (v.dx * v.nx) : lenX;
	lenY = (v.dy * v.ny) > lenY ? (v.dy * v.ny) : lenY;
	lenZ = (v.dz * v.nz) > lenZ ? (v.dz * v.nz) : lenZ;
}

vtkSmartPointer<vtkPolyData> Viewer3D::isoSurface(VolumeData<short> &v, int isoValue) {
	vtkSmartPointer<vtkImageImport> image_import = vtkSmartPointer<vtkImageImport>::New();
	image_import->SetDataSpacing(v.dx, v.dy, v.dz);
	image_import->SetDataOrigin(0, 0, 0);
	image_import->SetWholeExtent(0, v.nx - 1, 0, v.ny - 1, 0, v.nz - 1);
	image_import->SetDataExtentToWholeExtent();
	image_import->SetDataScalarTypeToShort();
	image_import->SetNumberOfScalarComponents(1);
	image_import->SetImportVoidPointer(v.data);
	image_import->Update();

	vtkSmartPointer<vtkMarchingCubes> marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
	marchingCubes->SetInputConnection(image_import->GetOutputPort());
	marchingCubes->SetValue(0, isoValue);

	vtkSmartPointer<vtkStripper> stripper = vtkSmartPointer<vtkStripper>::New();
	stripper->SetInputConnection(marchingCubes->GetOutputPort());

	vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
	connectivityFilter->SetInputConnection(stripper->GetOutputPort());
	connectivityFilter->SetExtractionModeToLargestRegion();
	connectivityFilter->Update();

	vtkNew<vtkSmoothPolyDataFilter> smoothFilter;
	smoothFilter->SetInputData(connectivityFilter->GetOutput());
	smoothFilter->SetNumberOfIterations(15);
	smoothFilter->SetRelaxationFactor(0.1);
	smoothFilter->Update();

	return smoothFilter->GetOutput();
}

cv::Mat Viewer3D::generateSlice2d(int plane, double pos, int scale) {
	if (lenX == 0 || lenY == 0 || lenZ == 0) {
		cv::Mat blank_img(scale, scale, CV_8UC3);
		for (int i = 0; i < scale; ++i) for (int j = 0; j < scale; ++j)
			blank_img.at<cv::Vec3b>(i, j)[0] = blank_img.at<cv::Vec3b>(i, j)[1] = blank_img.at<cv::Vec3b>(i, j)[2] = 0;
		return blank_img;
	}
	int xMax = 0, yMax = 0;
	double spacing = 1.0;
	if (plane == SAGITTAL_PLANE) {
		if (lenZ > lenY) {
			xMax = (double)scale * lenY / lenZ;
			yMax = (double)scale;
			spacing = (double)lenZ / scale;
		} else {
			xMax = (double)scale;
			yMax = (double)scale * lenZ / lenY;
			spacing = (double)lenY / scale;
		}
	} else if (plane == CORONAL_PLANE) {
		if (lenZ > lenX) {
			xMax = (double)scale * lenX / lenZ;
			yMax = (double)scale;
			spacing = (double)lenZ / scale;
		} else {
			xMax = (double)scale;
			yMax = (double)scale * lenZ / lenX;
			spacing = (double)lenX / scale;
		}
	} else {
		if (lenY > lenX) {
			xMax = (double)scale * lenX / lenY;
			yMax = (double)scale;
			spacing = (double)lenY / scale;
		} else {
			xMax = (double)scale;
			yMax = (double)scale * lenY / lenX;
			spacing = (double)lenX / scale;
		}
	}
	cv::Mat img(yMax, xMax, CV_8UC3);
	for (int y = 0; y < yMax; ++y) {
		for (int x = 0; x < xMax; ++x) {
			int r = 0, g = 0, b = 0;
			for (int i = 0; i < volumes.size(); ++i) {
				if (!visible[i])
					continue;

				double xIndex = 0, yIndex = 0, zIndex = 0;
				if (plane == SAGITTAL_PLANE) {
					xIndex = (double)pos / volumes[i].dx;
					yIndex = (double)spacing * x / volumes[i].dy;
					zIndex = (double)spacing * y / volumes[i].dz;
				} else if (plane == CORONAL_PLANE) {
					xIndex = (double)spacing * x / volumes[i].dx;
					yIndex = (double)pos / volumes[i].dy;
					zIndex = (double)spacing * y / volumes[i].dz;
				} else {
					xIndex = (double)spacing * x / volumes[i].dx;
					yIndex = (double)spacing * y / volumes[i].dy;
					zIndex = (double)pos / volumes[i].dz;
				}
				if (xIndex >= volumes[i].nx || yIndex >= volumes[i].ny || zIndex >= volumes[i].nz)
					continue;
				short val = volumes[i].trilinear(xIndex, yIndex, zIndex);
				double op = (double)(val - WindowCenter[i] + WindowWidth[i] / 2.0) / WindowWidth[i];
				op = op < 0 ? 0 : op;
				op = op > 1 ? 1 : op;
				r = op * color[i].red();
				g = op * color[i].green();
				b = op * color[i].blue();
			}
			if (plane == TRANSVERSE_PLANE) {
				img.at<cv::Vec3b>(y, x)[0] = r;
				img.at<cv::Vec3b>(y, x)[1] = g;
				img.at<cv::Vec3b>(y, x)[2] = b;
			} else {
				img.at<cv::Vec3b>(yMax - y - 1, x)[0] = r;
				img.at<cv::Vec3b>(yMax - y - 1, x)[1] = g;
				img.at<cv::Vec3b>(yMax - y - 1, x)[2] = b;
			}
		}
	}
	return img;
}

void Viewer3D::setAmbient(int v) {
	ambient = (double)v / 100.0;
	updateView();
}

void Viewer3D::setDiffuse(int v) {
	diffuse = (double)v / 100.0;
	updateView();
}

void Viewer3D::setSpecular(int v) {
	specular = (double)v / 100.0;
	updateView();
}

void Viewer3D::setRenderMode(int v) {
	renderingMode = v ? VOLUME_RENDERING : MESH_RENDERING;
	updateView();
}

void Viewer3D::setVisible(int v) {
	visible[v] = !visible[v];
	updateView();
}

void Viewer3D::setIsoValue(int i, int v) {
	isoValue[i] = v;
	meshes[i] = isoSurface(volumes[i], v);
	updateView();
}
