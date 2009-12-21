/*
 * Copyright (c) 2009 Nokia Corporation.
 */

#ifndef XQLISTWIDGET_H
#define XQLISTWIDGET_H

#include <QListWidget>
#include <QKeyEvent>

class XQListWidget: public QListWidget
{
public:
    XQListWidget(QWidget* parent = 0);  
    
protected:
    void keyPressEvent(QKeyEvent* event);
};

#endif // XQLISTWIDGET_H
