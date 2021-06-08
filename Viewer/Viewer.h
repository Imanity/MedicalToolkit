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
	// ========================== �˵��� =============================
	// ��Dicom�ļ�
	void onOpenDicomFile();
	// ��Nifti�ļ�
	void onOpenNiftiFile();
	// ����Nifti�ļ�
	void onSaveNiftiFile();
	// ��DSA�ļ�
	void onOpenDSAFile();
	// ��Ѫ����ǿ�Ի���
	void onExtractVessel();
	// ��������ѡȡ��ǩҳ
	void onVolumePicking();
	// ���ֶ�DSA��׼��ǩҳ
	void onManualRegisterDSA();

	// ========================== ��ʾͼ�� =============================
	// ѡ��ͼ��
	void setCurrentLayer(int id);
	// ɾ��ͼ��
	void deleteLayer(int id);

	// ========================== ��ʾ2Dͼ�� =============================
	// ѡ��ͼ��
	void setCurrentLayer2d(int id);
	// ɾ��ͼ��
	void deleteLayer2d(int id);

	// ========================== �������� =============================
	// �����Ƿ���ʾ������
	void updateAxesVisible(int);
	// �����Ƿ���ʾ3D��Ƭ
	void updateSliceVisible(int);
	
	// ========================== ��ʾ��ǩҳ =============================
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

	// ========================== ��ʾ2D��ǩҳ =============================
	// ����ѡ��2Dͼ����
	void updateLayer2dName(QString str);

	// ========================== ������ʰȡ��ǩҳ =============================
	// ����/�ر�������ʰȡ
	void onPickingVolume();
	// ����/�ر�������ȡ��ʰȡ
	void onUnpickingVolume();
	// ʰȡcell
	void onPickedCell(int id);
	// ��ʼʰȡ
	void onStartPickingCell();
	// ֹͣʰȡ
	void onStopPickingCell();
	// ȫ��ʰȡ
	void onPickAll();

	// ========================== CT-DSA��׼��ǩҳ =============================
	// ѡ������ͼDSAͼ��
	void onOpenDSAFront();
	// ѡ������ͼDSAͼ��
	void onOpenDSARight();
	// ����DSAͼ����Ƭ
	void selectDSAPos(int pos);
	// ������׼
	void onRegistration();
	// �����ں�
	void onFusion();

	// ========================== Ѫ����ǿ =============================
	// Ѫ����ǿ
	void extractVessel(int nonContrastId, int enhanceId);

	// ========================== ���� =============================
	// ˢ�����пؼ�
	void updateAllViewers();

private:
	// ��������ͼ����Ϣ
	void updateLayers();
	// ����ͼ����ϸ��Ϣ
	void updateLayerDetail();
	// ��������2Dͼ����Ϣ
	void updateLayers2d();
	// ����2Dͼ����ϸ��Ϣ
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
