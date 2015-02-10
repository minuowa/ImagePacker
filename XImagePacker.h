#pragma once
#include <stdio.h>
#include <stdlib.h>


#include <vector>
#include "XDelegate.h"
#include "IPTexture.h"


struct CXAddTextureArg: public CXEventArgs
{
    GString mName;
    GString mParent;
};
typedef CXTree<IPTextureNode> XImageTree;

class XImagePacker
{
public:
    static const int MAX_WIDTH = 4096;
    static const int MAX_HEIGHT = 4096;

    int mSpace;

    CXDelegate mDelegateAddTexture;
    CXDelegate mDelegateAddPath;
	CXDelegate mDelegateAddTextureFailed;
	CXDelegate mDelegateSettingImageFile;

    XImagePacker ( void );
    ~XImagePacker ( void );

    void addPath ( const char* path, bool traverse = true, const char* parent = nullptr );
    void tryToAddTexture ( const char* filename, const char* parent = nullptr );
    void save ( const char* outTextureName, const char* projectFile );
    void saveProject ();
    void loadProject ( const char* name );
    void setOutFormat ( int w, int h, FREE_IMAGE_FORMAT fmt );
    IPTexture* getTexture ( const char* name );
	const char* getImageFile();
	const char* getProjectFile();
private:
    bool saveImage();
    void reset();
    void sortTextures();
    bool calcPos();
    bool isInCanvos ( const CXRect& rc );
    bool adpotWithRes ( IPTexture* curTexture, std::vector<IPTexture*>& res );
    void deleteTextureInner ( const char* name );
    bool tryToAddTextureTreeNode ( const char* texName, const char* parent = nullptr );
    static void errorReport ( FREE_IMAGE_FORMAT fif, const char* message );
    void linkTo ( XImageTree::Node* treeNode, CXRapidxmlNode* docNode );
private:
    void refillData();

    GString mImageFile;
    GString mProjectFile;
    int mOutWidth;
    int mOutHeight;
    FREE_IMAGE_FORMAT mOutFormat;
    CXDynaArray<IPTexture*> mTextureArray;
    XImageTree mTree;
};
extern XImagePacker gImagePacker;