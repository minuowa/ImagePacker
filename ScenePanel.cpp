#include "stdafx.h"
#include "ScenePanel.h"
#include "GGameDemoHeader.h"
#include "GGame.h"

ScenePanel::ScenePanel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ScenePanel::~ScenePanel()
{

}

void ScenePanel::resizeEvent( QResizeEvent *szEvent )
{
	__super::resizeEvent ( szEvent );
	QRect rc = this->rect();
	TheGame->resize ( rc.width(), rc.height() );
}
