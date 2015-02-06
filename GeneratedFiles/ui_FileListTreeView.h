/********************************************************************************
** Form generated from reading UI file 'FileListTreeView.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILELISTTREEVIEW_H
#define UI_FILELISTTREEVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_FileListTreeView
{
public:

    void setupUi(QTreeView *FileListTreeView)
    {
        if (FileListTreeView->objectName().isEmpty())
            FileListTreeView->setObjectName(QStringLiteral("FileListTreeView"));
        FileListTreeView->resize(400, 300);
        if (FileListTreeView->header()->objectName().isEmpty())

        retranslateUi(FileListTreeView);

        QMetaObject::connectSlotsByName(FileListTreeView);
    } // setupUi

    void retranslateUi(QTreeView *FileListTreeView)
    {
        FileListTreeView->setWindowTitle(QApplication::translate("FileListTreeView", "FileListTreeView", 0));
    } // retranslateUi

};

namespace Ui {
    class FileListTreeView: public Ui_FileListTreeView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILELISTTREEVIEW_H
