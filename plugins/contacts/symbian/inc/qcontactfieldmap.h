/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONTACTFIELDMAP_H
#define QCONTACTFIELDMAP_H

#include <QtCore>
#include <QSharedData>
#include <QMap>
#include <QPair>

/*!
 * Data class for shared data pointer.
 * 
 * Used by QContactFieldMap to prevent data duplication.
 */
class QContactFieldMapData : public QSharedData
{
public:
	// Maps used for String/UID lookup
	QMap<QString, int> mStringIdMap;
	QMap<int, QString> mIntIdMap;
};

/*!
 * QContactFieldMap
 * 
 * This class is used to quickly look up Symbian field UIDs that
 * correspond to a given Qt contact detail and field description.
 */
class QContactFieldMap
{
public:
	QContactFieldMap();
		
	// Lookup functions
	TUid symbianOsUid(const QString &detailType, const QString &field) const;
	QPair<QString, QString> qtDetailFieldPair(TUid uid) const;

private:
    int intIdentifier(const QString &fieldStringIdentifier) const;
    QString stringIdentifier(int fieldUid) const;
    QString dotJoin(const QString &detail, const QString &field) const;

    // Setup field translation
    void initializeMaps();
	
	// Shared data between instances.
	QSharedDataPointer<QContactFieldMapData> d;
};

#endif // QCONTACTFIELDMAP_H

