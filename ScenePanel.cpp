#include "stdafx.h"
#include "ScenePanel.h"
#include "GGameDemoHeader.h"
#include "GGame.h"

ScenePanel::ScenePanel ( QWidget *parent )
    : QWidget ( parent )
{
    ui.setupUi ( this );
    mMainMenu = new QMenu ( this );
    QAction* act = new QAction ( tr ( "&CopyName" ), this );
    act->setStatusTip ( tr ( "Copy Name to clipboard" ) );
    connect ( act, SIGNAL ( triggered() ), this, SLOT ( copyNameToClipboard() ) );
    mMainMenu->addAction ( act );

    this->setContextMenuPolicy ( Qt::ActionsContextMenu );
}

ScenePanel::~ScenePanel()
{

}

void ScenePanel::resizeEvent ( QResizeEvent *szEvent )
{
    __super::resizeEvent ( szEvent );
    QRect rc = this->geometry();
    Content::Game.resize ( rc.width(), rc.height() );
}
bool ScenePanel::event ( QEvent* event )
{
    switch ( event->type() )
    {
    case QEvent::ContextMenu:
    {
        QContextMenuEvent* menuEvent = ( QContextMenuEvent* ) event;
        CXASSERT ( mMainMenu );
        mMainMenu->exec ( menuEvent->globalPos() );
    }
    break;
	case  QEvent::WindowActivate:
		{
			if(this->focusWidget()==this)
				Content::Game.active ( true );
		}
		break;
	case QEvent::WindowDeactivate :
		{
			Content::Game.active ( false );
		}
		break;
    }
    return __super::event ( event );
}

void ScenePanel::copyNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText ( mSelectNodeName.c_str() );
}

void ScenePanel::clearSelect()
{
    mSelectNodeName.clear();
}

void ScenePanel::setSelect ( const char* name )
{
    mSelectNodeName = name;
}
