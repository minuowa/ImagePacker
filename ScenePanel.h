#ifndef SCENEPANEL_H
#define SCENEPANEL_H

#include <QWidget>
#include "ui_ScenePanel.h"

class ScenePanel : public QWidget
{
	Q_OBJECT

public:
	ScenePanel(QWidget *parent = 0);
	~ScenePanel();

private:
	Ui::ScenePanel ui;
	virtual void resizeEvent(QResizeEvent *);
};

#endif // SCENEPANEL_H
