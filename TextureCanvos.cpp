#include "stdafx.h"
#include "TextureCanvos.h"
#include "Game.h"

TextureCanvos::TextureCanvos ( void )
    : mUI ( nullptr )
{
    recreate();
}


TextureCanvos::~TextureCanvos ( void )
{
}

void TextureCanvos::setSize ( int w, int h )
{
    CXASSERT ( IS2SQUARE ( w ) && IS2SQUARE ( h ) && "Image's root is not 2" );
    const CXRect& rc = mUI->getRect();
    mUI->setRect ( rc.mX, rc.mY, w, h );

}

void TextureCanvos::setPos ( int x, int y )
{
    const CXRect& rc = mUI->getRect();
    mUI->setRect ( x, y, rc.mW, rc.mH );
}

void TextureCanvos::recreate()
{
    this->clear();
    mUI = new GUINode;
    mUI->setRect ( 0, 0, 1024, 1024 );
    mUI->setColor ( 0X66666666 );
    mUI->recreate();
    getUIRootNode()->addChild ( mUI );
}

GUINode* TextureCanvos::getCanvos()
{
    return mUI;
}

void TextureCanvos::clear()
{
    if ( mUI )
    {
        getUIRootNode()->deleteChild ( mUI );
        mUI = nullptr;
    }
}
