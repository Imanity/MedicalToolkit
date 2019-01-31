/********************************************************************************
** Form generated from reading UI file 'Viewer2D.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWER2D_H
#define UI_VIEWER2D_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Viewer2D
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;

    void setupUi(QWidget *Viewer2D)
    {
        if (Viewer2D->objectName().isEmpty())
            Viewer2D->setObjectName(QStringLiteral("Viewer2D"));
        Viewer2D->resize(400, 300);
        verticalLayout = new QVBoxLayout(Viewer2D);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(Viewer2D);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);


        retranslateUi(Viewer2D);

        QMetaObject::connectSlotsByName(Viewer2D);
    } // setupUi

    void retranslateUi(QWidget *Viewer2D)
    {
        Viewer2D->setWindowTitle(QApplication::translate("Viewer2D", "Viewer2D", nullptr));
        label->setText(QApplication::translate("Viewer2D", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Viewer2D: public Ui_Viewer2D {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWER2D_H
