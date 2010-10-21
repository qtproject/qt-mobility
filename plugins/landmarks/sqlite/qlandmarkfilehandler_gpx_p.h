/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLANDMARKFILEHANDLER_GPX_P_H
#define QLANDMARKFILEHANDLER_GPX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qlandmark.h>
#include <qlandmarkmanagerengine.h>

class QXmlStreamReader;
class QXmlStreamWriter;

class QIODevice;

QTM_USE_NAMESPACE

class QLandmarkFileHandlerGpx : public QObject
{
    Q_OBJECT

public:
    enum Behavior{ExportAll, ExportSubset};//requirement whether we need to export all landmarks given
                                         //or only those that are capable of being exported ie have valid coords.

    QLandmarkFileHandlerGpx(const volatile bool *cancel = 0);
    ~QLandmarkFileHandlerGpx();

    QList<QLandmark> waypoints() const;
    void setWaypoints(const QList<QLandmark> &waypoints);

    QList<QList<QLandmark> > tracks() const;
    void setTracks(const QList<QList<QLandmark> > &tracks);

    QList<QList<QLandmark> > routes() const;
    void setRoutes(const QList<QList<QLandmark> > &routes);

    bool importData(QIODevice *device);
    bool exportData(QIODevice *device, const QString &nsPrefix = QString());

    QString errorString() const;
    QLandmarkManager::Error error();

    void setBehavior(Behavior behavior);

private:
    bool readGpx();
    bool readWaypoint(QLandmark &landmark, const QString &elementName);
    bool readRoute(QList<QLandmark> &route);
    bool readTrack(QList<QLandmark> &track);
    bool readTrackSegment(QList<QLandmark> &track);

    bool writeGpx();
    bool writeWaypoint(const QLandmark &landmark, const QString &elementName);
    bool writeRoute(const QList<QLandmark> &route);
    bool writeTrack(const QList<QLandmark> &track);

    QString m_nsPrefix;
    QString m_ns;

    QList<QLandmark> m_waypoints;
    QList<QList<QLandmark> > m_tracks;
    QList<QList<QLandmark> > m_routes;

    QXmlStreamReader *m_reader;
    QXmlStreamWriter *m_writer;

    QLandmarkManager::Error m_errorCode;
    QString m_errorString;
    Behavior m_behavior;
    volatile const bool *m_cancel;
};

#endif // #ifndef QLANDMARKGPXHANDLER_H
