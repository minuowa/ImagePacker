#pragma once
#include "XImagePacker.h"
class GTexture;
class GUINode;
//-------------------------------------------------------------------------
/**
	@brief »­²¼
	@author nnboy,2015/02/11
*/
class TextureCanvos
{
public:
	TextureCanvos(void);
	~TextureCanvos(void);

	void offset(long x,long y);
	void setSize(int w,int h);
	void setPos(int x,int y);
	void recreate();
	void clear();
	GUINode* getCanvos();
protected:
	GUINode* mUI;
};

