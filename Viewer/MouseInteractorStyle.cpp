#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>

#include "MouseInteractorSytle.h"

vtkStandardNewMacro(MouseInteractorStyle);

MouseInteractorStyle::MouseInteractorStyle() {
}

void MouseInteractorStyle::OnLeftButtonDown() {
	if (isPicking)
		return;

	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void MouseInteractorStyle::OnLeftButtonUp() {
	if (isPicking) {
		vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
		picker->SetTolerance(0.001);
		picker->Pick(this->GetInteractor()->GetEventPosition()[0], this->GetInteractor()->GetEventPosition()[1], 0, this->GetDefaultRenderer());
		int cell_id = picker->GetCellId();
		emit pickCell(cell_id);

		return;
	}

	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}
