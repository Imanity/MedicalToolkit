#pragma once

#include <QResizeEvent>
#include <QWheelEvent>

#include <QWidget>
#include "ui_Viewer2D.h"

#include "Viewer3D.h"

class Viewer2D : public QWidget {
	Q_OBJECT

public:
	Viewer2D(SLICE_PLANE plane, Viewer3D *viewer3d, QWidget *parent = Q_NULLPTR);
	~Viewer2D();

	// ˢ�½���
	void updateView();

	// �ؼ���С�¼�����
	void resizeEvent(QResizeEvent* event);
	// �����¼�����
	void wheelEvent(QWheelEvent* event);

public:
	double pos = 0.0;

private:
	Ui::Viewer2D ui;

	SLICE_PLANE plane;
	Viewer3D *viewer3d;
};
