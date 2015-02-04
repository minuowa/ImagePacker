#ifndef IMAGEPACKER_H
#define IMAGEPACKER_H

#include <QtWidgets/QMainWindow>
#include "ui_imagepacker.h"
#include "TextureCanvos.h"
class FileListTreeView;
class ScenePanel;
class XTexture;
class GTexture;
struct TextureInfo
{
    XTexture* mTextureDim;
    GTexture* mImage;
    bool mSelected;
    TextureInfo();
};
class ImagePacker : public QMainWindow, public CXCallBack
{
    Q_OBJECT

public:
    ImagePacker ( QWidget *parent = 0 );
    ~ImagePacker();

    virtual void onCallBack ( const CXDelegate& d, CXEventArgs* e );
protected:
    virtual void timerEvent ( QTimerEvent * );
    bool event ( QEvent * e );
private slots:
    void browseConfig();
    void browseImage();
    void saveConfig();
    void onClicked ( const QModelIndex &index );
private:
    bool findTreeItem ( QStandardItemModel* model, const char* fileOrPath, QStandardItem*& outItem );
    bool findItem ( QStandardItem* item, const char* fileOrPath, QStandardItem*& outItem );
    void drawAllTextures();

	bool getItemString(const CXRect& rc, GString& str);

    void selectTreeItem ( const char* orignalFileName );

	void onCalTexturePos();
	void addPathTexture(CXAddTexturePathArg* arg);
	void directAddTexture(CXAddTextureArg* arg);
	void onChangeHoveredNode(GUIHoverNodeChangedEvent* arg);

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
