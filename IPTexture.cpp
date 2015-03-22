#include "stdafx.h"
#include "IPTexture.h"

bool IPTextureNode::isPath()
{
    return true;
}

const char* IPTextureNode::getRawName()
{
    return mOrignalName.c_str();
}

const char* IPTextureNode::getDisplayName()
{
    return mDisplayName.c_str();
}

void IPTextureNode::setRawName ( const char* name )
{
    mOrignalName = name;
    mIsPath = dIsPath ( name );
}

void IPTextureNode::setDisplayName ( const char* name )
{
    mDisplayName = name;
}

CXDynaArray<IPTextureNode*>& IPTextureNode::getChildren()
{
    return mChildren;
}

IPTextureNode::IPTextureNode()
    : mIsPath ( false )
{

}

FIBITMAP* IPTexture::getData()
{
    return mData;
}

void IPTexture::setSize ( int w, int h )
{
    mRect.mW = w;
    mRect.mH = h;
    mPixelSize = w * h;
}

int IPTexture::getPixelSize()
{
    return mPixelSize;
}

int IPTexture::getWByData()
{
    return FreeImage_GetWidth ( mData );
}

int IPTexture::getHByData()
{
    return FreeImage_GetHeight ( mData );
}

bool IPTexture::isPath()
{
    return dIsPath ( mOrignalName );
}
