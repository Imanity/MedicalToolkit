#pragma once

#include <QSignalMapper>
#include <QCheckBox>
#include <QLabel>

#include <QtWidgets/QMainWindow>
#include "ui_Viewer.h"

#include "Viewer2D.h"
#include "Viewer3D.h"
#include "ViewerDSA.h"

#include "Widgets/ClickableLabel.h"

struct LayerItem {
	QCheckBox *checkBox;
	ClickableLabel *colorLabel;
	ClickableLabel *label;
	ClickableLabel *closeBtn;
};

struct Layer2DItem {
	ClickableLabel *label;
	ClickableLabel *closeBtn;
};

class Viewer : public QMainWindow {
	Q_OBJECT

public:
	Viewer(QWidget *parent = Q_NULLPTR);

public slots:
	// ========================== 菜单栏 =============================
	// 打开Dicom文件
	void onOpenDicomFile();
	// 打开Nifti文件
	void onOpenNiftiFile();
	// 导出Nifti文件
	void onSaveNiftiFile();
	// 打开DSA文件
	void onOpenDSAFile();
	// 打开血管增强对话框
	void onExtractVessel();
	// 打开体数据选取标签页
	void onVolumePicking();
	// 打开手动DSA配准标签页
	void onManualRegisterDSA();

	// ========================== 显示图层 =============================
	// 选中图层
	void setCurrentLayer(int id);
	// 删除图层
	void deleteLayer(int id);

	// ========================== 显示2D图层 =============================
	// 选中图层
	void setCurrentLayer2d(int id);
	// 删除图层
	void deleteLayer2d(int id);

	// ========================== 绘制属性 =============================
	// 更新是否显示坐标轴
	void updateAxesVisible(int);
	// 更新是否显示3D切片
	void updateSliceVisible(int);
	
	// ========================== 显示标签页 =============================
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

	// ========================== 显示2D标签页 =============================
	// 更新选定2D图层名
	void updateLayer2dName(QString str);

	// ========================== 体数据拾取标签页 =============================
	// 开启/关闭体数据拾取
	void onPickingVolume();
	// 开启/关闭体数据取消拾取
	void onUnpickingVolume();
	// 拾取cell
	void onPickedCell(int id);
	// 开始拾取
	void onStartPickingCell();
	// 停止拾取
	void onStopPickingCell();
	// 全部拾取
	void onPickAll();

	// ========================== CT-DSA配准标签页 =============================
	// 选择正视图DSA图像
	void onOpenDSAFront();
	// 选择右视图DSA图像
	void onOpenDSARight();
	// 更新DSA图像切片
	void selectDSAPos(int pos);
	// 进行配准
	void onRegistration();
	// 进行融合
	void onFusion();

	// ========================== 血管增强 =============================
	// 血管增强
	void extractVessel(int nonContrastId, int enhanceId);

	// ========================== 其他 =============================
	// 刷新所有控件
	void updateAllViewers();

private:
	// 更新所有图层信息
	void updateLayers();
	// 更新图层详细信息
	void updateLayerDetail();
	// 更新所有2D图层信息
	void updateLayers2d();
	// 更新2D图层详细信息
	void updateLayers2dDetail();

private:
	Ui::ViewerClass ui;
	Viewer2D *viewer_up;
	Viewer2D *viewer_left;
	Viewer2D *viewer_front;
	Viewer3D *viewer3d;
	ViewerDSA* viewerDSA;
	QSignalMapper *visibleSignalMapper;
	QSignalMapper *selectSignalMapper;
	QSignalMapper *closeSignalMapper;
	QSignalMapper *selectSignalMapper2d;
	QSignalMapper *closeSignalMapper2d;
	std::vector<LayerItem> layerItems;
	std::vector<Layer2DItem> layer2dItems;
	int currentLayerId = 0;
	int currentLayer2dId = 0;
	int pickedLayerId = 0;
	QWidget *pageCTDSARegister;
	QWidget *pagePicking;
	bool pickStatus = 0;
	double maxPickingDistance = 24.0;
	double maxUnPickingDistance = 16.0;
	std::vector<int> pickedCells;
	int selectingDSA = 0;
	int dsa_front_id = 0, dsa_front_pos = 0;
	int dsa_right_id = 0, dsa_right_pos = 0;
};
