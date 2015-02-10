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
    , mOptionSetting ( AppName, Option )
{

    ui.setupUi ( this );

    createMenus();

    createFileListPanel();

    createConfigPanel();

    createScenePanel();
}

ImagePacker::~ImagePacker()
{
    mTextureMap.destroySecond();

    gImagePacker.mDelegateAddPath -= this;
    gImagePacker.mDelegateAddTexture -= this;
    gImagePacker.mDelegateAddTextureFailed -= this;
    gImagePacker.mDelegateSettingImageFile -= this;

    TextMgr->mDelegateOnDrawTextBegin -= this;
    UIMgr->mDelegateHoverNodeChanged -= this;

    dSafeDelete ( mCanvos );

    FiGameDemo_ShutDown();
}


void ImagePacker::onCallBack ( const CXDelegate& d, CXEventArgs* e )
{
    if ( d == gImagePacker.mDelegateAddPath )
    {
        CXAddTextureArg* arg = ( CXAddTextureArg* ) e;
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
    else if ( d == gImagePacker.mDelegateSettingImageFile )
    {
        ui.comboBox_ProjectFile->setCurrentText ( gImagePacker.getProjectFile() );
        ui.comboBox_Image->setCurrentText ( gImagePacker.getImageFile() );
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


void ImagePacker::browseImage()
{
    QString directory = QFileDialog::getSaveFileName ( this, tr ( "Find Files" ), QDir::currentPath(), "*.tga;*.dds" );

    if ( !directory.isEmpty() )
    {
        if ( ui.comboBox_Image->findText ( directory ) == -1 )
        {
            ui.comboBox_Image->addItem ( directory );
            ui.comboBox_Image->setCurrentText ( directory );
        }
    }
}

void ImagePacker::saveConfig()
{
    QString outTextureName = ui.comboBox_Image->currentText();
    QString outProjectFile = ui.comboBox_ProjectFile->currentText();

    if ( outTextureName.isEmpty()  || outProjectFile.isEmpty() )
    {
        return;
    }
    gImagePacker.save ( outTextureName.toStdString().c_str(), outProjectFile.toStdString().c_str() );
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
            str = texture->mTextureDim->getRawName();
            return true;
        }
    }
    return false;
}

void ImagePacker::selectTreeItem ( const char* orignalFileName )
{

}

void ImagePacker::addPathTexture ( CXAddTextureArg* arg )
{
    QStandardItem *pathItem = 0, *newItem = 0;
    CXFileName filename ( arg->mName );
    CXFileName parentFile ( arg->mParent );

    //已存在
    if ( findTreeItem ( mTreeModel, filename.GetRelativeFileName(), newItem ) )
    {
        return;
    }

    //父目录存在
    newItem = new QStandardItem ( filename.GetFileName() );
    newItem->setData ( filename.GetRelativeFileName() );
    newItem->setEditable ( false );
    if ( findTreeItem ( mTreeModel, parentFile.GetRelativeFileName(), pathItem ) )
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
    CXFileName parentFile ( arg->mParent );

    newItem = new QStandardItem ( filename.GetFileName () );
    newItem->setData ( filename.GetRelativeFileName() );
    newItem->setEditable ( false );

    GTexture* texture = TextureMgr->getResource ( filename.GetRelativeFileName() );
    if ( texture )
    {
        TextureInfo* info = new TextureInfo;
        info->mImage = texture;
        info->mTextureDim = gImagePacker.getTexture ( filename.GetRelativeFileName() );
        mTextureMap.Insert ( filename.GetRelativeFileName(), info );
    }

    if ( findTreeItem ( mTreeModel, parentFile.GetRelativeFileName(), pathItem ) )
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
                newItem->setForeground ( Qt::BDiagPattern );
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
        node->setTexture ( texture->mTextureDim->getRawName() );
        node->recreate();
        mCanvos->getCanvos()->addChild ( node );
    }
}

void ImagePacker::createMenus()
{
    createOtherMenus();
    createRecentFileMenu();
}

void ImagePacker::newFile()
{

}

void ImagePacker::open()
{
    QString fileName = QFileDialog::getOpenFileName ( this );
    if ( !fileName.isEmpty() )
    {
        loadProject ( fileName.toStdString().c_str() );
    }
}

bool ImagePacker::save()
{
    return true;
}

bool ImagePacker::saveAs()
{
    return true;
}


void ImagePacker::loadProject ( const char* name )
{
    gImagePacker.loadProject ( name );
}

void ImagePacker::createFileListPanel()
{
    mTreeModel = new QStandardItemModel();
    mFileTree = new FileListTreeView ( ui.dockWidgetContents );
    mFileTree->setModel ( mTreeModel );
    mFileTree->setAcceptDrops ( true );
    ui.horizontalLayout->addWidget ( mFileTree );

    ui.dockWidget->setWindowTitle ( "FileList" );
    ui.dockWidget->setAcceptDrops ( true );
    ui.dockWidgetContents->setAcceptDrops ( true );

    connect ( mFileTree, SIGNAL ( clicked ( const QModelIndex & ) ), this, SLOT ( onClicked ( const QModelIndex & ) ) );
}

void ImagePacker::createConfigPanel()
{
    ui.dockWidget_2->setWindowTitle ( "Config" );
    ui.dockWidget_2->setAcceptDrops ( true );
    ui.dockWidgetContents_2->setAcceptDrops ( true );

    connect ( ui.pushButton_3, SIGNAL ( clicked() ), this, SLOT ( browseImage() ) );
    connect ( ui.pushButton_4, SIGNAL ( clicked() ), this, SLOT ( browseProject() ) );
    connect ( ui.pushButton_Save, SIGNAL ( clicked() ), this, SLOT ( saveConfig() ) );
}

void ImagePacker::createScenePanel()
{
    mScenePanel = new ScenePanel ( ui.centralWidget );
    ui.horizontalLayout_Scene->addWidget ( mScenePanel );

    mIdleTimeID = startTimer ( 0 );

    gImagePacker.mDelegateAddPath += this;
    gImagePacker.mDelegateAddTexture += this;
    gImagePacker.mDelegateAddTextureFailed += this;
    gImagePacker.mDelegateSettingImageFile += this;

    if ( !FiGameDemo_Init ( ( HWND ) ui.centralWidget->winId() ) )
    {
        assert ( 0 );
    }

    D9Device->setBackColor ( 0 );

    TextMgr->mDelegateOnDrawTextBegin += this;
    UIMgr->mDelegateHoverNodeChanged += this;

    mCanvos = new TextureCanvos;

    mSelectNode = new GRectNode;
    mSelectNode->setColor ( Color_Pure_Green );
    mSelectNode->setState ( eObjState_Render, false );
    mSelectNode->recreate();
}

bool ImagePacker::openRecentFile (  )
{
    QAction *action = qobject_cast<QAction *> ( sender() );
    QString text = action->data().toString();
    if ( !text.isEmpty() )
    {
        loadProject ( text.toStdString().c_str() );
        return true;
    }
    return false;
}

static const char* gRecentFileName = "__imagepacker_recentFile";

void ImagePacker::createRecentFileMenu()
{
    separatorAct = fileMenu->addSeparator();

    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentFileActs[i] = new QAction ( this );
        recentFileActs[i]->setVisible ( false );
        connect ( recentFileActs[i], SIGNAL ( triggered() ),
                  this, SLOT ( openRecentFile() ) );
    }
    for ( int i = 0; i < MaxRecentFiles; ++i )
        fileMenu->addAction ( recentFileActs[i] );

    updateRecentFileActions();
}

void ImagePacker::createOtherMenus()
{
    newAct = new QAction ( QIcon ( ":/images/new.png" ), tr ( "&New" ), this );
    newAct->setShortcuts ( QKeySequence::New );
    newAct->setStatusTip ( tr ( "Create a new file" ) );
    connect ( newAct, SIGNAL ( triggered() ), this, SLOT ( newFile() ) );

    //! [19]
    openAct = new QAction ( QIcon ( ":/images/open.png" ), tr ( "&Open..." ), this );
    openAct->setShortcuts ( QKeySequence::Open );
    openAct->setStatusTip ( tr ( "Open an existing file" ) );
    connect ( openAct, SIGNAL ( triggered() ), this, SLOT ( open() ) );
    //! [18] //! [19]

    saveAct = new QAction ( QIcon ( ":/images/save.png" ), tr ( "&Save" ), this );
    saveAct->setShortcuts ( QKeySequence::Save );
    saveAct->setStatusTip ( tr ( "Save the document to disk" ) );
    connect ( saveAct, SIGNAL ( triggered() ), this, SLOT ( save() ) );

    saveAsAct = new QAction ( tr ( "Save &As..." ), this );
    saveAsAct->setShortcuts ( QKeySequence::SaveAs );
    saveAsAct->setStatusTip ( tr ( "Save the document under a new name" ) );
    connect ( saveAsAct, SIGNAL ( triggered() ), this, SLOT ( saveAs() ) );

    fileMenu = menuBar()->addMenu ( tr ( "&File" ) );
    fileMenu->addAction ( newAct );
    //! [28]
    fileMenu->addAction ( openAct );
    //! [28]
    fileMenu->addAction ( saveAct );
    //! [26]
    fileMenu->addAction ( saveAsAct );
    fileMenu->addSeparator();
}

void ImagePacker::browseProject()
{
    QString directory = QFileDialog::getSaveFileName ( this, tr ( "Find Files" ), QDir::currentPath(), "*.xml" );

    if ( !directory.isEmpty() )
    {
        if ( ui.comboBox_ProjectFile->findText ( directory ) == -1 )
        {
            ui.comboBox_ProjectFile->addItem ( directory );
            ui.comboBox_ProjectFile->setCurrentText ( directory );
            setCurrentFile ( directory );
        }
    }
}

void ImagePacker::updateRecentFileActions()
{
    QStringList files = mOptionSetting.value ( gRecentFileName ).toStringList();

    int numRecentFiles = qMin ( files.size(), ( int ) MaxRecentFiles );

    for ( int i = 0; i < numRecentFiles; ++i )
    {
        QString text = tr ( "&%1 %2" ).arg ( i + 1 ).arg ( strippedName ( files[i] ) );
        recentFileActs[i]->setText ( text );
        recentFileActs[i]->setData ( files[i] );
        recentFileActs[i]->setVisible ( true );
    }
    for ( int j = numRecentFiles; j < MaxRecentFiles; ++j )
        recentFileActs[j]->setVisible ( false );

    separatorAct->setVisible ( numRecentFiles > 0 );
}

QString ImagePacker::strippedName ( const QString &fullFileName )
{
    return QFileInfo ( fullFileName ).fileName();
}

void ImagePacker::setCurrentFile ( const QString &fileName )
{
    mCurFile = fileName;
    setWindowFilePath ( mCurFile );

    QStringList files = mOptionSetting.value ( gRecentFileName ).toStringList();
    files.removeAll ( fileName );
    files.prepend ( fileName );
    while ( files.size() > MaxRecentFiles )
        files.removeLast();

    mOptionSetting.setValue ( gRecentFileName, files );

    updateRecentFileActions();
    //foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    //	MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
    //	if (mainWin)
    //		mainWin->updateRecentFileActions();
    //}
}

const char* ImagePacker::Option = "Option";

const char* ImagePacker::AppName = "ImagePacker";

TextureInfo::TextureInfo()
{
    mSelected = false;
    mTextureDim = nullptr;
    mImage = nullptr;
}
