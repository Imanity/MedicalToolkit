#pragma once

#include <QDialog>
#include "ui_DialogVesselExtractor.h"

class DialogVesselExtractor : public QDialog {
	Q_OBJECT

public:
	DialogVesselExtractor(QWidget *parent = Q_NULLPTR, const std::vector<QString> &titles = std::vector<QString>());
	~DialogVesselExtractor();

public slots:
	void onConfirm();

signals:
	void extractVessel(int i, int j);

private:
	Ui::DialogVesselExtractor ui;
};
