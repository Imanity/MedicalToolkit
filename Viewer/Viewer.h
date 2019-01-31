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
	// 打开Dicom文件
	void openDicomFile();
	// 选中图层
	void setCurrentLayer(int id);

	// 更新选定图层名
	void updateLayerName(QString str);
	// 更新选定图层颜色
	void updateLayerColor();
	// 更新选定图层窗位
	void updateLayerWindowCenterVal();
	void updateLayerWindowCenter();
	// 更新选定图层窗宽
	void updateLayerWindowWidthVal();
	void updateLayerWindowWidth();
	// 更新选定图层等值面取值
	void updateLayerIsoValueVal();
	void updateLayerIsoValue();

private:
	// 更新所有图层信息
	void updateLayers();
	// 更新图层详细信息
	void updateLayerDetail();
	// 刷新所有控件
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
