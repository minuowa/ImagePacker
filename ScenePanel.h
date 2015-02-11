#ifndef SCENEPANEL_H
#define SCENEPANEL_H

#include <QWidget>
#include "ui_ScenePanel.h"

class ScenePanel : public QWidget
{
    Q_OBJECT

public:
    ScenePanel ( QWidget *parent = 0 );
    ~ScenePanel();

    void clearSelect();
    void setSelect ( const char* name );
private slots:
    void copyNameToClipboard();

private:
    Ui::ScenePanel ui;
    virtual void resizeEvent ( QResizeEvent * );
    bool event ( QEvent *event );

    GString mSelectNodeName;

    QMenu* mMainMenu;
};

#endif // SCENEPANEL_H
