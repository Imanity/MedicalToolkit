/********************************************************************************
** Form generated from reading UI file 'Viewer.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWER_H
#define UI_VIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewerClass
{
public:
    QAction *actionOpenDicom;
    QWidget *centralWidget;
    QHBoxLayout *centralLayout;
    QVBoxLayout *leftLayout;
    QGridLayout *propertyLayout;
    QSpacerItem *verticalSpacer;
    QLabel *colorGLabel;
    QSlider *alpha;
    QSlider *colorB;
    QLabel *colorBLabel;
    QLabel *windowCenterLabel;
    QHBoxLayout *windowWidthLayout;
    QSlider *windowWidth;
    QLineEdit *windowWidthVal;
    QHBoxLayout *windowCenterLayout;
    QSlider *windowCenter;
    QLineEdit *windowCenterVal;
    QSlider *colorG;
    QLabel *layerNameLabel;
    QLineEdit *layerName;
    QLabel *windowWidthLabel;
    QSlider *colorR;
    QLabel *colorRLabel;
    QLabel *alphaLabel;
    QLabel *isoValueLabel;
    QHBoxLayout *isoValueLayout;
    QSlider *isoValue;
    QLineEdit *isoValueVal;
    QVBoxLayout *settingLayout;
    QTabWidget *tabWidget;
    QWidget *tab1;
    QVBoxLayout *verticalLayout;
    QTableWidget *layerTable;
    QWidget *tab2;
    QGridLayout *gridLayout_2;
    QLabel *ambientLabel;
    QSlider *diffuseSlider;
    QLabel *SpecularLabel;
    QSlider *SpecularSlider;
    QLabel *renderLabel;
    QLabel *diffuseLabel;
    QSlider *ambientSlider;
    QComboBox *renderComboBox;
    QVBoxLayout *mainLayout;
    QVBoxLayout *sliceLayout;
    QHBoxLayout *sliceLayout1;
    QHBoxLayout *sliceLayout2;
    QHBoxLayout *sliceLayout3;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *ViewerClass)
    {
        if (ViewerClass->objectName().isEmpty())
            ViewerClass->setObjectName(QStringLiteral("ViewerClass"));
        ViewerClass->resize(1080, 720);
        actionOpenDicom = new QAction(ViewerClass);
        actionOpenDicom->setObjectName(QStringLiteral("actionOpenDicom"));
        centralWidget = new QWidget(ViewerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralLayout = new QHBoxLayout(centralWidget);
        centralLayout->setSpacing(1);
        centralLayout->setContentsMargins(11, 11, 11, 11);
        centralLayout->setObjectName(QStringLiteral("centralLayout"));
        centralLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout = new QVBoxLayout();
        leftLayout->setSpacing(6);
        leftLayout->setObjectName(QStringLiteral("leftLayout"));
        propertyLayout = new QGridLayout();
        propertyLayout->setSpacing(6);
        propertyLayout->setObjectName(QStringLiteral("propertyLayout"));
        propertyLayout->setContentsMargins(5, 5, 5, 5);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        propertyLayout->addItem(verticalSpacer, 9, 0, 1, 1);

        colorGLabel = new QLabel(centralWidget);
        colorGLabel->setObjectName(QStringLiteral("colorGLabel"));

        propertyLayout->addWidget(colorGLabel, 3, 0, 1, 1);

        alpha = new QSlider(centralWidget);
        alpha->setObjectName(QStringLiteral("alpha"));
        alpha->setMaximum(255);
        alpha->setValue(255);
        alpha->setOrientation(Qt::Horizontal);

        propertyLayout->addWidget(alpha, 5, 1, 1, 1);

        colorB = new QSlider(centralWidget);
        colorB->setObjectName(QStringLiteral("colorB"));
        colorB->setMaximum(255);
        colorB->setValue(255);
        colorB->setOrientation(Qt::Horizontal);

        propertyLayout->addWidget(colorB, 4, 1, 1, 1);

        colorBLabel = new QLabel(centralWidget);
        colorBLabel->setObjectName(QStringLiteral("colorBLabel"));

        propertyLayout->addWidget(colorBLabel, 4, 0, 1, 1);

        windowCenterLabel = new QLabel(centralWidget);
        windowCenterLabel->setObjectName(QStringLiteral("windowCenterLabel"));

        propertyLayout->addWidget(windowCenterLabel, 6, 0, 1, 1);

        windowWidthLayout = new QHBoxLayout();
        windowWidthLayout->setSpacing(6);
        windowWidthLayout->setObjectName(QStringLiteral("windowWidthLayout"));
        windowWidth = new QSlider(centralWidget);
        windowWidth->setObjectName(QStringLiteral("windowWidth"));
        windowWidth->setMinimum(1);
        windowWidth->setMaximum(1000);
        windowWidth->setSingleStep(10);
        windowWidth->setPageStep(100);
        windowWidth->setValue(800);
        windowWidth->setOrientation(Qt::Horizontal);

        windowWidthLayout->addWidget(windowWidth);

        windowWidthVal = new QLineEdit(centralWidget);
        windowWidthVal->setObjectName(QStringLiteral("windowWidthVal"));

        windowWidthLayout->addWidget(windowWidthVal);

        windowWidthLayout->setStretch(0, 5);
        windowWidthLayout->setStretch(1, 1);

        propertyLayout->addLayout(windowWidthLayout, 7, 1, 1, 1);

        windowCenterLayout = new QHBoxLayout();
        windowCenterLayout->setSpacing(6);
        windowCenterLayout->setObjectName(QStringLiteral("windowCenterLayout"));
        windowCenter = new QSlider(centralWidget);
        windowCenter->setObjectName(QStringLiteral("windowCenter"));
        windowCenter->setMinimum(1);
        windowCenter->setMaximum(1000);
        windowCenter->setSingleStep(10);
        windowCenter->setPageStep(100);
        windowCenter->setValue(200);
        windowCenter->setOrientation(Qt::Horizontal);

        windowCenterLayout->addWidget(windowCenter);

        windowCenterVal = new QLineEdit(centralWidget);
        windowCenterVal->setObjectName(QStringLiteral("windowCenterVal"));

        windowCenterLayout->addWidget(windowCenterVal);

        windowCenterLayout->setStretch(0, 5);
        windowCenterLayout->setStretch(1, 1);

        propertyLayout->addLayout(windowCenterLayout, 6, 1, 1, 1);

        colorG = new QSlider(centralWidget);
        colorG->setObjectName(QStringLiteral("colorG"));
        colorG->setMaximum(255);
        colorG->setValue(255);
        colorG->setOrientation(Qt::Horizontal);

        propertyLayout->addWidget(colorG, 3, 1, 1, 1);

        layerNameLabel = new QLabel(centralWidget);
        layerNameLabel->setObjectName(QStringLiteral("layerNameLabel"));

        propertyLayout->addWidget(layerNameLabel, 0, 0, 1, 1);

        layerName = new QLineEdit(centralWidget);
        layerName->setObjectName(QStringLiteral("layerName"));

        propertyLayout->addWidget(layerName, 0, 1, 1, 1);

        windowWidthLabel = new QLabel(centralWidget);
        windowWidthLabel->setObjectName(QStringLiteral("windowWidthLabel"));

        propertyLayout->addWidget(windowWidthLabel, 7, 0, 1, 1);

        colorR = new QSlider(centralWidget);
        colorR->setObjectName(QStringLiteral("colorR"));
        colorR->setMaximum(255);
        colorR->setValue(255);
        colorR->setOrientation(Qt::Horizontal);

        propertyLayout->addWidget(colorR, 2, 1, 1, 1);

        colorRLabel = new QLabel(centralWidget);
        colorRLabel->setObjectName(QStringLiteral("colorRLabel"));

        propertyLayout->addWidget(colorRLabel, 2, 0, 1, 1);

        alphaLabel = new QLabel(centralWidget);
        alphaLabel->setObjectName(QStringLiteral("alphaLabel"));

        propertyLayout->addWidget(alphaLabel, 5, 0, 1, 1);

        isoValueLabel = new QLabel(centralWidget);
        isoValueLabel->setObjectName(QStringLiteral("isoValueLabel"));

        propertyLayout->addWidget(isoValueLabel, 8, 0, 1, 1);

        isoValueLayout = new QHBoxLayout();
        isoValueLayout->setSpacing(6);
        isoValueLayout->setObjectName(QStringLiteral("isoValueLayout"));
        isoValue = new QSlider(centralWidget);
        isoValue->setObjectName(QStringLiteral("isoValue"));
        isoValue->setMinimum(1);
        isoValue->setMaximum(1000);
        isoValue->setSingleStep(10);
        isoValue->setPageStep(100);
        isoValue->setValue(200);
        isoValue->setOrientation(Qt::Horizontal);

        isoValueLayout->addWidget(isoValue);

        isoValueVal = new QLineEdit(centralWidget);
        isoValueVal->setObjectName(QStringLiteral("isoValueVal"));

        isoValueLayout->addWidget(isoValueVal);

        isoValueLayout->setStretch(0, 5);
        isoValueLayout->setStretch(1, 1);

        propertyLayout->addLayout(isoValueLayout, 8, 1, 1, 1);

        propertyLayout->setColumnStretch(0, 1);
        propertyLayout->setColumnStretch(1, 3);

        leftLayout->addLayout(propertyLayout);

        settingLayout = new QVBoxLayout();
        settingLayout->setSpacing(6);
        settingLayout->setObjectName(QStringLiteral("settingLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tab1 = new QWidget();
        tab1->setObjectName(QStringLiteral("tab1"));
        verticalLayout = new QVBoxLayout(tab1);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        layerTable = new QTableWidget(tab1);
        layerTable->setObjectName(QStringLiteral("layerTable"));

        verticalLayout->addWidget(layerTable);

        tabWidget->addTab(tab1, QString());
        tab2 = new QWidget();
        tab2->setObjectName(QStringLiteral("tab2"));
        gridLayout_2 = new QGridLayout(tab2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        ambientLabel = new QLabel(tab2);
        ambientLabel->setObjectName(QStringLiteral("ambientLabel"));

        gridLayout_2->addWidget(ambientLabel, 1, 0, 1, 1);

        diffuseSlider = new QSlider(tab2);
        diffuseSlider->setObjectName(QStringLiteral("diffuseSlider"));
        diffuseSlider->setValue(80);
        diffuseSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(diffuseSlider, 2, 1, 1, 1);

        SpecularLabel = new QLabel(tab2);
        SpecularLabel->setObjectName(QStringLiteral("SpecularLabel"));

        gridLayout_2->addWidget(SpecularLabel, 3, 0, 1, 1);

        SpecularSlider = new QSlider(tab2);
        SpecularSlider->setObjectName(QStringLiteral("SpecularSlider"));
        SpecularSlider->setValue(20);
        SpecularSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(SpecularSlider, 3, 1, 1, 1);

        renderLabel = new QLabel(tab2);
        renderLabel->setObjectName(QStringLiteral("renderLabel"));

        gridLayout_2->addWidget(renderLabel, 0, 0, 1, 1);

        diffuseLabel = new QLabel(tab2);
        diffuseLabel->setObjectName(QStringLiteral("diffuseLabel"));

        gridLayout_2->addWidget(diffuseLabel, 2, 0, 1, 1);

        ambientSlider = new QSlider(tab2);
        ambientSlider->setObjectName(QStringLiteral("ambientSlider"));
        ambientSlider->setValue(0);
        ambientSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(ambientSlider, 1, 1, 1, 1);

        renderComboBox = new QComboBox(tab2);
        renderComboBox->addItem(QString());
        renderComboBox->addItem(QString());
        renderComboBox->setObjectName(QStringLiteral("renderComboBox"));

        gridLayout_2->addWidget(renderComboBox, 0, 1, 1, 1);

        tabWidget->addTab(tab2, QString());

        settingLayout->addWidget(tabWidget);


        leftLayout->addLayout(settingLayout);

        leftLayout->setStretch(0, 3);
        leftLayout->setStretch(1, 2);

        centralLayout->addLayout(leftLayout);

        mainLayout = new QVBoxLayout();
        mainLayout->setSpacing(6);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));

        centralLayout->addLayout(mainLayout);

        sliceLayout = new QVBoxLayout();
        sliceLayout->setSpacing(1);
        sliceLayout->setObjectName(QStringLiteral("sliceLayout"));
        sliceLayout1 = new QHBoxLayout();
        sliceLayout1->setSpacing(0);
        sliceLayout1->setObjectName(QStringLiteral("sliceLayout1"));

        sliceLayout->addLayout(sliceLayout1);

        sliceLayout2 = new QHBoxLayout();
        sliceLayout2->setSpacing(0);
        sliceLayout2->setObjectName(QStringLiteral("sliceLayout2"));

        sliceLayout->addLayout(sliceLayout2);

        sliceLayout3 = new QHBoxLayout();
        sliceLayout3->setSpacing(0);
        sliceLayout3->setObjectName(QStringLiteral("sliceLayout3"));

        sliceLayout->addLayout(sliceLayout3);

        sliceLayout->setStretch(0, 1);
        sliceLayout->setStretch(1, 1);
        sliceLayout->setStretch(2, 1);

        centralLayout->addLayout(sliceLayout);

        centralLayout->setStretch(0, 2);
        centralLayout->setStretch(1, 6);
        centralLayout->setStretch(2, 2);
        ViewerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ViewerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1080, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        ViewerClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(ViewerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ViewerClass->setStatusBar(statusBar);
        toolBar = new QToolBar(ViewerClass);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        ViewerClass->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpenDicom);

        retranslateUi(ViewerClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ViewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ViewerClass)
    {
        ViewerClass->setWindowTitle(QApplication::translate("ViewerClass", "Viewer", nullptr));
        actionOpenDicom->setText(QApplication::translate("ViewerClass", "\346\211\223\345\274\200Dicom", nullptr));
        colorGLabel->setText(QApplication::translate("ViewerClass", "G\357\274\232", nullptr));
        colorBLabel->setText(QApplication::translate("ViewerClass", "B\357\274\232", nullptr));
        windowCenterLabel->setText(QApplication::translate("ViewerClass", "\347\252\227\344\275\215\357\274\232", nullptr));
        windowWidthVal->setText(QApplication::translate("ViewerClass", "800", nullptr));
        windowCenterVal->setText(QApplication::translate("ViewerClass", "200", nullptr));
        layerNameLabel->setText(QApplication::translate("ViewerClass", "\345\233\276\345\261\202\357\274\232", nullptr));
        windowWidthLabel->setText(QApplication::translate("ViewerClass", "\347\252\227\345\256\275\357\274\232", nullptr));
        colorRLabel->setText(QApplication::translate("ViewerClass", "R\357\274\232", nullptr));
        alphaLabel->setText(QApplication::translate("ViewerClass", "\351\200\217\346\230\216\345\272\246\357\274\232", nullptr));
        isoValueLabel->setText(QApplication::translate("ViewerClass", "\347\255\211\345\200\274\351\235\242", nullptr));
        isoValueVal->setText(QApplication::translate("ViewerClass", "200", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab1), QApplication::translate("ViewerClass", "\346\230\276\347\244\272\345\233\276\345\261\202", nullptr));
        ambientLabel->setText(QApplication::translate("ViewerClass", "\347\216\257\345\242\203\345\205\211", nullptr));
        SpecularLabel->setText(QApplication::translate("ViewerClass", "\351\225\234\351\235\242\345\217\215\345\260\204", nullptr));
        renderLabel->setText(QApplication::translate("ViewerClass", "\347\273\230\345\210\266\346\226\271\345\274\217", nullptr));
        diffuseLabel->setText(QApplication::translate("ViewerClass", "\346\274\253\345\217\215\345\260\204", nullptr));
        renderComboBox->setItemText(0, QApplication::translate("ViewerClass", "\351\235\242\347\273\230\345\210\266", nullptr));
        renderComboBox->setItemText(1, QApplication::translate("ViewerClass", "\344\275\223\347\273\230\345\210\266", nullptr));

        tabWidget->setTabText(tabWidget->indexOf(tab2), QApplication::translate("ViewerClass", "\347\273\230\345\210\266\345\261\236\346\200\247", nullptr));
        menuFile->setTitle(QApplication::translate("ViewerClass", "\346\226\207\344\273\266", nullptr));
        toolBar->setWindowTitle(QApplication::translate("ViewerClass", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewerClass: public Ui_ViewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWER_H
