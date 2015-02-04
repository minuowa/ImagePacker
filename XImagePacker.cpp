#include "StdAfx.h"
#include "XImagePacker.h"
#include <iostream>
#include <assert.h>
#include <io.h>
#include <algorithm>

XImagePacker gImagePacker;

XImagePacker::XImagePacker ( void )
    : mSpace ( 0 )
{
    mOutWidth = 1024;
    mOutHeight = 1024;
    // Set FreeImage message handler
    FreeImage_SetOutputMessage ( errorReport );
}

XImagePacker::~XImagePacker ( void )
{
    dSafeDeleteVector ( mTextureArray );
}

void XImagePacker::addTexture ( const char* filename )
{
    XTexture* texture = new XTexture;
    CXFileName xfilename ( filename );

    texture->mFileName = xfilename.GetOrignalName();
    texture->loadData();
    if ( texture->getData() == nullptr )
    {
        delete texture;
        return;
    }
    else
    {
        mTextureArray.push_back ( texture );
    }
    onAdd ( xfilename.GetOrignalName() );
}


bool compare_texture ( XTexture* lhs, XTexture* rhs )
{
    return lhs->mRect.area() > rhs->mRect.area();
}

void XImagePacker::sortTextures()
{
    std::sort ( mTextureArray.begin(), mTextureArray.end(), compare_texture );
}

bool XImagePacker::saveAll()
{
    FILE* output_text = 0;

    printf ( "Creating output texture for atlas %s\n", mOutTextureFile.c_str() );

    FIBITMAP* output_texture = FreeImage_Allocate ( mOutWidth, mOutHeight, 32, 0, 0, 0 );

    if ( fopen_s ( &output_text, mOutConfigFile.c_str(), "w" ) )
    {
        assert ( 0 && "can not open file!" );
        return false;
    }

    size_t count = mTextureArray.size();
    for ( size_t i = 0; i < count; ++i )
    {
        XTexture* tex = mTextureArray[i];
        fprintf ( output_text, "\"%s\" %d %d %d\n", tex->mFileName.c_str(), tex->mRect.mX, tex->mRect.mY, tex->getPixelSize() );
        FreeImage_Paste ( output_texture, tex->getData(), tex->mRect.mX, tex->mRect.mY, 255 );
    }

    printf ( "Writing texture atlas to %s\n", mOutTextureFile.c_str() );

    if ( !FreeImage_Save ( mOutFormat, output_texture, mOutTextureFile.c_str(), 0 ) )
    {
        fprintf ( stderr, "Unable to save image %s\n", mOutTextureFile.c_str() );

        return false;
    }

    FreeImage_Unload ( output_texture );
    fclose ( output_text );

    return true;
}



void XImagePacker::errorReport ( FREE_IMAGE_FORMAT fif, const char* message )
{

}

void XImagePacker::save ( const char* outTextureName, const char* outTextFile, int outW, int outH, FREE_IMAGE_FORMAT fmt )
{
    mOutTextureFile = outTextureName;
    mOutConfigFile = outTextFile;
    mOutWidth = outW;
    mOutHeight = outH;
    mOutFormat = fmt;
    /** 重新计算一遍 **/
    onAdd ( nullptr );
    saveAll();
}


bool XImagePacker::adpotWithRes ( XTexture* curTexture, std::vector<XTexture*>& res )
{
    bool ok = false;
    size_t n = 0;
    //放下面
    for ( ; n < res.size(); ++n )
    {
        XTexture* txt = res[n];

        if ( curTexture == txt )
            continue;

        curTexture->mRect.mX = txt->mRect.mX;
        curTexture->mRect.mY = txt->mRect.bottom() + mSpace;

        bool overlap = false;

        for ( size_t i = 0; i < res.size(); ++i )
        {
            XTexture* pt = res[i];
            if ( pt == curTexture )
                continue;
            if ( curTexture->mRect.isOverlaps ( pt->mRect ) || !isInCanvos ( curTexture->mRect ) )
            {
                overlap = true;
            }
        }

        if ( overlap )
        {
            continue;
        }
        else
        {
            ok = true;
            break;
        }
    }
    //放右面
    if ( !ok )
    {
        n = 0;
        for ( ; n < res.size(); ++n )
        {
            XTexture* txt = res[n];

            if ( curTexture == txt )
                continue;

            curTexture->mRect.mY = txt->mRect.mY;
            curTexture->mRect.mX = txt->mRect.right() + mSpace;

            bool overlap = false;

            for ( size_t i = 0; i < res.size(); ++i )
            {
                XTexture* pt = res[i];
                if ( pt == curTexture )
                    continue;
                if ( curTexture->mRect.isOverlaps ( pt->mRect ) || !isInCanvos ( curTexture->mRect ) )
                {
                    overlap = true;
                }
            }

            if ( overlap )
            {
                continue;
            }
            else
            {
                ok = true;
                break;
            }
        }
    }

    if ( ok )
    {
        res.push_back ( curTexture );
    }
    return ok;
}
bool XImagePacker::calcPos()
{
    size_t cnt = mTextureArray.size();
    for ( size_t i = 0; i < cnt; ++i )
    {
        XTexture* curTexture = mTextureArray[i];
        curTexture->mRect.mX = 0;
        curTexture->mRect.mY = 0;
        if ( !isInCanvos ( curTexture->mRect ) )
        {
            //assert ( 0 && "Canvos is to small!" );
            return false;
        }
    }

    std::vector<XTexture*> res;

    for ( size_t i = 0; i < cnt; ++i )
    {
        XTexture* curTexture = mTextureArray[i];
        if ( !isInCanvos ( curTexture->mRect ) )
        {
            //assert ( 0 && "Canvos is to small!" );
            return false;
        }
        else
        {
            if ( res.size() == 0 )
            {
                res.push_back ( curTexture );
                continue;
            }
        }
        if ( !adpotWithRes ( curTexture, res ) )
        {
            return false;
        }
    }
    return true;
}

const std::vector<XTexture*>& XImagePacker::getAllTexture() const
{
    return mTextureArray;
}

bool XImagePacker::isInCanvos ( const CXRect& rc )
{
    return rc.mX >= 0 && rc.mY >= 0 && rc.right() <= mOutWidth && rc.bottom() <= mOutHeight;
}

void XImagePacker::addPath ( const char* path )
{
    CXAddTextureArg arg;
    arg.mName = path;
    mDelegateAddPath.trigger ( &arg );

    _finddata_t allFile;
    intptr_t hFile;
    std::string target = path;
    target += "/*.*";
    std::string filename;
    if ( ( hFile = _findfirst ( target.c_str(), &allFile ) ) == -1L )
        printf ( "No *.c files in current directory!\n" );
    else
    {
        do
        {
            filename = path;
            filename += "/";
            filename += allFile.name;
            if ( !dStrEqual ( allFile.name, "." ) && !dStrEqual ( allFile.name, ".." ) )
            {
                if ( dIsPath ( filename.c_str() ) )
                {
                    addPath ( filename.c_str() );
                }
                else
                {
                    addTexture ( filename.c_str() );
                }
            }
        }
        while ( _findnext ( hFile, &allFile ) == 0 );
        _findclose ( hFile );
    }
    //onAdd();
}

XTexture* XImagePacker::getTexture ( const char* name )
{
for ( auto tex: mTextureArray )
    {
        if ( tex->mFileName == name  )
        {
            return tex;
        }
    }
    return nullptr;
}

void XImagePacker::onAdd ( const char* texName )
{
    sortTextures();

    if ( texName != nullptr )
    {
        if ( calcPos() )
        {
            CXAddTextureArg arg;
            arg.mName = texName;
            mDelegateAddTexture.trigger ( &arg );
        }
        else
        {
            deleteTextureInner ( texName );
            mDelegateAddTextureFailed.trigger ( nullptr );
        }
    }
}

void XImagePacker::deleteTextureInner ( const char* name )
{
    for ( auto i = mTextureArray.begin(); i != mTextureArray.end(); ++i )
    {
        auto tex = *i;
        if ( tex->mFileName == name )
        {
            delete tex;
            mTextureArray.erase ( i );
            break;
        }
    }
}





XTexture::~XTexture()
{
    unloadData();
}

void XTexture::unloadData()
{
    if ( mData != nullptr )
    {
        FreeImage_Unload ( mData );
        mData = nullptr;
    }
}

XTexture::XTexture()
{
    mData = nullptr;
}

bool XTexture::loadData()
{
    FREE_IMAGE_FORMAT fmt;

    fmt = FreeImage_GetFIFFromFilename ( mFileName.c_str() );

    if ( fmt == FIF_UNKNOWN )
    {
        return false;
    }
    mData = FreeImage_Load ( fmt, mFileName.c_str(), 0 );

    setSize ( getWByData(), getHByData() );

    return true;
}

FIBITMAP* XTexture::getData()
{
    return mData;
}

void XTexture::setSize ( int w, int h )
{
    mRect.mW = w;
    mRect.mH = h;
    mPixelSize = w * h;
}

int XTexture::getPixelSize()
{
    return mPixelSize;
}

int XTexture::getWByData()
{
    return FreeImage_GetWidth ( mData );
}

int XTexture::getHByData()
{
    return FreeImage_GetHeight ( mData );
}
