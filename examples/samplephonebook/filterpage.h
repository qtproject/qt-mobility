/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FILTERPAGE_H
#define FILTERPAGE_H

#include "qtcontacts.h"

#include <QWidget>
#include <QMap>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
class QScrollArea;
class QComboBox;
class QPushButton;
class QLineEdit;
class QLabel;
QT_END_NAMESPACE

QTM_USE_NAMESPACE

class FilterPage : public QWidget
{
    Q_OBJECT

public:
    FilterPage(QWidget *parent = 0);
    ~FilterPage();

signals:
    void showListPage(const QContactFilter& filter);

public slots:
    void clearFilter();

private slots:
    void filterClicked();
    void cancelClicked();

private:
    QString m_name;
    QString m_phone;
    QString m_email;
    QString m_address;
    QLineEdit *m_nameEdit;
    QLineEdit *m_phoneEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_addressEdit;

#ifdef Q_OS_SYMBIAN
    QAction *m_filterBtn;
    QAction *m_cancelBtn;
#else
    QPushButton *m_filterBtn;
    QPushButton *m_cancelBtn;
#endif
    QContactIntersectionFilter m_currentFilter;
};

#endif // FILTERPAGE_H
