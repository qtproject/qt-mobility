/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef CNTDISPLAYLABEL_H_
#define CNTDISPLAYLABEL_H_

#include <QObject>
#include <QLatin1String>

#include <qcontactmanager.h>
#include <qcontact.h>
#include <qcontactdetail.h>


#include <qmobilityglobal.h>

QTM_USE_NAMESPACE

class CntDisplayLabel : public QObject
{
    Q_OBJECT
    
public:
    CntDisplayLabel();
    virtual ~CntDisplayLabel();

    QString synthesizedDisplayLabel( const QContact& contact, QContactManager::Error* error) const;
    QString unNamned() const;
    QList<QPair<QLatin1String, QLatin1String> > contactFilterDetails() const;
    QList<QPair<QLatin1String, QLatin1String> > groupFilterDetails() const;
    
private:
    void setDisplayLabelDetails();
    QString generateDisplayLabel( const QContact &contact, const QList<QList<QPair<QLatin1String, QLatin1String> > > detailList) const;
    QString delimiter() const;
    
private:
    QList<QList<QPair<QLatin1String, QLatin1String> > > m_contactDisplayLabelDetails;
    QList<QList<QPair<QLatin1String, QLatin1String> > > m_groupDisplayLabelDetails;
};

#endif /* CNTDISPLAYLABEL_H_ */
