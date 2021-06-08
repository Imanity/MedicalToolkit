#include "ViewerDSA.h"

#include <QPainter>

ViewerDSA::ViewerDSA(Viewer3D* viewer3d, QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);

	this->setStyleSheet("background-color: rgb(0, 0, 0)");
	ui.label->setMinimumSize(1, 1);

	this->viewer3d = viewer3d;

	connect(this, SIGNAL(select_pos(int)), parent, SLOT(selectDSAPos(int)));
}

ViewerDSA::~ViewerDSA() {
}

void ViewerDSA::updateView() {
	if (image_idx < 0 || image_idx >= viewer3d->dsaImages.size()) {
		return;
	}
	if (pos_idx < 0 || pos_idx >= viewer3d->dsaImages[image_idx].nz) {
		return;
	}
	cv::Mat img = viewer3d->generateDSA2d(image_idx, pos_idx, this->rect().width());
	QImage qimg;
	qimg = QImage((const unsigned char*)(img.data), img.cols, img.rows, img.cols * img.channels(), QImage::Format_RGB888);
	QPainter pp(&qimg);
	QPen pen = QPen(Qt::blue, 6);
	QBrush brush = QBrush(Qt::blue);
	pp.setPen(pen);
	pp.setBrush(brush);
	pp.drawText(QPointF(25, 15), QString::number(pos_idx) + " / " + QString::number(viewer3d->dsaImages[image_idx].nz));
	int w = ui.label->width();
	int h = ui.label->height();
	this->setFixedHeight(img.rows);
	ui.label->setPixmap(QPixmap::fromImage(qimg).scaled(w, h, Qt::KeepAspectRatio));
}

void ViewerDSA::resizeEvent(QResizeEvent* event) {
	updateView();
	QWidget::resizeEvent(event);
}

void ViewerDSA::wheelEvent(QWheelEvent* event) {
	if (image_idx < 0 || image_idx >= viewer3d->dsaImages.size()) {
		return;
	}
	if (event->delta() > 0 && pos_idx > 0) {
		pos_idx -= 1;
	}
	if (event->delta() < 0 && pos_idx < viewer3d->dsaImages[image_idx].nz) {
		pos_idx += 1;
	}
	if (pos_idx < 16) {
		viewer3d->currentFrame = pos_idx;
		viewer3d->updateView();
	}

	emit select_pos(pos_idx);
	updateView();
}
