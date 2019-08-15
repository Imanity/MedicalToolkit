#pragma once

#include <QSignalMapper>
#include <QCheckBox>
#include <QLabel>

#include <QtWidgets/QMainWindow>
#include "ui_Viewer.h"

#include "Viewer2D.h"
#include "Viewer3D.h"

#include "Widgets/ClickableLabel.h"

struct LayerItem {
	QCheckBox *checkBox;
	ClickableLabel *colorLabel;
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

	// ========================== �ֶ���׼��ǩҳ =============================
	// ��DSA�ļ�
	void onOpenDSAFile();
	// �����ֶ���׼���
	void onGenerateCamera();
	// ������ʾ֡
	void updateFrame(int);

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

private:
	Ui::ViewerClass ui;
	Viewer2D *viewer_up;
	Viewer2D *viewer_left;
	Viewer2D *viewer_front;
	Viewer3D *viewer3d;
	QSignalMapper *visibleSignalMapper;
	QSignalMapper *colorSignalMapper;
	QSignalMapper *closeSignalMapper;
	std::vector<LayerItem> layerItems;
	int currentLayerId = 0;
	int pickedLayerId = 0;
	QWidget *pageDSAManualRegister;
	QWidget *pagePicking;
	bool pickStatus = 0;
	double maxPickingDistance = 24.0;
	double maxUnPickingDistance = 16.0;
	std::vector<int> pickedCells;
};
