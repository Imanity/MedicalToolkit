#include "Viewer.h"

#include "../VolumeData/VolumeData.h"

#include <QFileDialog>
#include <QSizePolicy>

Viewer::Viewer(QWidget *parent)	: QMainWindow(parent) {
	ui.setupUi(this);

	viewer3d = new Viewer3D(this);
	ui.mainLayout->addWidget(viewer3d);

	viewer_up = new Viewer2D(TRANSVERSE_PLANE, viewer3d, this);
	ui.sliceLayout1->addWidget(viewer_up);
	viewer_left = new Viewer2D(CORONAL_PLANE, viewer3d, this);
	ui.sliceLayout2->addWidget(viewer_left);
	viewer_front = new Viewer2D(SAGITTAL_PLANE, viewer3d, this);
	ui.sliceLayout3->addWidget(viewer_front);

	ui.layerTable->setRowCount(0);
	ui.layerTable->setColumnCount(4);
	ui.layerTable->setShowGrid(false);
	ui.layerTable->horizontalHeader()->setVisible(false);
	ui.layerTable->verticalHeader()->setVisible(false);
	ui.layerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	visibleSignalMapper = new QSignalMapper(this);
	colorSignalMapper = new QSignalMapper(this);
	closeSignalMapper = new QSignalMapper(this);

	ui.windowCenterVal->setValidator(new QIntValidator(1, 2000, this));
	ui.windowWidthVal->setValidator(new QIntValidator(1, 4000, this));
	ui.isoValueVal->setValidator(new QIntValidator(1, 1000, this));

	ui.layerTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.layerTable->setColumnWidth(0, 25);
	ui.layerTable->setColumnWidth(1, 25);
	ui.layerTable->setColumnWidth(2, 100);
	ui.layerTable->setColumnWidth(3, 25);

	connect(ui.actionOpenDicom, SIGNAL(triggered()), this, SLOT(openDicomFile()));
	connect(ui.actionOpenNifti, SIGNAL(triggered()), this, SLOT(openNiftiFile()));
	connect(ui.actionSaveNifti, SIGNAL(triggered()), this, SLOT(saveNiftiFile()));
	connect(ui.ambientSlider, SIGNAL(valueChanged(int)), viewer3d, SLOT(setAmbient(int)));
	connect(ui.diffuseSlider, SIGNAL(valueChanged(int)), viewer3d, SLOT(setDiffuse(int)));
	connect(ui.SpecularSlider, SIGNAL(valueChanged(int)), viewer3d, SLOT(setSpecular(int)));
	connect(ui.renderComboBox, SIGNAL(currentIndexChanged(int)), viewer3d, SLOT(setRenderMode(int)));
	connect(visibleSignalMapper, SIGNAL(mapped(int)), viewer3d, SLOT(setVisible(int)));
	connect(visibleSignalMapper, SIGNAL(mapped(int)), this, SLOT(updateAllViewers()));
	connect(colorSignalMapper, SIGNAL(mapped(int)), this, SLOT(setCurrentLayer(int)));
	connect(closeSignalMapper, SIGNAL(mapped(int)), this, SLOT(deleteLayer(int)));
	connect(ui.layerName, SIGNAL(textChanged(QString)), this, SLOT(updateLayerName(QString)));
	connect(ui.colorR, SIGNAL(sliderReleased()), this, SLOT(updateLayerColor()));
	connect(ui.colorG, SIGNAL(sliderReleased()), this, SLOT(updateLayerColor()));
	connect(ui.colorB, SIGNAL(sliderReleased()), this, SLOT(updateLayerColor()));
	connect(ui.alpha, SIGNAL(sliderReleased()), this, SLOT(updateLayerColor()));
	connect(ui.windowCenter, SIGNAL(sliderReleased()), this, SLOT(updateLayerWindowCenterVal()));
	connect(ui.windowCenterVal, SIGNAL(editingFinished()), this, SLOT(updateLayerWindowCenter()));
	connect(ui.windowWidth, SIGNAL(sliderReleased()), this, SLOT(updateLayerWindowWidthVal()));
	connect(ui.windowWidthVal, SIGNAL(editingFinished()), this, SLOT(updateLayerWindowWidth()));
	connect(ui.isoValue, SIGNAL(sliderReleased()), this, SLOT(updateLayerIsoValueVal()));
	connect(ui.isoValueVal, SIGNAL(editingFinished()), this, SLOT(updateLayerIsoValue()));
	connect(ui.AxesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateAxesVisible(int)));
	connect(ui.SliceCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateSliceVisible(int)));
}

void Viewer::openDicomFile() {
	QString fileToOpen = QFileDialog::getExistingDirectory(this, "Open", "D:/Data/");
	if (!fileToOpen.size())
		return;

	int n = viewer3d->volumes.size();

	VolumeData<short> v;
	v.readFromDicom(fileToOpen.toStdString());
	viewer3d->addVolume(v, QString("Image ") + QString::number(n + 1));

	if (!n) {
		viewer_front->pos = viewer3d->slicePos[0] = viewer3d->lenX / 2.0;
		viewer_left->pos = viewer3d->slicePos[1] = viewer3d->lenY / 2.0;
		viewer_up->pos = viewer3d->slicePos[2] = viewer3d->lenZ / 2.0;
	}

	updateAllViewers();
	updateLayers();
}

void Viewer::openNiftiFile() {
	QString fileToOpen = QFileDialog::getOpenFileName(this, "Open", "D:/Data/", "Nifti(*.nii)");
	if (!fileToOpen.size())
		return;

	int n = viewer3d->volumes.size();

	VolumeData<short> v;
	v.readFromNII(fileToOpen.toStdString());
	viewer3d->addVolume(v, QString("Image ") + QString::number(n + 1));

	if (!n) {
		viewer_front->pos = viewer3d->slicePos[0] = viewer3d->lenX / 2.0;
		viewer_left->pos = viewer3d->slicePos[1] = viewer3d->lenY / 2.0;
		viewer_up->pos = viewer3d->slicePos[2] = viewer3d->lenZ / 2.0;
	}

	updateAllViewers();
	updateLayers();
}

void Viewer::saveNiftiFile() {
	QString fileToSave = QFileDialog::getSaveFileName(this, "Save", "D:/Data/", "Nifti(*.nii)");
	if (!fileToSave.size())
		return;

	if (currentLayerId >= viewer3d->volumes.size())
		return;
	viewer3d->volumes[currentLayerId].writeToNII(fileToSave.toStdString());
}

void Viewer::updateLayers() {
	int n = viewer3d->volumes.size();
	ui.layerTable->setRowCount(n);

	layerItems.clear();
	ui.layerTable->clear();
	
	for (int i = 0; i < n; ++i) {
		LayerItem newItem;
		newItem.checkBox = new QCheckBox(this);
		newItem.checkBox->setChecked(true);
		newItem.checkBox->setStyleSheet("margin-left: 5px");
		connect(newItem.checkBox, SIGNAL(clicked()), visibleSignalMapper, SLOT(map()));
		visibleSignalMapper->setMapping(newItem.checkBox, i);
		ui.layerTable->setCellWidget(i, 0, newItem.checkBox);

		newItem.colorLabel = new ClickableLabel(this);
		newItem.colorLabel->setAutoFillBackground(true);
		newItem.colorLabel->setStyleSheet("QLabel { border: 1px solid black; background-color : " + viewer3d->color[i].name() + "; }");
		newItem.colorLabel->setCursor(Qt::PointingHandCursor);
		ui.layerTable->setCellWidget(i, 1, newItem.colorLabel);
		connect(newItem.colorLabel, SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
		colorSignalMapper->setMapping(newItem.colorLabel, i);

		newItem.label = new ClickableLabel(this);
		newItem.label->setStyleSheet("margin-left: 20px");
		newItem.label->setText(viewer3d->title[i]);
		newItem.label->setCursor(Qt::PointingHandCursor);
		ui.layerTable->setCellWidget(i, 2, newItem.label);
		connect(newItem.label, SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
		colorSignalMapper->setMapping(newItem.label, i);

		newItem.closeBtn = new ClickableLabel(this);
		newItem.closeBtn->setAutoFillBackground(true);
		newItem.closeBtn->setPixmap(QPixmap("Resources/closeBtn.png"));
		newItem.closeBtn->setCursor(Qt::PointingHandCursor);
		ui.layerTable->setCellWidget(i, 3, newItem.closeBtn);
		connect(newItem.closeBtn, SIGNAL(clicked()), closeSignalMapper, SLOT(map()));
		closeSignalMapper->setMapping(newItem.closeBtn, i);

		layerItems.push_back(newItem);
	}
}

void Viewer::setCurrentLayer(int id) {
	currentLayerId = id;
	updateLayerDetail();
}

void Viewer::deleteLayer(int id) {
	int n = viewer3d->volumes.size();
	viewer3d->deleteVolume(id);
	updateAllViewers();
	updateLayers();
}

void Viewer::updateLayerDetail() {
	// Tab 1
	ui.layerName->setText(viewer3d->title[currentLayerId]);
	ui.colorR->setValue(viewer3d->color[currentLayerId].red());
	ui.colorG->setValue(viewer3d->color[currentLayerId].green());
	ui.colorB->setValue(viewer3d->color[currentLayerId].blue());
	ui.alpha->setValue(viewer3d->color[currentLayerId].alpha());
	ui.windowCenter->setValue(viewer3d->WindowCenter[currentLayerId]);
	ui.windowCenterVal->setText(QString::number(viewer3d->WindowCenter[currentLayerId]));
	ui.windowWidth->setValue(viewer3d->WindowWidth[currentLayerId]);
	ui.windowWidthVal->setText(QString::number(viewer3d->WindowWidth[currentLayerId]));
	ui.isoValue->setValue(viewer3d->isoValue[currentLayerId]);
	ui.isoValueVal->setText(QString::number(viewer3d->isoValue[currentLayerId]));

	// Tab 2
	ui.layerName1->setText(viewer3d->title[currentLayerId]);
}

void Viewer::updateAllViewers() {
	viewer3d->updateView();
	viewer_up->updateView();
	viewer_left->updateView();
	viewer_front->updateView();
}

void Viewer::updateLayerName(QString str) {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	viewer3d->title[currentLayerId] = str;
	updateLayers();
}

void Viewer::updateLayerColor() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	viewer3d->color[currentLayerId].setRed(ui.colorR->value());
	viewer3d->color[currentLayerId].setGreen(ui.colorG->value());
	viewer3d->color[currentLayerId].setBlue(ui.colorB->value());
	viewer3d->color[currentLayerId].setAlpha(ui.alpha->value());
	updateLayers();
	updateAllViewers();
}

void Viewer::updateLayerWindowCenterVal() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	int val = ui.windowCenter->value();
	viewer3d->WindowCenter[currentLayerId] = val;
	ui.windowCenterVal->setText(QString::number(val));
	updateAllViewers();
}

void Viewer::updateLayerWindowCenter() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	int val = ui.windowCenterVal->text().toInt();
	viewer3d->WindowCenter[currentLayerId] = val;
	ui.windowCenter->setValue(val);
	updateAllViewers();
}

void Viewer::updateLayerWindowWidthVal() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	int val = ui.windowWidth->value();
	viewer3d->WindowWidth[currentLayerId] = val;
	ui.windowWidthVal->setText(QString::number(val));
	updateAllViewers();
}

void Viewer::updateLayerWindowWidth() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	int val = ui.windowWidthVal->text().toInt();
	viewer3d->WindowWidth[currentLayerId] = val;
	ui.windowWidth->setValue(val);
	updateAllViewers();
}

void Viewer::updateLayerIsoValueVal() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	int val = ui.isoValue->value();
	viewer3d->isoValue[currentLayerId] = val;
	viewer3d->meshes[currentLayerId] = viewer3d->isoSurface(viewer3d->volumes[currentLayerId], val);
	ui.isoValueVal->setText(QString::number(val));
	updateAllViewers();
}

void Viewer::updateLayerIsoValue() {
	if (currentLayerId < 0 || currentLayerId >= viewer3d->volumes.size())
		return;
	int val = ui.isoValueVal->text().toInt();
	viewer3d->isoValue[currentLayerId] = val;
	viewer3d->meshes[currentLayerId] = viewer3d->isoSurface(viewer3d->volumes[currentLayerId], val);
	ui.isoValue->setValue(val);
	updateAllViewers();
}

void Viewer::updateAxesVisible(int state) {
	viewer3d->axesFlag = state > 0;
	viewer3d->updateView();
}

void Viewer::updateSliceVisible(int state) {
	viewer3d->sliceFlag = state > 0;
	viewer3d->updateView();
}
