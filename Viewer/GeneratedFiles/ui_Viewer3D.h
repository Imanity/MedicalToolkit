/********************************************************************************
** Form generated from reading UI file 'Viewer3D.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWER3D_H
#define UI_VIEWER3D_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Viewer3D
{
public:

    void setupUi(QWidget *Viewer3D)
    {
        if (Viewer3D->objectName().isEmpty())
            Viewer3D->setObjectName(QStringLiteral("Viewer3D"));
        Viewer3D->resize(400, 300);

        retranslateUi(Viewer3D);

        QMetaObject::connectSlotsByName(Viewer3D);
    } // setupUi

    void retranslateUi(QWidget *Viewer3D)
    {
        Viewer3D->setWindowTitle(QApplication::translate("Viewer3D", "Viewer3D", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Viewer3D: public Ui_Viewer3D {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWER3D_H
