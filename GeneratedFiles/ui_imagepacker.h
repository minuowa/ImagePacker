/********************************************************************************
** Form generated from reading UI file 'imagepacker.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEPACKER_H
#define UI_IMAGEPACKER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImagePackerClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_Scene;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QDockWidget *dockWidget_2;
    QWidget *dockWidgetContents_2;
    QGroupBox *groupBox;
    QPushButton *pushButton_Save;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QComboBox *comboBox_Config;
    QComboBox *comboBox_Image;
    QComboBox *comboBox_ProjectFile;

    void setupUi(QMainWindow *ImagePackerClass)
    {
        if (ImagePackerClass->objectName().isEmpty())
            ImagePackerClass->setObjectName(QStringLiteral("ImagePackerClass"));
        ImagePackerClass->resize(1177, 853);
        ImagePackerClass->setAcceptDrops(true);
        centralWidget = new QWidget(ImagePackerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_4 = new QVBoxLayout(centralWidget);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_Scene = new QHBoxLayout();
        horizontalLayout_Scene->setSpacing(1);
        horizontalLayout_Scene->setObjectName(QStringLiteral("horizontalLayout_Scene"));

        verticalLayout_4->addLayout(horizontalLayout_Scene);

        ImagePackerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ImagePackerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1177, 23));
        ImagePackerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ImagePackerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ImagePackerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        dockWidget = new QDockWidget(ImagePackerClass);
        dockWidget->setObjectName(QStringLiteral("dockWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
        dockWidget->setSizePolicy(sizePolicy);
        dockWidget->setMinimumSize(QSize(263, 721));
        dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        verticalLayout->addLayout(horizontalLayout);

        dockWidget->setWidget(dockWidgetContents);
        ImagePackerClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
        dockWidget_2 = new QDockWidget(ImagePackerClass);
        dockWidget_2->setObjectName(QStringLiteral("dockWidget_2"));
        dockWidget_2->setMinimumSize(QSize(316, 818));
        dockWidget_2->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        groupBox = new QGroupBox(dockWidgetContents_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 311, 141));
        pushButton_Save = new QPushButton(groupBox);
        pushButton_Save->setObjectName(QStringLiteral("pushButton_Save"));
        pushButton_Save->setGeometry(QRect(0, 100, 311, 31));
        verticalLayoutWidget_2 = new QWidget(groupBox);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(220, 10, 91, 83));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        pushButton_2 = new QPushButton(verticalLayoutWidget_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout_3->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(verticalLayoutWidget_2);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        verticalLayout_3->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(verticalLayoutWidget_2);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        verticalLayout_3->addWidget(pushButton_4);

        verticalLayoutWidget = new QWidget(groupBox);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 10, 211, 81));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        comboBox_Config = new QComboBox(verticalLayoutWidget);
        comboBox_Config->setObjectName(QStringLiteral("comboBox_Config"));
        comboBox_Config->setEditable(true);

        verticalLayout_2->addWidget(comboBox_Config);

        comboBox_Image = new QComboBox(verticalLayoutWidget);
        comboBox_Image->setObjectName(QStringLiteral("comboBox_Image"));
        comboBox_Image->setEditable(true);

        verticalLayout_2->addWidget(comboBox_Image);

        comboBox_ProjectFile = new QComboBox(verticalLayoutWidget);
        comboBox_ProjectFile->setObjectName(QStringLiteral("comboBox_ProjectFile"));
        comboBox_ProjectFile->setEditable(true);

        verticalLayout_2->addWidget(comboBox_ProjectFile);

        dockWidget_2->setWidget(dockWidgetContents_2);
        ImagePackerClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_2);

        retranslateUi(ImagePackerClass);

        QMetaObject::connectSlotsByName(ImagePackerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ImagePackerClass)
    {
        ImagePackerClass->setWindowTitle(QApplication::translate("ImagePackerClass", "ImagePacker", 0));
        groupBox->setTitle(QString());
        pushButton_Save->setText(QApplication::translate("ImagePackerClass", "\344\277\235\345\255\230", 0));
        pushButton_2->setText(QApplication::translate("ImagePackerClass", "XML...", 0));
        pushButton_3->setText(QApplication::translate("ImagePackerClass", "Image...", 0));
        pushButton_4->setText(QApplication::translate("ImagePackerClass", "ProjectFile", 0));
    } // retranslateUi

};

namespace Ui {
    class ImagePackerClass: public Ui_ImagePackerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEPACKER_H
