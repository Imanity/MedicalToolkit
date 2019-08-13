#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>

#include <QtWidgets/QWidget>

class MouseInteractorStyle : public QWidget, public vtkInteractorStyleTrackballCamera {
	Q_OBJECT

public:
	static MouseInteractorStyle * New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();

protected:
	MouseInteractorStyle();

signals:
	void pickCell(int);

public:
	bool isPicking = false;
	vtkSmartPointer<vtkPolyData> polydata;
};
