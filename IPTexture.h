#ifndef IPTexture_h__
#define IPTexture_h__

#include "FreeImage.h"

#include <direct.h>
#include "dirent-win.h"
class IPTextureNode
{
public:
	IPTextureNode();
    virtual bool isPath();
    const char* getRawName();
    const char* getDisplayName();
    void setRawName ( const char* name );
    void setDisplayName ( const char* name );
    CXDynaArray<IPTextureNode*>& getChildren();
	CXRect mRect;
protected:
	GString mOrignalName;
    GString mDisplayName;
    CXDynaArray<IPTextureNode*> mChildren;
	bool mIsPath;
};
class IPTexture: public IPTextureNode
{
public:

    IPTexture();
    ~IPTexture();

    virtual bool isPath();
    FIBITMAP* getData();
    bool loadData();
    void unloadData();
    void setSize ( int w, int h );
    int getPixelSize();
    int getWByData();
    int getHByData();

private:
    int mPixelSize;
    FIBITMAP* mData;
};

#endif // IPTexture_h__
