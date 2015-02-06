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

    XImagePacker ( void );
    ~XImagePacker ( void );

    void addPath ( const char* path, bool traverse = true, const char* parent = nullptr );
    void addTexture ( const char* filename, const char* parent = nullptr );
    void save ( const char* outTextureName, const char* outTextFile, int outW, int outH, FREE_IMAGE_FORMAT fmt );
    void saveProjectFile ( const char* name );
    void loadProjectFile ( const char* name );
    const std::vector<IPTexture*>& getAllTexture() const;

    IPTexture* getTexture ( const char* name );
private:
    void reset();

    void calcSize();
    void unloadTexture ( IPTexture* texture );
    void sortTextures();
    bool calcPos();
    bool isInCanvos ( const CXRect& rc );
    bool adpotWithRes ( IPTexture* curTexture, std::vector<IPTexture*>& res );
    bool saveAll();
    void deleteTextureInner ( const char* name );
	void onAdd ( const char* texName, const char* parent = nullptr );
	void addTextureTreeNode ( const char* texName, const char* parent = nullptr );
    static void errorReport ( FREE_IMAGE_FORMAT fif, const char* message );
	void linkTo(XImageTree::Node* treeNode,CXRapidxmlNode* docNode);
private:
    std::string mOutConfigFile;
    std::string mOutTextureFile;
    int mOutWidth;
    int mOutHeight;
    FREE_IMAGE_FORMAT mOutFormat;
    CXDynaArray<IPTexture*> mTextureArray;
    XImageTree mTree;
};
extern XImagePacker gImagePacker;