#pragma once

#include <QResizeEvent>
#include <QWheelEvent>

#include <QWidget>
#include "ui_ViewerDSA.h"

#include "Viewer3D.h"

class ViewerDSA : public QWidget {
	Q_OBJECT

public:
	ViewerDSA(Viewer3D* viewer3d, QWidget *parent = Q_NULLPTR);
	~ViewerDSA();

	// ˢ�½���
	void updateView();

	// �ؼ���С�¼�����
	void resizeEvent(QResizeEvent* event);
	// �����¼�����
	void wheelEvent(QWheelEvent* event);

signals:
	void select_pos(int pos);

public:
	int image_idx = -1, pos_idx = -1;

private:
	Ui::ViewerDSA ui;

	Viewer3D* viewer3d;
};
