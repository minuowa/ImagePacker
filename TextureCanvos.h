#pragma once
#include "XImagePacker.h"
class GTexture;
class GUINode;
class TextureCanvos
{
public:
	TextureCanvos(void);
	~TextureCanvos(void);

	void setSize(int w,int h);
	void setPos(int x,int y);
	void draw();
	void recreate();
	GUINode* getCanvos();
protected:
	GTexture* mTexture;
	GUINode* mUI;
};

