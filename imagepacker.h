#ifndef IMAGEPACKER_H
#define IMAGEPACKER_H

#include <QtWidgets/QMainWindow>
#include "ui_imagepacker.h"
#include "TextureCanvos.h"
class FileListTreeView;
class ScenePanel;
class IPTexture;
class GTexture;
struct TextureInfo
{
    IPTexture* mTextureDim;
    GTexture* mImage;
    bool mSelected;
    TextureInfo();
};
class ImagePacker : public QMainWindow, public CXCallBack
{
public:
    static const char* AppName;
    static const char* Option;
    Q_OBJECT

public:
    ImagePacker ( QWidget *parent = 0 );
    ~ImagePacker();

    virtual void onCallBack ( const CXDelegate& d, CXEventArgs* e );

protected:
    void loadProject ( const char* name );
    void createMenus();
    void createFileListPanel();
    void createConfigPanel();
    void createScenePanel();
    void createRecentFileMenu();
    void createOtherMenus();

    virtual void timerEvent ( QTimerEvent * );
    bool event ( QEvent * e );
private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void browseImage();
    void browseProject();
    void saveConfig();
    bool openRecentFile( );
    void onClicked ( const QModelIndex &index );
private:
    bool findTreeItem ( QStandardItemModel* model, const char* fileOrPath, QStandardItem*& outItem );
    bool findItem ( QStandardItem* item, const char* fileOrPath, QStandardItem*& outItem );

    TextureInfo* getItemString ( const CXRect& rc );

    void selectTreeItem ( const char* orignalFileName );

    void onCalTexturePos();
    void addPathTexture ( CXAddTextureArg* arg );
    void directAddTexture ( CXAddTextureArg* arg );
    void onChangeHoveredNode ( GUIHoverNodeChangedEvent* arg );

    void updateRecentFileActions();
    QString strippedName ( const QString &fullFileName );
    void setCurrentFile ( const QString &fileName );
private:

    QSettings mOptionSetting;


    enum { MaxRecentFiles = 20 };
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorAct;

    QString mCurFile;


    QList<QAction*> mRecentFileActions;

    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;

    Ui::ImagePackerClass ui;

    QStandardItemModel* mTreeModel;

    FileListTreeView* mFileTree;

    ScenePanel* mScenePanel;

    CXMap<GString, TextureInfo*> mTextureMap;

    TextureCanvos* mCanvos;

    GRectNode* mSelectNode;


    int mIdleTimeID;
};
#endif // IMAGEPACKER_H
