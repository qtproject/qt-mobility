/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
