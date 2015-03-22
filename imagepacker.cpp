#include "stdafx.h"
#include "imagepacker.h"
#include "XImagePacker.h"
#include "FileListTreeView.h"
#include "GGameDemoHeader.h"
#include "GGame.h"
#include "ScenePanel.h"
#include "GText.h"
#include "GUINode.h"

ImagePacker::ImagePacker ( QWidget *parent )
    : QMainWindow ( parent )
    , mIdleTimeID ( 0 )
    , mOptionSetting ( AppName, Option )
    , mSelectNode ( nullptr )
{
    mInstance = this;

    ui.setupUi ( this );

    createMenus();

    createFileListPanel();

    createConfigPanel();

    createScenePanel();
}

ImagePacker::~ImagePacker()
{
    gImagePacker.mDelegateAddPath -= this;
    gImagePacker.mDelegateAddTexture -= this;
    gImagePacker.mDelegateDeleteTexture -= this;
    gImagePacker.mDelegateAddTextureFailed -= this;
    gImagePacker.mDelegateLoadSuccessed -= this;
    gImagePacker.mDelegateClear -= this;

    Content::Text.mDelegateOnDrawTextBegin -= this;
    Content::UIMgr.mDelegateNodeClicked -= this;
    Content::InputSystem.mDelegateMouseDownAndMoved -= this;
    Content::InputSystem.mDelegateMouseDown -= this;
    Content::InputSystem.mDelegateMouseUp -= this;

    dSafeDelete ( mCanvos );

    Content::Game.shutDown();
}

void ImagePacker::onCallBack ( const CXDelegate& d, CXEventArgs* e )
{
    if ( d == gImagePacker.mDelegateAddPath )
    {
        CXAddTextureArg* arg = ( CXAddTextureArg* ) e;
        onAddPath ( arg );
    }
    else if ( d == gImagePacker.mDelegateAddTexture )
    {
        CXAddTextureArg* arg = ( CXAddTextureArg* ) e;
        onAddTexture ( arg );
        onCalTexturePos();
    }
    else if ( d == gImagePacker.mDelegateDeleteTexture )
    {
        CXDeleteTextureArg* arg = ( CXDeleteTextureArg* ) e;
        onDeleteTexture ( arg );
        //mTextureMap.deleteChild ( isOrignalNameTrue, arg->mName );
        mSelectNode->setState ( eObjState_Render, false );
        onCalTexturePos();
    }
    else if ( d == gImagePacker.mDelegateAddTextureFailed )
    {
        QMessageBox::warning ( nullptr, "", qtToUnicode ( "画布太小，放不了这么多图片！" ) );
    }
    else if ( d == gImagePacker.mDelegateLoadSuccessed )
    {
        ui.comboBox_ProjectFile->setCurrentText ( gImagePacker.getProjectFile() );
        ui.comboBox_Image->setCurrentText ( gImagePacker.getImageFile() );
        if ( !mCanvos->getCanvos() )
            mCanvos->recreate();
    }
    else if ( d == gImagePacker.mDelegateClear )
    {
        this->clear();
    }
    else if ( Content::Text.mDelegateOnDrawTextBegin == d )
    {
        mSelectNode->draw();
    }
    else if ( d == Content::UIMgr.mDelegateNodeClicked )
    {
        GUINode* node = Content::UIMgr.getCapture();
        onClickedNode ( node );
    }
    else if ( d == Content::InputSystem.mDelegateMouseDownAndMoved )
    {
        if ( Content::InputSystem.isPressingButton ( eButtonType_LeftButton ) )
        {
            POINT pt = Content::InputSystem.GetMouseMove();
            mCanvos->offset ( pt.x, pt.y );
            mSelectNode->offset ( pt.x, pt.y );
        }
    }
    else if ( d == Content::InputSystem.mDelegateMouseDown )
    {
        if ( Content::InputSystem.isButtonDown ( eButtonType_LeftButton ) )
        {
            setCursor ( QCursor ( Qt::SizeAllCursor ) );
        }
    }
    else if ( d == Content::InputSystem.mDelegateMouseUp )
    {
        if ( Content::InputSystem.isButtonUp ( eButtonType_LeftButton ) )
        {
            setCursor ( QCursor ( Qt::ArrowCursor ) );
        }
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
        if ( !Content::Game.loop() )
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
        Content::Game.active ( true );
    }
    break;
    case QEvent::WindowDeactivate :
    {
        Content::Game.active ( false );
    }
    break;
    default:
        break;
    }
    return __super::event ( event );
}


void ImagePacker::onAddPath ( CXAddTextureArg* arg )
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

void ImagePacker::onAddTexture ( CXAddTextureArg* arg )
{
    QStandardItem *pathItem = 0, *newItem = 0;
    GString name, parentPath;
    CXFileName filename ( arg->mName );
    CXFileName parentFile ( arg->mParent );

    newItem = new QStandardItem ( filename.GetFileName () );
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

void ImagePacker::onClickedNode ( GUINode* node )
{
    mScenePanel->clearSelect();
    mSelectNode->setState ( eObjState_Render, false );
    IPTextureNode* info = gImagePacker.getImageByRect ( node->getRelRect() );
    if ( info != nullptr )
    {
        selectNode ( info->getRawName() );

        QStandardItem* treeItem = nullptr;
        if ( findItem ( mTreeModel->invisibleRootItem(), info->getRawName(), treeItem ) )
        {
            if ( treeItem->parent() )
            {
                QStandardItem* parItem = treeItem->parent();
                while ( parItem )
                {
                    mFileTree->expand ( parItem->index() );
                    parItem = parItem->parent();
                }
            }
            QItemSelectionModel* selectionModel = mFileTree->selectionModel();
            selectionModel->select ( treeItem->index(), QItemSelectionModel::SelectionFlag::ClearAndSelect );
        }
    }
}
void traverseForCreateUINode ( XImageTree::Node* n,  void*  win )
{
    IPTextureNode* texture = n->getData();
    if ( !texture->isPath() )
    {
        GUINode* node = new GUINode;
        node->setRelRect ( texture->mRect );
        node->setTexture ( texture->getRawName() );
        node->setName ( texture->getRawName() );
        node->recreate();
        ( ( ImagePacker* ) win )->getCanvos()->getCanvos()->addChild ( node );
    }
}
void ImagePacker::onCalTexturePos()
{
    mCanvos->recreate();
    gImagePacker.textureInforaverse ( traverseForCreateUINode, ( void* ) this );
}

void ImagePacker::createMenus()
{
    createOtherMenus();
    createRecentFileMenu();
}

void ImagePacker::newFile()
{
    gImagePacker.reset();
    mCanvos->recreate();
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
    gImagePacker.saveProject();
    return true;
}

bool ImagePacker::saveAs()
{
    return true;
}


void ImagePacker::loadProject ( const char* name )
{
    if ( !gImagePacker.loadProject ( name ) )
    {
        QMessageBox::warning ( nullptr, "", qtToUnicode ( "加载文件失败！" ) );
        mCanvos->recreate();
    }
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

    //connect ( mFileTree, SIGNAL ( clicked ( const QModelIndex & ) ), this, SLOT ( onClicked ( const QModelIndex & ) ) );

    QItemSelectionModel* selectionModel = mFileTree->selectionModel();
    bool res = connect ( selectionModel, SIGNAL ( selectionChanged ( const QItemSelection &, const QItemSelection & ) ),
                         this, SLOT ( onSelectionChanged ( const QItemSelection &, const QItemSelection & ) ) );
    CXASSERT ( res );
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
    gImagePacker.mDelegateDeleteTexture += this;
    gImagePacker.mDelegateAddTextureFailed += this;
    gImagePacker.mDelegateLoadSuccessed += this;
    gImagePacker.mDelegateClear += this;

    if ( !Content::Game.init ( ( HWND ) ui.centralWidget->winId() ) )
    {
        assert ( 0 );
    }


    Content::Device.setBackColor ( 0X55FF9999 );

    Content::Text.mDelegateOnDrawTextBegin += this;
    Content::UIMgr.mDelegateNodeClicked += this;

    Content::InputSystem.mDelegateMouseDownAndMoved += this;
    Content::InputSystem.mDelegateMouseDown += this;
    Content::InputSystem.mDelegateMouseUp += this;

    mCanvos = new TextureCanvos;

    mSelectNode = new GTapeNode();
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
}

void ImagePacker::clear()
{
    ui.comboBox_Image->clearEditText();
    ui.comboBox_ProjectFile->clearEditText();
    mSelectNode->setState ( eObjState_Render, false );
    mCanvos->clear();
    mTreeModel->clear();
}

void ImagePacker::selectNode ( const char* name )
{
    IPTextureNode* info = gImagePacker.getTexture ( name );
    if ( info )
    {
        mScenePanel->setSelect ( info->getDisplayName() );
        CXRect rc = info->mRect;
        GString str;
        str.Format ( "Name: %s", info->getDisplayName() );
        ui.label_name->setText ( str.c_str() );
        str.Format ( "X:%d, Y:%d, R:%d, B:%d", rc.mX, rc.mY, rc.right(), rc.bottom() );
        ui.label_pos->setText ( str.c_str() );
        str.Format ( "W:%d, H:%d", rc.mW, rc.mH );
        ui.label_wh->setText ( str.c_str() );
        GUINode* n = Content::UIMgr.getNode ( info->getRawName() );
        if ( n )
            mSelectNode->setRect ( n->getRect() );
        mSelectNode->setState ( eObjState_Render, n != nullptr );
    }
}


void ImagePacker::onSelectionChanged ( const QItemSelection &selected, const QItemSelection &deselected )
{
    const QModelIndexList&  indexlist = selected.indexes();
    if ( indexlist.size() > 0 )
        onSelect ( indexlist.front() );
}

void ImagePacker::onSelect ( const QModelIndex& index )
{
    QStandardItem* item = mTreeModel->itemFromIndex ( index );
    QVariant var = item->data ( );
    if ( var.type() != QVariant::String )
        return;

    selectNode ( var.toString().toStdString().c_str() );
}

void ImagePacker::onDeleteTexture ( CXDeleteTextureArg* arg )
{
    QStandardItem* item = nullptr;
    if ( findItem ( mTreeModel->invisibleRootItem(), arg->mName, item ) )
    {
        int row = item->index().row();
        int col = item->index().column();
        if ( item->parent() )
        {
            item->takeChild ( row, col );
            item->parent()->removeRow ( row );
        }
        else
        {
            mTreeModel->takeRow ( row );
        }
    }
}

TextureCanvos* ImagePacker::getCanvos() const
{
    return mCanvos;
}

ImagePacker* ImagePacker::mInstance = nullptr;


const char* ImagePacker::Option = "Option";

const char* ImagePacker::AppName = "ImagePacker";

