#include "stdafx.h"
#include "FileListTreeView.h"
#include <sys\stat.h>
#include "XImagePacker.h"

static const char* FileFlag = "file:///";

FileListTreeView::FileListTreeView ( QWidget *parent )
    : QTreeView ( parent )
{
    ui.setupUi ( this );
}

FileListTreeView::~FileListTreeView()
{

}
void FileListTreeView::dragEnterEvent ( QDragEnterEvent *event )
{
    event->acceptProposedAction();
    emit changed ( event->mimeData() );
}
//! [dragEnterEvent() function]

//! [dragMoveEvent() function]
void FileListTreeView::dragMoveEvent ( QDragMoveEvent *event )
{
    event->acceptProposedAction();
}
//! [dragMoveEvent() function]

//! [dropEvent() function part1]
void FileListTreeView::dropEvent ( QDropEvent *event )
{
    const QMimeData *mimeData = event->mimeData();
    //! [dropEvent() function part1]

    //! [dropEvent() function part2]
    if ( mimeData->hasImage() )
    {
    }
    else if ( mimeData->hasHtml() )
    {
    }
    else if ( mimeData->hasText() )
    {
        QString text = mimeData->text();
        QModelIndex idx = indexAt ( event->pos() );
        if ( model() )
        {
            QVariant var = model()->data ( idx );
            if ( !var.isValid() )
            {
                GString str = text.toStdString();
                CXDynaArray<GString> strFileList;
                str.Splite ( '\n', strFileList );

                if ( strFileList.size() == 0 )
                    strFileList.push_back ( str );

for ( GString strFile: strFileList )
                {
                    strFile = strFile.substr ( strlen ( FileFlag ), -1 );
                    if ( dIsPath ( strFile ) )
                    {
                        gImagePacker.addPath ( strFile );
                    }
                    else
                    {
                        gImagePacker.addTexture ( strFile );
                    }
                }
            }
            if ( var.type() == QMetaType::QString )
            {
                //model()->setData ( idx, relname.c_str() );
            }
        }
    }
    else if ( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        QString text;
        for ( int i = 0; i < urlList.size() && i < 32; ++i )
        {
            QString url = urlList.at ( i ).path();
            CXASSERT ( url.toStdString().c_str() );
            text += url + QString ( "\n" );
        }
        QModelIndex idx = indexAt ( event->pos() );
        if ( model() )
        {
            QVariant var = model()->data ( idx );
            if ( var.type() == QMetaType::QString )
            {
                text = text.right ( text.length() - 1 );
                CXFileName path ( text.toStdString().c_str() );
                GString relname;
                model()->setData ( idx, path.GetRelativeFileName () );
            }
        }
    }
    else
    {
    }
    //! [dropEvent() function part2]

    //! [dropEvent() function part3]
    setBackgroundRole ( QPalette::Dark );
    event->acceptProposedAction();
}
//! [dropEvent() function part3]

//! [dragLeaveEvent() function]
void FileListTreeView::dragLeaveEvent ( QDragLeaveEvent *event )
{
    event->accept();
}

void FileListTreeView::selectItem ( QStandardItem* item )
{
    //setSelection(item->isDragEnabled())
}

