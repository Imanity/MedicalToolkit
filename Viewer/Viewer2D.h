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

	// 刷新界面
	void updateView();

	// 控件大小事件处理
	void resizeEvent(QResizeEvent* event);
	// 滚轮事件处理
	void wheelEvent(QWheelEvent* event);

public:
	double pos = 0.0;

private:
	Ui::Viewer2D ui;

	SLICE_PLANE plane;
	Viewer3D *viewer3d;
};
