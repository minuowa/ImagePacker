/********************************************************************************
** Form generated from reading UI file 'ScenePanel.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENEPANEL_H
#define UI_SCENEPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScenePanel
{
public:

    void setupUi(QWidget *ScenePanel)
    {
        if (ScenePanel->objectName().isEmpty())
            ScenePanel->setObjectName(QStringLiteral("ScenePanel"));
        ScenePanel->resize(400, 300);

        retranslateUi(ScenePanel);

        QMetaObject::connectSlotsByName(ScenePanel);
    } // setupUi

    void retranslateUi(QWidget *ScenePanel)
    {
        ScenePanel->setWindowTitle(QApplication::translate("ScenePanel", "ScenePanel", 0));
    } // retranslateUi

};

namespace Ui {
    class ScenePanel: public Ui_ScenePanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENEPANEL_H
