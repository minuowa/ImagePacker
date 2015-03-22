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
	QMenu* mMainMenu;
signals:
    void changed ( const QMimeData *mimeData = 0 );
public Q_SLOTS:
	void deleteImage();
protected:
    void dragEnterEvent ( QDragEnterEvent *event );
    void dragMoveEvent ( QDragMoveEvent *event );
    void dragLeaveEvent ( QDragLeaveEvent *event );
    void dropEvent ( QDropEvent *event );
	bool event ( QEvent *event );
	QString getSelectImage() const;
	QString mSeledtImage;

};

#endif // FILELISTTREEVIEW_H
