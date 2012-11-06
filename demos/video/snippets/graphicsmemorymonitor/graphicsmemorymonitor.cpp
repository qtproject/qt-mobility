/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphicsmemorymonitor.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>
#include <QtGui/QWidget>

#ifdef Q_OS_SYMBIAN
#ifdef GRAPHICSMEMORYMONITOR_EGL
#define EGL_RESOURCE_PROFILING_SUPPORT
#endif
#endif

#ifdef EGL_RESOURCE_PROFILING_SUPPORT
#include <EGL/egl.h>
typedef EGLBoolean (*PFNEGLQUERYPROFILINGDATANOK)(EGLDisplay, EGLint, EGLint *, EGLint, EGLint *);
#endif

//#define VERBOSE_TRACE

inline QDebug qtTrace() { return qDebug() << "[graphicsmemorymonitor]"; }
#ifdef VERBOSE_TRACE
inline QDebug qtVerboseTrace() { return qtTrace(); }
#else
inline QNoDebug qtVerboseTrace() { return QNoDebug(); }
#endif

static const qint64 DefaultUpdateInterval = 1000;

// Helper function
QString humanReadableSize(qint64 size)
{
    static const qint64 KB = 1024;
    static const qint64 MB = KB * KB;
    static const qint64 GB = MB * KB;
    static const qint64 TB = GB * KB;
    QString suffix = "B";
    qint64 factor = 1;
    qreal n = size;
    if (size > TB) {
        suffix = "TB";
        factor = TB;
    } else if (size > GB) {
        suffix = "GB";
        factor = GB;
    } else if (size > MB) {
        suffix = "MB";
        factor = MB;
    } else if (size > KB) {
        suffix = "KB";
        factor = KB;
    }
    n = n / qreal(factor);
    return QString("%1 %2").arg(n, 6, 'f', 2).arg(suffix);
}

class GraphicsMemoryMonitorPrivate : public QObject
{
    Q_OBJECT
public:
    GraphicsMemoryMonitorPrivate(GraphicsMemoryMonitor *parent);
    ~GraphicsMemoryMonitorPrivate();

private:
    void ensureEglExtension();

public slots:
    void update();

public:
    GraphicsMemoryMonitor *const q_ptr;
    QTimer *m_timer;
#ifdef EGL_RESOURCE_PROFILING_SUPPORT
    EGLDisplay m_eglDisplay;
    PFNEGLQUERYPROFILINGDATANOK m_eglQueryProfilingDataNOK;
#endif
#ifdef Q_OS_SYMBIAN
    TFullName m_processName;
#endif
    QByteArray m_buffer;
    qint64 m_currentProcessId;
    qint64 m_totalMemory;
    qint64 m_usedMemory;
    qint64 m_currentProcessUsage;
    QList<GraphicsMemoryMonitor::ProcessInfo> m_processList;
};

GraphicsMemoryMonitorPrivate::GraphicsMemoryMonitorPrivate(GraphicsMemoryMonitor *parent)
:   QObject(parent)
,   q_ptr(parent)
,   m_timer(new QTimer(this))
#ifdef EGL_RESOURCE_PROFILING_SUPPORT
,   m_eglDisplay(EGL_NO_DISPLAY)
,   m_eglQueryProfilingDataNOK(0)
#endif
,   m_currentProcessId(0)
,   m_totalMemory(0)
,   m_usedMemory(0)
,   m_currentProcessUsage(0)
{
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(update()));
    m_timer->setInterval(DefaultUpdateInterval);
    m_timer->start();
#ifdef Q_OS_SYMBIAN
    m_currentProcessId = RProcess().Id().operator unsigned int();
#endif
}

GraphicsMemoryMonitorPrivate::~GraphicsMemoryMonitorPrivate()
{

}

void GraphicsMemoryMonitorPrivate::ensureEglExtension()
{
#ifdef EGL_RESOURCE_PROFILING_SUPPORT
    if (!m_eglQueryProfilingDataNOK) {
        m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        qtTrace() << "GraphicsMemoryMonitorPrivate::ensureEglExtension" << "eglDisplay" << m_eglDisplay;
        if (m_eglDisplay == EGL_NO_DISPLAY)
            qtTrace() << "No EGL display";
        else
            m_eglQueryProfilingDataNOK = reinterpret_cast<PFNEGLQUERYPROFILINGDATANOK>(eglGetProcAddress("eglQueryProfilingDataNOK"));
        if (m_eglQueryProfilingDataNOK)
            q_ptr->emit activeChanged();
        else
            qtTrace() << "eglQueryProfilingDataNOK extension not found";
    }
#endif
}

void GraphicsMemoryMonitorPrivate::update()
{
    qint64 totalMemory = 0;
    qint64 usedMemory = 0;
    qint64 currentProcessUsage = 0;
    QList<GraphicsMemoryMonitor::ProcessInfo> processList;

#ifdef EGL_RESOURCE_PROFILING_SUPPORT
    ensureEglExtension();
    if (m_eglQueryProfilingDataNOK) {
        // Ensure we have an EGL connection, in case we are using raster graphics
        eglInitialize(m_eglDisplay, 0, 0);

        GraphicsMemoryMonitor::ProcessInfo process;
        process.pid = 0;
        EGLint count = 0;
        m_eglQueryProfilingDataNOK(m_eglDisplay, EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK | EGL_PROF_QUERY_GLOBAL_BIT_NOK, NULL, 0, &count);
        m_buffer.resize(count * sizeof(EGLint));
        m_buffer.fill(0);
        EGLint *data = reinterpret_cast<EGLint *>(m_buffer.data());
        m_eglQueryProfilingDataNOK(m_eglDisplay, EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK | EGL_PROF_QUERY_GLOBAL_BIT_NOK, data, count, &count);
        for (int i=0; i<count; ++i) {
            switch (data[i]) {
            case EGL_PROF_TOTAL_MEMORY_NOK:
                totalMemory = data[++i];
                break;
            case EGL_PROF_USED_MEMORY_NOK:
                usedMemory = data[++i];
                break;
            case EGL_PROF_PROCESS_ID_NOK:
            {
                if (process.pid)
                    processList << process;
                process.pid = quint64(data[i+1]) + (quint64(data[i+2]) << 32);
                process.name = QString();
                i += 2;
                RProcess p;
                const TInt err = p.Open(TProcessId(process.pid));
                if (!err) {
                    m_processName = p.FullName();
                    TPtr8 name = m_processName.Collapse();
                    process.name = QString(reinterpret_cast<const char *>(name.PtrZ()));
                    p.Close();
                }
                process.privateUsage = 0;
                process.sharedUsage = 0;
            }
                break;
            case EGL_PROF_PROCESS_USED_PRIVATE_MEMORY_NOK:
                process.privateUsage = data[++i];
                break;
            case EGL_PROF_PROCESS_USED_SHARED_MEMORY_NOK:
                process.sharedUsage = data[++i];
                break;
            default:
                qtTrace() << "unknown data" << "index" << i << "data" << reinterpret_cast<void *>(data[i]);
                break;
            }
        }
        if (process.pid)
            processList << process;
    }
#endif // EGL_RESOURCE_PROFILING_SUPPORT

    bool processListChanged = (m_processList.count() != processList.count());
    if (!processListChanged) {
        for (int i=0; !processListChanged && i<m_processList.count(); ++i) {
            const GraphicsMemoryMonitor::ProcessInfo &oldProcess = m_processList.at(i);
            const GraphicsMemoryMonitor::ProcessInfo &newProcess = processList.at(i);
            processListChanged = (oldProcess.pid != newProcess.pid) ||
                                 (oldProcess.privateUsage != newProcess.privateUsage) ||
                                 (oldProcess.sharedUsage != newProcess.sharedUsage);
        }
    }

    foreach (GraphicsMemoryMonitor::ProcessInfo process, m_processList)
        if (m_currentProcessId == process.pid)
            currentProcessUsage = process.privateUsage + process.sharedUsage;

    bool availableMemoryChanged = false;
    if (totalMemory != m_totalMemory) {
        m_totalMemory = totalMemory;
        availableMemoryChanged = true;
        q_ptr->emit totalMemoryChanged(q_ptr->totalMemory());
        q_ptr->emit totalMemoryHumanReadableChanged(q_ptr->totalMemoryHumanReadable());
    }
    if (usedMemory != m_usedMemory) {
        m_usedMemory = usedMemory;
        availableMemoryChanged = true;
        q_ptr->emit usedMemoryChanged(q_ptr->usedMemory());
        q_ptr->emit usedMemoryHumanReadableChanged(q_ptr->usedMemoryHumanReadable());
    }
    bool hasChanged = availableMemoryChanged;
    if (availableMemoryChanged) {
        q_ptr->emit availableMemoryChanged(q_ptr->availableMemory());
        q_ptr->emit availableMemoryHumanReadableChanged(q_ptr->availableMemoryHumanReadable());
    }
    if (currentProcessUsage != m_currentProcessUsage) {
        m_currentProcessUsage = currentProcessUsage;
        hasChanged = true;
        q_ptr->emit currentProcessUsageChanged(q_ptr->currentProcessUsage());
        q_ptr->emit currentProcessUsageHumanReadableChanged(q_ptr->currentProcessUsageHumanReadable());
    }
    if (processListChanged) {
        m_processList = processList;
        hasChanged = true;
        q_ptr->emit processListChanged(m_processList);
    }
    if (hasChanged)
        q_ptr->emit changed();
}

GraphicsMemoryMonitor::GraphicsMemoryMonitor(QObject *parent)
:   QObject(parent)
,   d_ptr(0)
{
    qtTrace() << "GraphicsMemoryMonitor::GraphicsMemoryMonitor";
    d_ptr = new GraphicsMemoryMonitorPrivate(this);
    d_ptr->update();
}

GraphicsMemoryMonitor::~GraphicsMemoryMonitor()
{
    qtTrace() << "GraphicsMemoryMonitor::~GraphicsMemoryMonitor";
}

bool GraphicsMemoryMonitor::active() const
{
#ifdef EGL_RESOURCE_PROFILING_SUPPORT
    return (d_ptr->m_eglQueryProfilingDataNOK != 0);
#else
    return false;
#endif
}

qint64 GraphicsMemoryMonitor::updateInterval() const
{
    return d_ptr->m_timer->isActive() ? d_ptr->m_timer->interval() : 0;
}

void GraphicsMemoryMonitor::setUpdateInterval(qint64 value)
{
    if (value != updateInterval()) {
        if (value) {
            d_ptr->m_timer->setInterval(value);
            d_ptr->m_timer->start();
        } else {
            d_ptr->m_timer->stop();
        }
    }
}

qint64 GraphicsMemoryMonitor::totalMemory() const
{
    return d_ptr->m_totalMemory;
}

QString GraphicsMemoryMonitor::totalMemoryHumanReadable() const
{
    return humanReadableSize(totalMemory());
}

qint64 GraphicsMemoryMonitor::usedMemory() const
{
    return d_ptr->m_usedMemory;
}

QString GraphicsMemoryMonitor::usedMemoryHumanReadable() const
{
    return humanReadableSize(usedMemory());
}

qint64 GraphicsMemoryMonitor::availableMemory() const
{
    return totalMemory() - usedMemory();
}

QString GraphicsMemoryMonitor::availableMemoryHumanReadable() const
{
    return humanReadableSize(availableMemory());
}

qint64 GraphicsMemoryMonitor::currentProcessUsage() const
{
    return d_ptr->m_currentProcessUsage;
}

QString GraphicsMemoryMonitor::currentProcessUsageHumanReadable() const
{
    return humanReadableSize(currentProcessUsage());
}

const QList<GraphicsMemoryMonitor::ProcessInfo> &GraphicsMemoryMonitor::processList() const
{
    return d_ptr->m_processList;
}

void GraphicsMemoryMonitor::update()
{
    d_ptr->update();
}

void GraphicsMemoryMonitor::trace()
{
    qtTrace() << "GraphicsMemoryMonitor::trace";
    qtTrace() << "    Total memory:         " << d_ptr->m_totalMemory;
    qtTrace() << "    Used memory:          " << d_ptr->m_usedMemory;
    qtTrace() << "    Available memory:     " << d_ptr->m_totalMemory - d_ptr->m_usedMemory;
    qtTrace() << "    Current process ID:   " << d_ptr->m_currentProcessId;
    qtTrace() << "    Current process usage:" << d_ptr->m_currentProcessUsage;
    foreach (ProcessInfo process, d_ptr->m_processList) {
        qtTrace() << "    Process ID" << process.pid << "name" << process.name;
        qtTrace() << "        Private usage:" << process.privateUsage;
        qtTrace() << "        Shared usage: " << process.sharedUsage;
    }
}

#include "graphicsmemorymonitor.moc"
