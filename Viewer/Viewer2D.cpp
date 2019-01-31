#include "Viewer2D.h"

Viewer2D::Viewer2D(SLICE_PLANE plane, Viewer3D *viewer3d, QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);

	this->setStyleSheet("background-color: rgb(0, 0, 0)");
	ui.label->setMinimumSize(1, 1);

	this->plane = plane;
	this->viewer3d = viewer3d;
}

Viewer2D::~Viewer2D() {
}

void Viewer2D::updateView() {
	int img_size = this->width() < this->height() ? this->width() : this->height();
	cv::Mat img = viewer3d->generateSlice2d(plane, pos, 500);
	QImage qimg;
	qimg = QImage((const unsigned char *)(img.data), img.cols, img.rows, img.cols * img.channels(), QImage::Format_RGB888);
	int w = ui.label->width();
	int h = ui.label->height();
	ui.label->setPixmap(QPixmap::fromImage(qimg).scaled(w, h, Qt::KeepAspectRatio));
}

void Viewer2D::resizeEvent(QResizeEvent* event) {
	updateView();
	QWidget::resizeEvent(event);
}

void Viewer2D::wheelEvent(QWheelEvent* event) {
	if (event->delta() < 0 && pos > 1.0) {
		pos -= 1.0;
	}
	if (event->delta() > 0 && ((plane == SAGITTAL_PLANE && pos < viewer3d->lenX - 1.0) || 
		(plane == CORONAL_PLANE && pos < viewer3d->lenY - 1.0) || (plane == TRANSVERSE_PLANE && pos < viewer3d->lenZ - 1.0))) {
		pos += 1.0;
	}
	updateView();
}
