#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>

#include "MouseInteractorSytle.h"

vtkStandardNewMacro(MouseInteractorStyle);

MouseInteractorStyle::MouseInteractorStyle() {
}

void MouseInteractorStyle::OnRightButtonDown() {
	if (isPicking) {
		emit startPick();
		isDragging = true;
		return;
	}

	vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void MouseInteractorStyle::OnRightButtonUp() {
	if (isPicking) {
		emit stopPick();
		isDragging = false;
		return;
	}

	vtkInteractorStyleTrackballCamera::OnRightButtonUp();
}

void MouseInteractorStyle::OnMouseMove() {
	if (isPicking && isDragging) {
		vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
		picker->SetTolerance(0.01);
		picker->Pick(this->GetInteractor()->GetEventPosition()[0], this->GetInteractor()->GetEventPosition()[1], 0, this->GetDefaultRenderer());
		int cell_id = picker->GetCellId();
		if (cell_id >= 0) {
			emit pickCell(cell_id);
		}

		return;
	}

	vtkInteractorStyleTrackballCamera::OnMouseMove();
}
