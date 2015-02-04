#ifndef FILELISTTREEVIEW_H
#define FILELISTTREEVIEW_H

#include <QTreeView>
#include "ui_FileListTreeView.h"

class FileListTreeView : public QTreeView
{
    Q_OBJECT

public:
    FileListTreeView ( QWidget *parent = 0 );
    ~FileListTreeView();

	void selectItem(QStandardItem* item);
private:
    Ui::FileListTreeView ui;
signals:
    void changed ( const QMimeData *mimeData = 0 );
public Q_SLOTS:
protected:
    void dragEnterEvent ( QDragEnterEvent *event );
    void dragMoveEvent ( QDragMoveEvent *event );
    void dragLeaveEvent ( QDragLeaveEvent *event );
    void dropEvent ( QDropEvent *event );
};

#endif // FILELISTTREEVIEW_H
