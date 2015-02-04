#include "stdafx.h"
#include "imagepacker.h"
#include "XImagePacker.h"
#include "FileListTreeView.h"
#include "FiGameDemo.h"
#include "GGameDemoHeader.h"
#include "GGame.h"
#include "ScenePanel.h"
#include "GText.h"
#include "GUINode.h"

ImagePacker::ImagePacker ( QWidget *parent )
    : QMainWindow ( parent )
    , mIdleTimeID ( 0 )
{
    ui.setupUi ( this );

    CXASSERT ( this->acceptDrops() );

    mTreeModel = new QStandardItemModel();
    mFileTree = new FileListTreeView ( ui.dockWidgetContents );
    mFileTree->setModel ( mTreeModel );
    mFileTree->setAcceptDrops ( true );
    ui.horizontalLayout->addWidget ( mFileTree );

    ui.dockWidget->setWindowTitle ( "FileList" );
    ui.dockWidget->setAcceptDrops ( true );
    ui.dockWidgetContents->setAcceptDrops ( true );

    ui.dockWidget_2->setWindowTitle ( "Config" );
    ui.dockWidget_2->setAcceptDrops ( true );
    ui.dockWidgetContents_2->setAcceptDrops ( true );


    connect ( ui.pushButton_2, SIGNAL ( clicked() ), this, SLOT ( browseConfig() ) );
    connect ( ui.pushButton_3, SIGNAL ( clicked() ), this, SLOT ( browseImage() ) );

    connect ( ui.pushButton_Save, SIGNAL ( clicked() ), this, SLOT ( saveConfig() ) );

    connect ( mFileTree, SIGNAL ( clicked ( const QModelIndex & ) ), this, SLOT ( onClicked ( const QModelIndex & ) ) );


    ui.comboBox_Config->setEditable ( true );
    ui.comboBox_Image->setEditable ( true );

    mIdleTimeID = startTimer ( 0 );

    gImagePacker.mDelegateAddPath += this;
    gImagePacker.mDelegateAddTexture += this;
    gImagePacker.mDelegateAddTextureFailed += this;


    mScenePanel = new ScenePanel ( ui.centralWidget );
    ui.horizontalLayout_Scene->addWidget ( mScenePanel );

    if ( !FiGameDemo_Init ( ( HWND ) ui.centralWidget->winId() ) )
    {
        assert ( 0 );
    }
    D9DEVICE->setBackColor ( 0 );
    TextMgr->mDelegateOnDrawTextBegin += this;

    UIMgr->mDelegateHoverNodeChanged += this;

    mCanvos = new TextureCanvos;

    mSelectNode = new GRectNode;
    mSelectNode->setColor ( Color_Pure_Green );
    mSelectNode->setState ( eObjState_Render, false );
    mSelectNode->recreate();
}

ImagePacker::~ImagePacker()
{
    mTextureMap.destroySecond();

    gImagePacker.mDelegateAddPath -= this;
    gImagePacker.mDelegateAddTexture -= this;
    gImagePacker.mDelegateAddTextureFailed -= this;

    TextMgr->mDelegateOnDrawTextBegin -= this;
    UIMgr->mDelegateHoverNodeChanged -= this;

    dSafeDelete ( mCanvos );

	FiGameDemo_ShutDown();
}


void ImagePacker::onCallBack ( const CXDelegate& d, CXEventArgs* e )
{
    if ( d == gImagePacker.mDelegateAddPath )
    {
        CXAddTexturePathArg* arg = ( CXAddTexturePathArg* ) e;
        addPathTexture ( arg );
    }
    else if ( d == gImagePacker.mDelegateAddTexture )
    {
        CXAddTextureArg* arg = ( CXAddTextureArg* ) e;
        directAddTexture ( arg );
        onCalTexturePos();
    }
    else if ( d == gImagePacker.mDelegateAddTextureFailed )
    {
        QMessageBox::warning ( nullptr, "", "Canvos is to small!" );
    }
    else if ( TextMgr->mDelegateOnDrawTextBegin == d )
    {
        mSelectNode->draw();
    }
    else if ( d == UIMgr->mDelegateHoverNodeChanged )
    {
        GUIHoverNodeChangedEvent* arg = ( GUIHoverNodeChangedEvent* ) e;
        onChangeHoveredNode ( arg );
    }
}

bool ImagePacker::findTreeItem ( QStandardItemModel* model, const char* fileOrPath, QStandardItem*& outItem )
{
    int row = model->rowCount();
    int col = model->columnCount();
    for ( int i = 0; i < row; ++i )
    {
        for ( int j = 0; j < col; ++j )
        {
            QStandardItem* child = model->item ( i, j );
            if ( child && findItem ( child, fileOrPath, outItem ) )
            {
                return true;
            }
        }
    }
    return false;
}


bool ImagePacker::findItem ( QStandardItem* item, const char* fileOrPath, QStandardItem*& outItem )
{
    QVariant var = item->data();
    if ( dStrEqual ( var.toString().toStdString().c_str(), fileOrPath ) )
    {
        outItem = item;
        return true;
    }

    int row = item->rowCount();
    int col = item->columnCount();
    for ( int i = 0; i < row; ++i )
    {
        for ( int j = 0; j < col; ++j )
        {
            QStandardItem* child = item->child ( i, j );
            if ( child && findItem ( child, fileOrPath, outItem ) )
            {
                return true;
            }
        }
    }
    return false;
}

void ImagePacker::browseConfig()
{
    QString directory = QFileDialog::getSaveFileName ( this, tr ( "Find Files" ), QDir::currentPath(), "*.xml" );

    if ( !directory.isEmpty() )
    {
        if ( ui.comboBox_Config->findText ( directory ) == -1 )
            ui.comboBox_Config->addItem ( directory );
    }
}

void ImagePacker::browseImage()
{
    QString directory = QFileDialog::getSaveFileName ( this, tr ( "Find Files" ), QDir::currentPath(), "*.tga;*.dds" );

    if ( !directory.isEmpty() )
    {
        if ( ui.comboBox_Image->findText ( directory ) == -1 )
            ui.comboBox_Image->addItem ( directory );
    }
}

void ImagePacker::saveConfig()
{
    QString outTextureName = ui.comboBox_Image->currentText();
    QString outTextName = ui.comboBox_Config->currentText();

    if ( outTextName.isEmpty() || outTextName.isEmpty() )
    {
        return;
    }
    gImagePacker.save ( outTextureName.toStdString().c_str(), outTextName.toStdString().c_str(), 1024, 1024, FIF_TARGA );
}

void ImagePacker::timerEvent ( QTimerEvent * timeevent )
{
    if ( timeevent->timerId() == mIdleTimeID )
    {
        if ( !FiGameDemo_Update() )
        {
            close();
        }
    }
    else
    {
        __super::timerEvent ( timeevent );
    }
}
bool ImagePacker::event ( QEvent * event )
{
    switch ( event->type() )
    {
    case  QEvent::WindowActivate:
    {
        TheGame->active ( true );
    }
    break;
    case QEvent::WindowDeactivate :
    {
        TheGame->active ( false );
    }
    break;
    default:
        break;
    }
    return __super::event ( event );
}

void ImagePacker::onClicked ( const QModelIndex &index )
{
    QStandardItem* item = mTreeModel->itemFromIndex ( index );
    CXASSERT ( item );
    QVariant var = item->data();
    GString str = var.toString().toStdString();
for ( auto tex: mTextureMap )
    {
        TextureInfo* texture = tex.second;
        texture->mSelected = false;
        if ( tex.first.equal ( str ) )
        {
            texture->mSelected = true;
        }
    }
}


bool ImagePacker::getItemString ( const CXRect& rc, GString& str )
{
for ( auto tex: mTextureMap )
    {
        TextureInfo* texture = tex.second;
        if ( texture->mTextureDim->mRect == rc )
        {
            str = texture->mTextureDim->mFileName;
            return true;
        }
    }
    return false;
}

void ImagePacker::selectTreeItem ( const char* orignalFileName )
{

}

void ImagePacker::addPathTexture ( CXAddTexturePathArg* arg )
{
    QStandardItem *pathItem = 0, *newItem = 0;
    CXFileName filename ( arg->mName );

    //已存在
    if ( findTreeItem ( mTreeModel, filename.GetOrignalName(), newItem ) )
    {
        return;
    }

    //父目录存在
    newItem = new QStandardItem ( filename.GetFileName() );
    newItem->setData ( filename.GetOrignalName() );
    newItem->setEditable ( false );
    if ( findTreeItem ( mTreeModel, filename.GetAbsolutePath(), pathItem ) )
    {
        pathItem->appendRow ( newItem );
    }
    else
    {
        mTreeModel->appendRow ( newItem );
    }
}

void ImagePacker::directAddTexture ( CXAddTextureArg* arg )
{
    QStandardItem *pathItem = 0, *newItem = 0;
    GString name, parentPath;
    CXFileName filename ( arg->mName );
    filename.GetParentPath ( parentPath );
    newItem = new QStandardItem ( filename.GetFileName () );
    newItem->setData ( filename.GetOrignalName() );
    newItem->setEditable ( false );
    GTexture* texture = TextureMgr->getResource ( filename.GetOrignalName() );
    if ( texture )
    {
        TextureInfo* info = new TextureInfo;
        info->mImage = texture;
        info->mTextureDim = gImagePacker.getTexture ( filename.GetOrignalName() );
        mTextureMap.Insert ( filename.GetOrignalName(), info );
    }

    if ( findTreeItem ( mTreeModel, filename.GetAbsolutePath(), pathItem ) )
    {
        pathItem->appendRow ( newItem );
    }
    else
    {
        mTreeModel->appendRow ( newItem );
    }
}

void ImagePacker::onChangeHoveredNode ( GUIHoverNodeChangedEvent* arg )
{
    mSelectNode->setState ( eObjState_Render, arg->mNewNode != nullptr );
    if ( arg->mNewNode && arg->mNewNode->getRect() != mSelectNode->getRect() )
    {
        mSelectNode->setRect ( arg->mNewNode->getRect() );
        GString str;
        if ( getItemString ( arg->mNewNode->getRect(), str ) )
        {
            QStandardItem* newItem = nullptr;
            if ( findTreeItem ( mTreeModel, str.c_str(), newItem ) )
            {
                newItem->setCheckState ( Qt::CheckState::Checked );
                //mTreeModel->
            }
        }
    }
}

void ImagePacker::onCalTexturePos()
{
    mCanvos->recreate();

for ( auto tex: mTextureMap )
    {
        TextureInfo* texture = tex.second;
        GUINode* node = new GUINode;
        node->setRect ( texture->mTextureDim->mRect );
        node->setTexture ( texture->mTextureDim->mFileName.c_str() );
        node->recreate();
        mCanvos->getCanvos()->addChild ( node );
    }
}

TextureInfo::TextureInfo()
{
    mSelected = false;
    mTextureDim = nullptr;
    mImage = nullptr;
}
