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

#include "qsystemreadwritelock_p.h"
#include <QFile>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <QRegExp>
#include <QCryptographicHash>
#include <QDir>

QTM_BEGIN_NAMESPACE

class QSystemReadWriteLockPrivate
{
public:
    enum SemIndex {
        ActiveReaders = 0,
        TotalWriters,
        ActiveWriterSem,
        NumInstances
    };

    QSystemReadWriteLockPrivate():id(-1),semId(-1),
                    key(QString()),keyFileName(QString()),
                    error(QSystemReadWriteLock::NoError),
                    errorString(QString()){};
    void setError(const QString &errorString);

    key_t id;
    int semId;
    QString key;
    QString keyFileName;

    QSystemReadWriteLock::SystemReadWriteLockError error;
    QString errorString;
};

void QSystemReadWriteLockPrivate::setError(const QString &_errorString)
{
    errorString = _errorString;
    switch (errno) {
        case EPERM:
        case EACCES:
            error = QSystemReadWriteLock::PermissionDenied;
            break;
        case ERANGE:
        case ENOMEM:
        case ENOSPC:
            error = QSystemReadWriteLock::OutOfResources;
            break;
        case EIDRM:
            error = QSystemReadWriteLock::NotFound;
            break;
        default:
            error = QSystemReadWriteLock::UnknownError;
     }
}

/*
    This function is taken from qcore_unix_p.h(qt_safe_open)
    If/when QSystemReadWriteLock is integrated into Qt,
    this function defintion may replaced by the sharedmemory
    definition.
 */
static inline int safe_open(const char *pathname, int flags, mode_t mode = 0777)
{
#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
    register int fd;

    do {
        fd = ::open(pathname, flags, mode);
    } while (fd == -1 && errno == EINTR);

    // unknown flags are ignored, so we have no way of verifying if
    // O_CLOEXEC was accepted
    if (fd != -1)
        ::fcntl(fd, F_SETFD, FD_CLOEXEC);
    return fd;
}

/*
    This function is taken from qsharedmemory_unix.cpp.
    If/when QSystemReadWriteLock is integrated into Qt,
    this function defintion may replaced by the sharedmemory
    definition.
 */
int createUnixKeyFile(const QString &fileName)
{
    if (QFile::exists(fileName)) {
        return 0;
    }

    int fd = safe_open(QFile::encodeName(fileName).constData(),
            O_EXCL | O_CREAT | O_RDWR, 0640);

    if (-1 == fd) {
        if (errno == EEXIST)
            return 0;
        return -1;
    } else {
        close(fd);
    }
    return 1;
}

/*
    This function is taken from qsharedmemory_unix.cpp.
    If/when QSystemReadWriteLock is integrated into Qt,
    the function definition may replaced by the sharedmemory
    definition.
 */
QString makePlatformSafeKey(const QString &key,
        const QString &prefix)
{
    if (key.isEmpty())
        return QString();

    QString result = prefix;

    QString part1 = key;
    part1.remove(QRegExp(QLatin1String("[^A-Za-z]")));
    result.append(part1);

    QByteArray hex = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha1).toHex();
    result.append(QLatin1String(hex));
    return QDir::tempPath() + QLatin1Char('/') + result;
}

/*
   \class QSystemReadWriteLock

   \brief The QSystemReadWriteLock class provides read-write locking between
   processes.

   A read-write lock is a synchronization tool for protecting resources that can
   be accessed for reading and writing. This type of lock is useful if you want
   to allow multiple processes/threads to have simultaneous read-only access, but as soon
   as one thread wants to write to the resource, all other threads must be
   blocked until the writing is complete.

   QSystemReadWriteLock behaves much like the QReadWriteLock class, but it also
   works across multiple processes (although it also works perfectly well,
   albeit slightly less efficiently, in a single process).  The system
   resources are cleaned up when the last QSystemReadWriteLock instance
   is destroyed.

   The behaviour of QSystemReadWriteLock is such that writers will
   take precedence over readers.  When writers are waiting, any new
   readers must wait until all writers complete.  That is, writers
   can starve readers.
 */

/*
   Implementation Details:

   The QSystemReadWriteLock class uses Linux kernel semaphores to synchronize
   access between readers and writers.

   4 semaphores are used in total.
   activeReaders - number of active readers
   TotalWriters - number of active and pending writers

   ActiveWriterSem - indicates whether a writer is active

   NumInstances - counts the number of instances of QSystemReadWriteLock
                  objects there for a given key.  This is needed
                  to determine whether system resources are
                  safe to clean up during an instance's destruction.

  activeReaders, TotalWriters and NumInstance are used as counters and
  not in the way typical semaphores are used.  e.g.a value of 5 for
  activeReaders indicates that there are currently 5 readers.

  ActiveWriterSem is used as a typical semaphore.  A value of 1
  means there's an free slot for a writer to become active.
 */

/*
  Construct a system read write lock from the provided \a key.

  The \a mode parameter is only used in Unix systems to handle the case
  where underlying system resources such as semaphores survive a process
  crash.  In this case, the next process to instatiate a lock will
  get the resources that survived the crash, and unless \mode is
  Create, the resources will not be reset.
 */
QSystemReadWriteLock::QSystemReadWriteLock(const QString &key, AccessMode mode)
{
    d = new QSystemReadWriteLockPrivate;
    d->key = key;
    d->keyFileName = makePlatformSafeKey(key, QLatin1String("qipc_systemsem_"));

    int built = createUnixKeyFile(d->keyFileName);
    if (built == -1) {
        d->setError(QObject::tr("QSystemReadWriteLock::QSystemReadWriteLock: "
                    "unable to make key file for key: %1(%2)")
                    .arg(key).arg(QString::fromLatin1(strerror(errno))));
        return;
    }

    // Get the unix key for the created file
    d->id = ftok(QFile::encodeName(d->keyFileName).constData(), 'Q');
    if (d->id == -1) {
        d->setError(QObject::tr("QSystemReadWriteLock::QSystemReadWriteLock: "
                        "ftok failed for key %1(%2)")
                    .arg(key).arg(QString::fromLatin1(strerror(errno))));
        return;
    }

    d->semId = ::semget(d->id, 4, IPC_CREAT | IPC_EXCL | 0666);
    bool created = false;
    if (d->semId == -1) {
        if (errno == EEXIST) {
            d->semId = ::semget(d->id, 4,
                    (mode == QSystemReadWriteLock::Create)?IPC_CREAT|0666:0);
            if (d->semId == -1) {
                d->setError(QObject::tr("QSystemReadWriteLock::QSystemReadWriteLock: "
                                    "Unable to access semaphore set for key %1(%2)")
                            .arg(key).arg(QString::fromLatin1(strerror(errno))));
                return;
            }
        } else {
            d->setError(QObject::tr("QSystemReadWriteLock:QSystemReadWriteLock: "
                        "Unable to access semaphore set for key %1(%2)")
                        .arg(key).arg(QString::fromLatin1(strerror(errno))));
            return;
        }
    } else {
        created = true;
    }

    Q_ASSERT(d->semId != -1);
    if (created || mode == QSystemReadWriteLock::Create) {
        typedef union {
            int val;
            struct semid_ds *buf;
            ushort *array;
        }semun;

        semun counterInitVal; //initial value for a "counter" semaphores
        counterInitVal.val = 0;

        semun activeWriterInitVal;//initial value for the "active writer" semaphore
        activeWriterInitVal.val = 1;

        if (-1 == ::semctl(d->semId, QSystemReadWriteLockPrivate::ActiveReaders,
                    SETVAL, counterInitVal) ||
                -1 == ::semctl(d->semId, QSystemReadWriteLockPrivate::TotalWriters,
                    SETVAL, counterInitVal) ||
                -1 == ::semctl(d->semId, QSystemReadWriteLockPrivate::ActiveWriterSem,
                    SETVAL, activeWriterInitVal) ||
                -1 == ::semctl(d->semId, QSystemReadWriteLockPrivate::NumInstances,
                    SETVAL, counterInitVal))
        {
            d->setError(QObject::tr("QSystemReadWriteLock::QSystemReadWriteLock: "
                        "Unable to reset semaphore set for key %1(%2)")
                        .arg(key).arg(QString::fromLatin1(strerror(errno))));
            QFile::remove(d->keyFileName);
            ::semctl(d->semId, 0, IPC_RMID);
            d->semId = -1;
            return;
        }
    }

    struct sembuf op;
    op.sem_num = QSystemReadWriteLockPrivate::NumInstances;
    op.sem_op = 1;
    op.sem_flg = SEM_UNDO;
    int semoprv = ::semop(d->semId, &op, 1);
    if (semoprv == -1) {
        d->setError(QObject::tr("QSystemReadWriteLock::QSystemReadWriteLock: "
                            "Unable to increment NumInstances semaphore "
                            "for key%1(%2)").arg(key).arg(QString::fromLatin1(strerror(errno))));
        d->semId = -1;
        return;
    }
}

/*
  Destroy the lock instance.  The last QSystemReadWriteLock instance
  for a particular key will destroy the underlying system resources.
 */
QSystemReadWriteLock::~QSystemReadWriteLock()
{
    if (d->semId != -1) {
        //decrement and check that there are 0 instances
        //be aware these 2 operations occur together atomically
        struct sembuf ops[2];
        ops[0].sem_num = QSystemReadWriteLockPrivate::NumInstances;
        ops[0].sem_op = -1;
        ops[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

        ops[1].sem_num = QSystemReadWriteLockPrivate::NumInstances;
        ops[1].sem_op = 0;
        ops[1].sem_flg = IPC_NOWAIT;

        //if successful, then delete the semaphore set
        int semoprv  = ::semop(d->semId, ops, 2);
        if (semoprv == 0) {
            if(::semctl(d->semId, 0, IPC_RMID) == -1) {
                qWarning("QSystemReadWriteLock::~QSystemReadWriteLock: "
                         "Unable to remove semaphore %s(%s)",
                         d->key.toLocal8Bit().constData(), strerror(errno));
            }
            QFile::remove(d->keyFileName);
        } else {
            if (errno == EAGAIN) {
                //wasn't 0 instances so just decrement the NumInstances semaphore
                if (::semop(d->semId, ops, 1) == -1) {
                    qWarning("QSystemReadWriteLock::~QSystemReadWriteLock: "
                             "Unable to decrement NumInstances semaphore for key %s(%s)",
                             d->key.toLocal8Bit().constData(), strerror(errno));
                }
            } else {
                qWarning("QSystemReadWriteLock::~QSystemReadWriteLock: "
                         "Unable to decrement and check NumInstances semaphore for key %s(%s)",
                         d->key.toLocal8Bit().constData(), strerror(errno));
                ::semop(d->semId, ops, 1);//try decrement anyway
            }
        }

    }

    Q_ASSERT(d);
    delete d;
    d = 0;
}

/*
  Return the key of the lock as passed to the constructor.
 */
QString QSystemReadWriteLock::key() const
{
    return d->key;
}

/*
  Locks the lock for reading.  The function will block if any thread/process
  has locked for writing.
 */
bool QSystemReadWriteLock::lockForRead()
{
    if (d->semId == -1) {
        d->errorString = QObject::tr("QSystemReadWriteLock::lockForRead: "
                                     "Unable to lock for read for key %1"
                                     "(Lock had not been correctly initialized)").arg(d->key);
        d->error = UnknownError;
        return false;
    }

    struct sembuf ops[2];

    ops[0].sem_num = QSystemReadWriteLockPrivate::ActiveReaders;
    ops[0].sem_op = 1;
    ops[0].sem_flg = SEM_UNDO;

    ops[1].sem_num = QSystemReadWriteLockPrivate::TotalWriters;
    ops[1].sem_op = 0;
    ops[1].sem_flg = 0;

    if (-1 == ::semop(d->semId, ops, 2)) {
        d->setError(QObject::tr("QSystemReadWriteLock::lockForRead: "
                                "Unable to lock for read for key %1(%2)")
                    .arg(d->key).arg(QString::fromLatin1(strerror(errno))));
        return false;
    } else {
        d->errorString.clear();
        d->error = NoError;
        return true;
    }
}

/*
   Locks the lock for writing.  This function will block if another thread/process
   has locked for reading or writing.
 */
bool QSystemReadWriteLock::lockForWrite()
{
    if (d->semId == -1) {
        d->errorString = QObject::tr("QSystemReadWriteLock::lockForWrite: "
                                     "Unable to lock for write for key %1"
                                     "(Lock had not been correctly initialized)").arg(d->key);
        d->error = UnknownError;
        return false;
    }

    struct sembuf op;
    op.sem_num = QSystemReadWriteLockPrivate::TotalWriters;
    op.sem_op = 1;
    op.sem_flg = SEM_UNDO;

    int semoprv = ::semop(d->semId, &op, 1);
    if (semoprv == -1) {
        d->setError(QObject::tr("QSystemReadWriteLock::lockForWrite: "
                                "Could not increment TotalWriters semaphore for key %1(%2)")
                    .arg(d->key).arg(QString::fromLatin1(strerror(errno))));
        return false;
    }

    op.sem_num = QSystemReadWriteLockPrivate::ActiveReaders;
    op.sem_op = 0;
    op.sem_flg = 0;
    semoprv = ::semop(d->semId, &op, 1);
    if (semoprv == -1) {
        d->setError(QObject::tr("QSystemReadWriteLock::lockForWrite: "
                                "Could not detect if all readers were finished for key %1(%2)")
                    .arg(d->key).arg(QString::fromLatin1(strerror(errno))));

        // Decrement our write lock
        op.sem_num = QSystemReadWriteLockPrivate::TotalWriters;
        op.sem_op = -1;
        op.sem_flg = SEM_UNDO;
        ::semop(d->semId, &op, 1);
        return false;
    } else {
        op.sem_num = QSystemReadWriteLockPrivate::ActiveWriterSem;
        op.sem_op = -1;
        op.sem_flg =SEM_UNDO;
        semoprv = ::semop(d->semId, &op, 1);

        if (semoprv == -1) {
            d->setError(QObject::tr("QSystemReadWriteLock::lockForWrite: "
                                    "Could not decrement ActiveWriterSem semaphore for key %1(%2)")
                        .arg(d->key).arg(QString::fromLatin1(strerror(errno))));

            op.sem_num = QSystemReadWriteLockPrivate::TotalWriters;
            op.sem_op = -1;
            op.sem_flg = 0;
            ::semop(d->semId, &op, 1);
            return false;
        }
        d->errorString.clear();
        d->error = NoError;
        return true;
    }
}

/*
  Release the lock.
 */
void QSystemReadWriteLock::unlock()
{
    if (d->semId == -1) {
        d->errorString = QObject::tr("QSystemReadWriteLock::unlock: "
                                     "Unable to unlock for key %1"
                                     "(Lock had not been correctly initialized)").arg(d->key);
        d->error = UnknownError;
        return;
    }

    struct sembuf op;
    op.sem_num = QSystemReadWriteLockPrivate::ActiveReaders;
    op.sem_op = -1;
    op.sem_flg = SEM_UNDO | IPC_NOWAIT;
    if (::semop(d->semId, &op, 1) == 0) {
        //do nothing, succeeded in decrementing number of readers
    } else if (errno == EAGAIN){ //no readers, so check for writers
            struct sembuf ops[3];
            ops[0].sem_num = QSystemReadWriteLockPrivate::ActiveWriterSem;
            ops[0].sem_op = 0;
            ops[0].sem_flg = IPC_NOWAIT;

            ops[1].sem_num = QSystemReadWriteLockPrivate::ActiveWriterSem;
            ops[1].sem_op = 1;
            ops[1].sem_flg = SEM_UNDO;

            ops[2].sem_num = QSystemReadWriteLockPrivate::TotalWriters;
            ops[2].sem_op = -1;
            ops[2].sem_flg = SEM_UNDO;

            if (::semop(d->semId, ops, 3) == -1) {
                if (errno != EAGAIN) {
                    d->setError(QObject::tr("QSystemSemaphoreWriteLock::unlock: "
                                            "Unable to check and update writer semaphores for key %1(%2)")
                                .arg(d->key).arg(QString::fromLatin1(strerror(errno))));
                    return;
                } //Note: EAGAIN indicates that ActiveWriterSem is has a non zero value
                  //indicating there is no current writer, so nothing needs to be done
            }
    } else {
        //error in decrementing readers
        d->setError(QObject::tr("QSystemReadWriteLock::unlock: "
                                "Unable to decrement ActiveReaders semaphore for key %1(%2)")
                    .arg(d->key).arg(QString::fromLatin1(strerror(errno))));
        return;
    }
    d->errorString.clear();
    d->error = NoError;
}

/*
  Returns the error code of the last encountered error
 */
QSystemReadWriteLock::SystemReadWriteLockError QSystemReadWriteLock::error() const {
    return d->error;
}

/*
  Returns a string describing the last encountered error
 */
QString QSystemReadWriteLock::errorString() const
{
    return d->errorString;
}

QTM_END_NAMESPACE

