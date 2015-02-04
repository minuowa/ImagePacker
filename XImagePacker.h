#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "FreeImage.h"

#include <direct.h>
#include "dirent-win.h"
#ifdef _WINDOWS
#else
//#include <unistd.h>
//#include <dirent.h>
#endif
#include <vector>
#include "XDelegate.h"

class XTexture
{
public:
    std::string mFileName;
    CXRect mRect;
	XTexture();
	~XTexture();
	FIBITMAP* getData();
	bool loadData();
	void unloadData();
	void setSize(int w,int h);
	int getPixelSize();
	int getWByData();
	int getHByData();
private:
	int mPixelSize;
	FIBITMAP* mData;
};
struct CXAddTexturePathArg: public CXEventArgs
{
    GString mName;
};
struct CXAddTextureArg: public CXEventArgs
{
	GString mName;
	GString mPath;
};
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

    void addPath( const char* path );
    void addTexture ( const char* filename );
    void save ( const char* outTextureName, const char* outTextFile, int outW, int outH, FREE_IMAGE_FORMAT fmt );

    const std::vector<XTexture*>& getAllTexture() const;

	XTexture* getTexture(const char* name);
private:
    void calcSize();
    void unloadTexture ( XTexture* texture );
    void sortTextures();
    bool calcPos();
    bool isInCanvos ( const CXRect& rc );
    bool adpotWithRes ( XTexture* curTexture, std::vector<XTexture*>& res );
    bool saveAll();
	void deleteTextureInner(const char* name);
	void onAdd(const char* texName);
    static void errorReport ( FREE_IMAGE_FORMAT fif, const char* message );

private:
    std::string mOutConfigFile;
    std::string mOutTextureFile;
    int mOutWidth;
    int mOutHeight;
    FREE_IMAGE_FORMAT mOutFormat;
    std::vector<XTexture*> mTextureArray;

};
extern XImagePacker gImagePacker;