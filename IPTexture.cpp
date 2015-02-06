#include "stdafx.h"
#include "IPTexture.h"

bool IPTextureNode::isPath()
{
    return true;
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
    return false;
}
