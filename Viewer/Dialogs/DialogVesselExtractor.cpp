#include "DialogVesselExtractor.h"

DialogVesselExtractor::DialogVesselExtractor(QWidget *parent, const std::vector<QString> &titles) : QDialog(parent) {
	ui.setupUi(this);

	for (int i = 0; i < titles.size(); ++i) {
		ui.nonContrastCTComboBox->addItem(titles[i]);
		ui.enhanceCTComboBox->addItem(titles[i]);
	}

	connect(ui.confirmBtn, SIGNAL(clicked()), this, SLOT(onConfirm()));
	connect(this, SIGNAL(extractVessel(int, int)), parent, SLOT(extractVessel(int, int)));
}

DialogVesselExtractor::~DialogVesselExtractor() {
}

void DialogVesselExtractor::onConfirm() {
	int i = ui.nonContrastCTComboBox->currentIndex();
	int j = ui.enhanceCTComboBox->currentIndex();

	emit extractVessel(i, j);

	this->close();
}
