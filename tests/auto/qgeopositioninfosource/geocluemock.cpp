/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "geocluemock.h"
#include "qgeopositioninfosource_geocluemaster_p.h"
#include <QTimerEvent>
#include <QDebug>

// todo all calls to geocluemock should be done by calling slots to ensure
// synchronization (the mocked library functions call mockend directly even
// though its in different thread). now going a bit dangerously, but seems to work..
static GeoclueMock* lg_currentMockend = 0;
static QString lg_currentJournal;
static bool lg_geocluemock_gcmaster_get_default = true;
static bool lg_geocluemock_gcmaster_create_client = true;
static bool lg_geocluemock_gcmaster_set_requirements = true;
static bool lg_geocluemock_gcmaster_create_position = true;
static bool lg_geocluemock_geoclue_velocity_new = true;

static bool lg_geocluemock_suppress_regular_updates = false;
static bool lg_geocluemock_suppress_single_update = false;
static bool lg_geocluemock_suppress_velocity_update = false;

static int lg_position_fields = -1;
static int lg_velocity_fields = -1;
static double lg_position_latitude = -1;

// These symbols override the symbols in the actual geoclue library;
// they are used to mock the backend. ld won't resolve the actualy symbols
// in the real geoclue library because they are already defined here.
// However if this dodgy logic someday becomes untrue, make a shim library
// with the same functionality and use LD_PRELOAD or LD_LIBRARY_PATH to
// force loading of the shim library. For CI system, you will also need to reimplement the main()
// of the testcase so that it can actually set one of those variables and then restart
// the application.

gulong g_signal_connect_data      (gpointer  instance,
                                               const gchar *detailed_signal,
                                               GCallback  c_handler,
                                               gpointer  data,
                                               GClosureNotify  destroy_data = NULL,
                                               GConnectFlags  connect_flags = (GConnectFlags)0)
{
    Q_UNUSED(instance)
    Q_UNUSED(destroy_data)
    Q_UNUSED(connect_flags)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= g_signal_connect (_data)");
#endif
    lg_currentMockend->setPositionSource(static_cast<QObject*>(data));
    if (QString::fromAscii(detailed_signal).contains("position-changed")) {
        lg_currentMockend->setPositionChangedCallback(c_handler);
    } else if (QString::fromAscii(detailed_signal).contains("velocity-changed")) {
        lg_currentMockend->setVelocityChangedCallback(c_handler);
    }
    return 1; // dummy
}

guint	 g_signal_handlers_disconnect_matched (gpointer	  instance,
                                               GSignalMatchType  mask,
                                               guint  signal_id = guint(0),
                                               GQuark detail = GQuark(0),
                                               GClosure *closure = NULL,
                                               gpointer  func = NULL,
                                               gpointer  data = NULL)
{
    Q_UNUSED(instance)
    Q_UNUSED(mask)
    Q_UNUSED(signal_id)
    Q_UNUSED(detail)
    Q_UNUSED(closure)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= g_signal_handlers_disconnect_by_func (_matched)");
#endif
    lg_currentMockend->disconnectSignal(func, data);
    return 1; // dummy
}

void geoclue_accuracy_get_details (GeoclueAccuracy      *accuracy,
                                   GeoclueAccuracyLevel *level,
                                   double               *horizontal_accuracy,
                                   double               *vertical_accuracy)
{
    // Dummy, todo take from journal file
    if (accuracy) {
        *level = (GeoclueAccuracyLevel)6;
        *horizontal_accuracy = 8;
        *vertical_accuracy = 9;
    } else {
        *level = (GeoclueAccuracyLevel)0;
        *horizontal_accuracy = 0;
        *vertical_accuracy = 0;
    }
}

GeoclueMaster *geoclue_master_get_default (void)
{
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= geoclue_master_get_default");
#endif
    if (lg_geocluemock_gcmaster_get_default) {
        GeoclueMaster* master = (GeoclueMaster*)g_object_new (G_TYPE_OBJECT, NULL);
        return master;
    } else {
        return NULL;
    }
}

GeoclueMasterClient *geoclue_master_create_client (GeoclueMaster *master,
                                                   char         **object_path,
                                                   GError       **error)
{
    Q_UNUSED(master)
    Q_UNUSED(object_path)
    Q_UNUSED(error)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= geoclue_master_create_client");
#endif
    if (lg_geocluemock_gcmaster_create_client) {
        GeoclueMasterClient* client = (GeoclueMasterClient*)g_object_new (G_TYPE_OBJECT, NULL);
        return client;
    } else {
        if (error) {
            *error = g_error_new ((GQuark)1, // dummy
                                  2,         // dummy
                                  "=mock= Failing gcmaster_create_client on purpose.");
        }
        return NULL;
    }
}

gboolean geoclue_master_client_set_requirements (GeoclueMasterClient   *client,
                                                 GeoclueAccuracyLevel   min_accuracy,
                                                 int                    min_time,
                                                 gboolean               require_updates,
                                                 GeoclueResourceFlags   allowed_resources,
                                                 GError               **error)
{
    Q_UNUSED(client)
    Q_UNUSED(min_accuracy)
    Q_UNUSED(min_time)
    Q_UNUSED(require_updates)
    Q_UNUSED(allowed_resources)
    Q_UNUSED(error)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= geoclue_master_client_set_requirements");
#endif
    if (lg_geocluemock_gcmaster_set_requirements) {
        return true;
    } else {
        if (error) {
            *error = g_error_new ((GQuark)1, // dummy
                                  2,         // dummy
                                  "=mock= Failing gcmaster_client set requirements on purpose.");
        }
        return false;
    }
}

GeocluePosition *geoclue_master_client_create_position (GeoclueMasterClient *client, GError **error)
{
    Q_UNUSED(client)
    Q_UNUSED(error)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= geoclue_master_client_create_position");
#endif
    if (lg_geocluemock_gcmaster_create_position) {
        GeocluePosition* position = (GeocluePosition*)g_object_new (G_TYPE_OBJECT, NULL);
        return position;
    } else {
        if (error) {
            *error = g_error_new ((GQuark)1, // dummy
                                  2,         // dummy
                                  "=mock= Failing gcmaster_client create position on purpose.");
        }
        return NULL;
    }
}

GeoclueVelocity *geoclue_velocity_new (const char *service,
                                       const char *path)
{
    Q_UNUSED(service)
    Q_UNUSED(path)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= geoclue_velocity_new");
#endif
    if (lg_geocluemock_geoclue_velocity_new) {
        GeoclueVelocity* velocity = (GeoclueVelocity*)g_object_new (G_TYPE_OBJECT, NULL);
        return velocity;
    } else {
        return NULL;
    }
}

void geoclue_position_get_position_async (GeocluePosition         *position,
                                     GeocluePositionCallback  callback,
                                     gpointer                 userdata)
{
    Q_UNUSED(position)
    Q_ASSERT(lg_currentMockend);
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=mocked= geoclue_position_get_position_async");
#endif
    lg_currentMockend->setPositionSource(static_cast<QObject*>(userdata));
    lg_currentMockend->setPositionCallback(callback);
    lg_currentMockend->singleUpdate();
}

GeoclueMock::GeoclueMock() :
    m_positionSource(0),
    m_positionChangedCallback(0), m_velocityChangedCallback(0),
    m_positionCallback(0)
{
    g_type_init();
    lg_currentMockend = this;
}

GeoclueMock::~GeoclueMock()
{
    lg_currentMockend = 0;
}

void GeoclueMock::setPositionSource(QObject *positionSource)
{
    m_positionSource = positionSource;
}

void GeoclueMock::start()
{
    Q_ASSERT(QFile::exists(lg_currentJournal));
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=GeoclueMock start");
#endif
    stop(); // just in case
    // If file is already open, continue reading it.
    if (!m_currentJournalFile.isOpen()) {
        m_currentJournalFile.setFileName(lg_currentJournal);
        if (!m_currentJournalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "Journal opening failed:" << m_currentJournalFile.fileName();
            Q_ASSERT(false);
        }
    }
    UPDATE_TYPE update = readNextEntry();

    while (update != INVALID_UPDATE) {
        while (update == SINGLE_UPDATE) {
            update = readNextEntry(); // skip single entries (just stores values)
        }
        if (update == REGULAR_UPDATE) {
            m_regular.timerId = startTimer(m_regular.timeout);
        } else if (update == VELOCITY_UPDATE) {
            m_velocity.timerId = startTimer(m_velocity.timeout);
        } else {
            Q_ASSERT(false);
        }
        update = readNextEntry();
    }
}

void GeoclueMock::stop()
{
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=GeoclueMock= stop");
#endif
    killTimer(m_single.timerId);
    killTimer(m_regular.timerId);
    killTimer(m_velocity.timerId);
    m_single.timerId = 0;
    m_regular.timerId = 0;
    m_velocity.timerId = 0;
    if (m_currentJournalFile.isOpen())
        m_currentJournalFile.close();
    m_positionChangedCallback = 0;
    m_positionCallback = 0;
    m_velocityChangedCallback = 0;
    m_positionSource = 0;
}

void GeoclueMock::timerEvent(QTimerEvent *event)
{
    if (!m_positionSource) {
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug("=GeoclueMock= timerEvent no interested position sources, ignoring update.");
#endif
        return;
    }

    // check which timer
    if (event->timerId() == m_regular.timerId) {
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug("=GeoclueMock= regular update.");
#endif
        if (m_positionChangedCallback && !lg_geocluemock_suppress_regular_updates)
            (*m_positionChangedCallback)((GeocluePosition*)1,  // dummy
                                         lg_position_fields == -1? (GeocluePositionFields)m_regular.fields : (GeocluePositionFields)lg_position_fields,
                                         m_regular.timestamp,
                                         lg_position_latitude == -1? m_regular.latitude : lg_position_latitude,
                                         m_regular.longitude,
                                         m_regular.altitude,
                                         (GeoclueAccuracy*)1,  // dummy
                                         (gpointer)m_positionSource);
        if (m_regular.repeats > 0) {
            m_regular.repeats--;
            return;
        }
        qDebug("-=-=-=-=-= ran out of regular updates");
        killTimer(m_regular.timerId);
    } else if (event->timerId() == m_velocity.timerId) {
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug("=GeoclueMock= velocity update.");
#endif
        if (m_velocityChangedCallback && !lg_geocluemock_suppress_velocity_update)
            (*m_velocityChangedCallback)((GeoclueVelocity*)1, // dummy
                                         lg_velocity_fields == -1? (GeoclueVelocityFields)m_velocity.fields : (GeoclueVelocityFields)lg_velocity_fields,
                                         1, // dummy
                                         m_velocity.speed,
                                         1, // dummy
                                         1, // dummy
                                         (gpointer)m_positionSource);


        if (m_velocity.repeats > 0) {
            m_velocity.repeats--;
            return;
        }
        qDebug("-=-=-=-=-= ran out of velocity updates");
        killTimer(m_velocity.timerId);
    } else if (event->timerId() == m_single.timerId) {
        Q_ASSERT(m_single.updateType);
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug("=GeoclueMock= single position update.");
#endif
        if (m_positionCallback && !lg_geocluemock_suppress_single_update)
            (*m_positionCallback)((GeocluePosition*)1, // dummy
                                  lg_position_fields == -1? (GeocluePositionFields)m_single.fields : (GeocluePositionFields)lg_position_fields,
                                  m_single.timestamp,
                                  lg_position_latitude == -1? m_single.latitude : lg_position_latitude,
                                  m_single.longitude,
                                  m_single.altitude,
                                  (GeoclueAccuracy*)1, // dummy
                                  (GError*)0,
                                  (gpointer)m_positionSource);
        // Single update only, remove the callback.
        disconnectSignal(gpointer(m_positionCallback), (gpointer)m_positionSource);
        killTimer(m_single.timerId);
    } else {
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug() << "=GeoclueMock= astray timer, killing and ignoring: " << event->timerId();
#endif
        killTimer(event->timerId()); // Something has gone astray, kill and ignore
        return;
    }
    // no more repeats
    UPDATE_TYPE update = readNextEntry();

    while (update != INVALID_UPDATE) {
        while (update == SINGLE_UPDATE) {
            update = readNextEntry(); // skip single entries (just stores values)
        }
        if (update == REGULAR_UPDATE) {
            m_regular.timerId = startTimer(m_regular.timeout);
        } else if (update == VELOCITY_UPDATE) {
            m_velocity.timerId = startTimer(m_velocity.timeout);
        } else {
            Q_ASSERT(false);
        }
        update = readNextEntry();
    }
}

GeoclueMock::UPDATE_TYPE GeoclueMock::readNextEntry()
{
    QString line;
    QTextStream stream;

    line = m_currentJournalFile.readLine();
    while (line.length() > 0) {
        if (line.at(0) == '#') {
            line = m_currentJournalFile.readLine();
            continue;
        }
        if (line == "\n") {
            line = m_currentJournalFile.readLine();
            continue;
        }
        if (line == "REGULAR_POS_UPDATE\n") {
            line = m_currentJournalFile.readLine();
            stream.setString(&line);
            m_regular.updateType = REGULAR_UPDATE;
            parseRegularUpdate(stream);
            return REGULAR_UPDATE;
        }
        else if (line == "SINGLE_POS_UPDATE\n") {
            line = m_currentJournalFile.readLine();
            stream.setString(&line);
            parseSingleUpdate(stream);
            m_single.updateType = SINGLE_UPDATE;
            return SINGLE_UPDATE;

        }
        else if (line == "VELOCITY_UPDATE\n") {
            line = m_currentJournalFile.readLine();
            stream.setString(&line);
            m_velocity.updateType = VELOCITY_UPDATE;
            parseVelocityUpdate(stream);
            return VELOCITY_UPDATE;
        }       
        qCritical("Invalid input file for geoclue mocking.");
        Q_ASSERT(false);
    }
    return INVALID_UPDATE;
}

void GeoclueMock::parseRegularUpdate(QTextStream& stream)
{
    stream >> m_regular.repeats >> m_regular.timeout >> m_regular.timestamp >> m_regular.fields >> m_regular.latitude >> m_regular.longitude >> m_regular.altitude;
}

void GeoclueMock::parseSingleUpdate(QTextStream& stream)
{
    stream >> m_single.repeats >> m_single.timeout >> m_single.timestamp >> m_single.fields >> m_single.latitude >> m_single.longitude >> m_single.altitude;
}

void GeoclueMock::parseVelocityUpdate(QTextStream& stream)
{
    stream >> m_velocity.repeats >> m_velocity.timeout >>  m_velocity.fields >> m_velocity.speed;
}

void GeoclueMock::setPositionChangedCallback(void (*callback)())
{
    m_positionChangedCallback = (PositionChangedCallback)callback;
}

void GeoclueMock::setVelocityChangedCallback(void (*callback)())
{
    m_velocityChangedCallback = (VelocityChangedCallback)callback;
}

void GeoclueMock::setPositionCallback(GeocluePositionCallback callback)
{
    m_positionCallback = (PositionCallback)callback;
}

void GeoclueMock::disconnectSignal(gpointer callback, gpointer positionSource)
{
    if (positionSource != (gpointer)m_positionSource) {
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug("=GeoclueMock disconnectSignal requested without connecting.");
#endif
        return;
    }
    if (callback == (gpointer)m_positionChangedCallback) {
        m_positionChangedCallback = 0;
    } else if (callback == (gpointer)m_positionCallback) {
        m_positionCallback = 0;
    } else if (callback == (gpointer)m_velocityChangedCallback) {
        m_velocityChangedCallback = 0;
    }
    if (m_positionChangedCallback == 0 &&
            m_velocityChangedCallback == 0 &&
            m_positionCallback == 0 ) {
#ifdef TST_GEOCLUE_MOCK_TRACE
        qDebug("=GeoclueMock= no callbacks anymore, removing pos source reference");
#endif
        m_positionSource = 0;
    }
}

void GeoclueMock::singleUpdate()
{
#ifdef TST_GEOCLUE_MOCK_TRACE
    qDebug("=GeoclueMock= singleUpdate");
#endif
    Q_ASSERT(m_single.updateType == SINGLE_UPDATE);
    Q_ASSERT(m_positionSource);
    killTimer(m_single.timerId);
    m_single.timerId = startTimer(m_single.timeout);
}

// Misc functions to parametrize the behavior of the mockend
void geocluemock_setjournal(QString journal)
{
    lg_currentJournal = journal;
}

void geocluemock_set_gcmaster_get_default(bool value)
{
    lg_geocluemock_gcmaster_get_default = value;
}

void geocluemock_set_gcmaster_create_client(bool value)
{
    lg_geocluemock_gcmaster_create_client = value;
}

void geocluemock_set_gcmaster_set_requirements(bool value)
{
    lg_geocluemock_gcmaster_set_requirements = value;
}

void geocluemock_set_gcmaster_create_position(bool value)
{
    lg_geocluemock_gcmaster_create_position = value;
}

void geocluemock_set_geoclue_velocity_new(bool value)
{
    lg_geocluemock_geoclue_velocity_new = value;
}

void geocluemock_set_suppress_regular_updates(bool value)
{
    lg_geocluemock_suppress_regular_updates = value;
}

void geocluemock_set_suppress_single_update(bool value)
{
    lg_geocluemock_suppress_single_update = value;
}

void geocluemock_set_suppress_velocity_update(bool value)
{
    lg_geocluemock_suppress_velocity_update = value;
}

void geocluemock_set_position_fields(int fields)
{
    lg_position_fields = fields;
}

void geocluemock_unset_position_fields()
{
    lg_position_fields = -1;
}

void geocluemock_set_position_latitude(double latitude)
{
    lg_position_latitude = latitude;
}

void geocluemock_unset_position_latitude()
{
    lg_position_latitude = -1;
}

void geocluemock_set_velocity_fields(int fields)
{
    lg_velocity_fields = fields;
}

void geocluemock_unset_velocity_fields()
{
    lg_velocity_fields = -1;
}
