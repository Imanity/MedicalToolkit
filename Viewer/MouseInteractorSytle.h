#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>

#include <QtWidgets/QWidget>

class MouseInteractorStyle : public QWidget, public vtkInteractorStyleTrackballCamera {
	Q_OBJECT

public:
	static MouseInteractorStyle * New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnRightButtonDown();
	virtual void OnRightButtonUp();
	virtual void OnMouseMove();

protected:
	MouseInteractorStyle();

signals:
	void pickCell(int);
	void startPick();
	void stopPick();

public:
	bool isPicking = false;
	bool isDragging = false;
	vtkSmartPointer<vtkPolyData> polydata;
};
