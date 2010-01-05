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
#ifndef PSPATHMAPPERSESSION_H
#define PSPATHMAPPERSESSION_H

#include <qmobilityglobal.h>
#include <e32base.h>

#include <QByteArray>

QTM_BEGIN_NAMESPACE

class CPSPathMapperServer;
class PathMapper;

class CPSPathMapperServerSession : public CSession2
{
public:
    CPSPathMapperServerSession(const PathMapper &aPathMapper);
    virtual ~CPSPathMapperServerSession();

    void ServiceL(const RMessage2 &aMessage);
    void DispatchMessageL(const RMessage2 &aMessage);

    void GetChildrenLengthL(const RMessage2 &aMessage);
    void GetChildrenL(const RMessage2 &aMessage);
    void ChildPathsLengthL(const RMessage2 &aMessage);
    void ChildPathsL(const RMessage2 &aMessage);
    void ResolvePathLengthL(const RMessage2 &aMessage);
    void ResolvePathL(const RMessage2 &aMessage);

protected:
    void PanicClient(const RMessage2 &aMessage, TInt aPanic) const;

private:
    const PathMapper &iPathMapper;
    QByteArray iResultByteArray;
};

QTM_END_NAMESPACE

#endif  //PSPATHMAPPERSESSION_H

// End of File
