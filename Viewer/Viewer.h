#pragma once

#include <QSignalMapper>
#include <QCheckBox>
#include <QLabel>

#include <QtWidgets/QMainWindow>
#include "ui_Viewer.h"

#include "Viewer2D.h"
#include "Viewer3D.h"
#include "ClickableLabel.h"

struct LayerItem {
	QCheckBox *checkBox;
	ClickableLabel *colorLabel;
	ClickableLabel *label;
};

class Viewer : public QMainWindow {
	Q_OBJECT

public:
	Viewer(QWidget *parent = Q_NULLPTR);

public slots:
	// ��Dicom�ļ�
	void openDicomFile();
	// ѡ��ͼ��
	void setCurrentLayer(int id);

	// ����ѡ��ͼ����
	void updateLayerName(QString str);
	// ����ѡ��ͼ����ɫ
	void updateLayerColor();
	// ����ѡ��ͼ�㴰λ
	void updateLayerWindowCenterVal();
	void updateLayerWindowCenter();
	// ����ѡ��ͼ�㴰��
	void updateLayerWindowWidthVal();
	void updateLayerWindowWidth();
	// ����ѡ��ͼ���ֵ��ȡֵ
	void updateLayerIsoValueVal();
	void updateLayerIsoValue();

private:
	// ��������ͼ����Ϣ
	void updateLayers();
	// ����ͼ����ϸ��Ϣ
	void updateLayerDetail();
	// ˢ�����пؼ�
	void updateAllViewers();

private:
	Ui::ViewerClass ui;
	Viewer2D *viewer_up;
	Viewer2D *viewer_left;
	Viewer2D *viewer_front;
	Viewer3D *viewer3d;
	QSignalMapper *visibleSignalMapper;
	QSignalMapper *colorSignalMapper;
	std::vector<LayerItem> layerItems;
	int currentLayerId = 0;
};
