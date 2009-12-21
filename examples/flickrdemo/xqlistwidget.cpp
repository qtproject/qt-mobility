/*
 * Copyright (c) 2009 Nokia Corporation.
 */

#include "xqlistwidget.h"

XQListWidget::XQListWidget(QWidget* parent) : QListWidget(parent)
{
}

void XQListWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Left: {
            focusPreviousChild();
            break;
        }
        case Qt::Key_Right: {
            focusNextChild();
            break;
        }
        default: {
            QListWidget::keyPressEvent(event);
        }
    }
}
