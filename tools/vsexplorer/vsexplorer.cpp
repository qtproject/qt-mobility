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

#include <QApplication>
#include <QObject>
#include <QTextStream>
#include <QFile>
#include <QSocketNotifier>
#include <cstdio>
#include <cstdlib>
#include <QMap>
#include <QDir>
#include <QStringList>
#include <QSet>

#include <qvaluespace.h>
#include <qvaluespacesubscriber.h>
#include <qvaluespacepublisher.h>

#ifdef USE_READLINE
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <QThread>
#include <QMutex>
#include <QEvent>
#include <QWaitCondition>
#include <pthread.h>
#endif

#ifdef Q_OS_WIN
#include <QThread>
#endif

QTM_USE_NAMESPACE

static bool terminateRequested = false;

class VSExplorer : public QObject
{
Q_OBJECT
public:
    VSExplorer();

    void printError();
    void printHelp();
    void ls();
    void dump();
    void pwdCmd();
    void ls(const QString &abs, bool);
    void subscribe();
    void unsubscribe();
    void quit();
    void suppress();
    void listwatchers();
    void watch(const QString &);
    void unwatch(const QString &);
    void set(const QString &, const QString &);
    void clear(const QString &);
    void subscriptions();

    QString path() const
    {
        return pwd.path();
    }

public slots:
    void processLine(const QString &);

private slots:
    void contentsChanged();
    void interestChanged(const QString &attribute, bool interested);

private:
    void lsPath(QValueSpaceSubscriber *, int = 0, bool = false);

    bool isSuppress;
    QValueSpaceSubscriber pwd;
    QValueSpacePublisher prov;
    QSet<QValueSpaceSubscriber *> subs;
    QSet<QValueSpacePublisher *> watchers;
};
static VSExplorer * vse = 0;

class LineInput :
#if defined(USE_READLINE) || defined(Q_OS_WIN)
    public QThread
#else
    public QObject
#endif
{
    Q_OBJECT

public:
    LineInput();

signals:
    void line(const QString &);

#if defined(USE_READLINE)
protected:
    virtual bool event(QEvent *);
    virtual void run();

private:
    QMutex mutex;
    QWaitCondition wait;
#elif defined(Q_OS_WIN)
protected:
    void run();

private:
    QFile ts;
#else
private slots:
    void readyRead();

private:
    QFile ts;
    QSocketNotifier *sock;
#endif
};

VSExplorer::VSExplorer()
: isSuppress(false), pwd(QLatin1String("/")), prov(QLatin1String("/"))
{
}

void VSExplorer::interestChanged(const QString &attribute, bool interested)
{
    Q_ASSERT(sender());

    if (!isSuppress) {
        QValueSpacePublisher *obj = static_cast<QValueSpacePublisher *>(sender());
        fprintf(stdout, "\nInterest Changed: %s ... %s %d\n",
                qPrintable(obj->path()), qPrintable(attribute), interested);
    }
}

static QString variantToString( const QVariant& var )
{
    if ( var.type() == QVariant::StringList )
        return var.toStringList().join(QLatin1String(", "));
    else
        return var.toString();
}

void VSExplorer::contentsChanged()
{
    Q_ASSERT(sender());

    if(!isSuppress) {
        QValueSpaceSubscriber *subscriber = static_cast<QValueSpaceSubscriber *>(sender());
        fprintf(stdout, "\nChanged: %s\n", subscriber->path().toAscii().constData());
    }
}

void VSExplorer::printError()
{
    fprintf(stdout, "Come again?\n");
    fflush(stdout);
}

void VSExplorer::printHelp()
{
    fprintf(stdout, "help/?: Print this message\n");
    fprintf(stdout, "quit: Exit VSExplorer\n");
    fprintf(stdout, "ls: List contents of path\n");
    fprintf(stdout, "pwd: Print current working directory\n");
    fprintf(stdout, "subscribe: Subscribe to path\n");
    fprintf(stdout, "unsubscribe: Unsubscribe from path\n");
    fprintf(stdout, "suppress: Toggle suppression of publish messages\n");
    fprintf(stdout, "subscriptions: List current subscriptions\n");
    fprintf(stdout, "set <key> <value>: Set app layer<key> to <value>\n");
    fprintf(stdout, "clear <key>: Clear app layer <key>\n");
    fprintf(stdout, "cd <path>: Change working path\n");
    fprintf(stdout, "watch <path>: Add a watch for the path\n");
    fprintf(stdout, "unwatch <path>: Remove a watch for the path\n");
    fprintf(stdout, "watchers: List paths for which a watch is active\n");
    fflush(stdout);
}

void VSExplorer::ls()
{
    lsPath(&pwd);
    fflush(stdout);
}

void VSExplorer::ls(const QString &abs, bool all)
{
    QValueSpaceSubscriber subscriber(abs);
    lsPath(&subscriber, 0, all);
    fflush(stdout);
}


void VSExplorer::lsPath(QValueSpaceSubscriber * p, int indent, bool showHidden)
{
    QVariant var = p->value();
    bool spaceRequired = false;
    if(!var.isNull()) {
        fprintf(stdout, "Value: '%s' (%s)\n",
                variantToString(var).toAscii().constData(), var.typeName());
        spaceRequired = true;
    }

    foreach (const QString &path, p->subPaths()) {
        if (!showHidden && path.startsWith(QLatin1Char('.')))
            continue;

        if(spaceRequired) {
            fprintf(stdout, "\n");
            spaceRequired = false;
        }
        for(int ii = 0; ii < indent; ++ii) fprintf(stdout, "\t");
        fprintf(stdout, "%s/", path.toAscii().constData());
        QVariant value = p->value(path);
        if(!value.isNull()) {
            if(path.length() < 30) {
                for(int ii = 0; ii < 30 - path.length(); ++ii)
                    fprintf(stdout, " ");
            } else {
                fprintf(stdout, "    ");
            }

            fprintf(stdout, " '%s' (%s)",
                    variantToString(value).toAscii().constData(), value.typeName());
        }
        fprintf(stdout, "\n");
    }
}

void VSExplorer::listwatchers()
{
    if(watchers.isEmpty()) {
        fprintf(stdout, "No watchers.\n");
    } else {
        fprintf(stdout, "Current watchers:\n");
        foreach (QValueSpacePublisher *obj, watchers)
            fprintf(stdout, "\t%s\n", obj->path().toAscii().constData());
    }

    fflush(stdout);
}

void VSExplorer::subscriptions()
{
    if(subs.isEmpty()) {
        fprintf(stdout, "No subscriptions.\n");
    } else {
        fprintf(stdout, "Current subscriptions:\n");

        foreach (QValueSpaceSubscriber *subscriber, subs)
            fprintf(stdout, "\t%s\n", subscriber->path().toAscii().constData());
    }

    fflush(stdout);
}

void VSExplorer::subscribe()
{
    QValueSpaceSubscriber *subscriber = new QValueSpaceSubscriber;
    subscriber->setPath(&pwd);
    QObject::connect(subscriber, SIGNAL(contentsChanged()),
                     this, SLOT(contentsChanged()));
    subs.insert(subscriber);

    fprintf(stdout, "OK\n");
    fflush(stdout);
}

void VSExplorer::unsubscribe()
{
    foreach (QValueSpaceSubscriber *subscriber, subs) {
        if (subscriber->path() == pwd.path()) {
            subs.remove(subscriber);
            delete subscriber;
            fprintf(stdout, "OK\n");
            fflush(stdout);
            return;
        }
    }

    fprintf(stdout, "No subscription.\n");
    fflush(stdout);
}

void VSExplorer::quit()
{
    fprintf(stdout, "Bye, bye.\n");
    fflush(stdout);
    terminateRequested = true;
}

void VSExplorer::watch(const QString &path)
{
    foreach (QValueSpacePublisher *obj, watchers) {
        if (obj->path() == path)
            return;
    }

    QValueSpacePublisher * newObject = new QValueSpacePublisher(path);
    watchers.insert(newObject);
    QObject::connect(newObject, SIGNAL(interestChanged(QString,bool)),
                     this, SLOT(interestChanged(QString,bool)));
}

void VSExplorer::unwatch(const QString &path)
{
    foreach (QValueSpacePublisher *obj, watchers) {
        if (obj->path() == path) {
            watchers.remove(obj);
            delete obj;
            return;
        }
    }
}

void VSExplorer::suppress()
{
    if (isSuppress) {
        isSuppress = false;
        fprintf(stdout, "Suppression off.\n");
    } else {
        isSuppress = true;
        fprintf(stdout, "Suppression on.\n");
    }
    fflush(stdout);
}

void VSExplorer::set(const QString &name, const QString &value)
{
    if (name.startsWith(QLatin1Char('/')))
        prov.setValue(name, value);
    else if (pwd.path().endsWith(QLatin1Char('/')))
        prov.setValue(pwd.path() + name, value);
    else
        prov.setValue(pwd.path() + QLatin1Char('/') + name, value);
}

void VSExplorer::clear(const QString &name)
{
    if (name.startsWith(QLatin1Char('/')))
        prov.resetValue(name);
    else if (pwd.path().endsWith(QLatin1Char('/')))
        prov.resetValue(pwd.path() + name);
    else
        prov.resetValue(pwd.path() + QLatin1Char('/') + name);
}

void VSExplorer::processLine(const QString &line)
{
    QStringList cmds = line.trimmed().split(QLatin1Char(' '));

    if(cmds.isEmpty()) {
        return;
    }

    const QString & cmd = cmds.at(0);
    if (cmd.isEmpty()) {

    } else if (cmd == QLatin1String("ls") && 1 == cmds.count()) {
        ls();
    } else if (cmd == QLatin1String("dump")) {
        dump();
    } else if (cmd == QLatin1String("pwd")) {
        pwdCmd();
    } else if (cmd == QLatin1String("ls") && 2 <= cmds.count()) {
        QStringList newCmds = cmds;
        newCmds.removeFirst();
        bool lsAll = false;
        if (newCmds.first() == QLatin1String("-a")) {
            lsAll = true;
            newCmds.removeFirst();
        }
        QString newPath = newCmds.join(QLatin1String(" "));
        if (newPath.startsWith(QLatin1Char('"')) && newPath.endsWith(QLatin1Char('"'))) {
            newPath = newPath.mid(1);
            newPath = newPath.left(newPath.length() - 1);
        }
        if(newPath.isEmpty()) {
            ls(pwd.path(), lsAll);
        } else if (newPath.startsWith(QLatin1Char('/'))) {
            ls(newPath, lsAll);
        } else {
            QString oldPath = pwd.path();
            if (!oldPath.endsWith(QLatin1Char('/')))
                oldPath.append(QLatin1Char('/'));
            oldPath.append(newPath);
            oldPath = QDir::cleanPath(oldPath);
            ls(oldPath, lsAll);
        }

    } else if (cmd == QLatin1String("cd") && 2 <= cmds.count()) {
        QStringList newCmds = cmds;
        newCmds.removeFirst();
        QString newPath = newCmds.join(QLatin1String(" "));
        if (newPath.startsWith(QLatin1Char('"')) && newPath.endsWith(QLatin1Char('"'))) {
            newPath = newPath.mid(1);
            newPath = newPath.left(newPath.length() - 1);
        }
        if (newPath.startsWith(QLatin1Char('/'))) {
            pwd.setPath(newPath);
        } else {
            QString oldPath = pwd.path();
            if (!oldPath.endsWith(QLatin1Char('/')))
                oldPath.append(QLatin1Char('/'));
            oldPath.append(newPath);
            oldPath = QDir::cleanPath(oldPath);
            pwd.setPath(oldPath);
        }
    } else if (cmd == QLatin1String("unwatch") && 2 <= cmds.count()) {
        QStringList newCmds = cmds;
        newCmds.removeFirst();
        QString newPath = newCmds.join(QLatin1String(" "));
        QString finalPath;
        if (newPath.startsWith(QLatin1Char('"')) && newPath.endsWith(QLatin1Char('"'))) {
            newPath = newPath.mid(1);
            newPath = newPath.left(newPath.length() - 1);
        }
        if (newPath.startsWith(QLatin1Char('/'))) {
            finalPath = QValueSpaceSubscriber(newPath).path();
        } else {
            QString oldPath = pwd.path();
            if (!oldPath.endsWith(QLatin1Char('/')))
                oldPath.append(QLatin1Char('/'));
            oldPath.append(newPath);
            oldPath = QDir::cleanPath(oldPath);
            finalPath = QValueSpaceSubscriber(oldPath).path();
        }
        unwatch(finalPath);
    } else if (cmd == QLatin1String("watch") && 2 <= cmds.count()) {
        QStringList newCmds = cmds;
        newCmds.removeFirst();
        QString newPath = newCmds.join(QLatin1String(" "));
        QString finalPath;
        if (newPath.startsWith(QLatin1Char('"')) && newPath.endsWith(QLatin1Char('"'))) {
            newPath = newPath.mid(1);
            newPath = newPath.left(newPath.length() - 1);
        }
        if (newPath.startsWith(QLatin1Char('/'))) {
            finalPath = QValueSpaceSubscriber(newPath).path();
        } else {
            QString oldPath = pwd.path();
            if (!oldPath.endsWith(QLatin1Char('/')))
                oldPath.append(QLatin1Char('/'));
            oldPath.append(newPath);
            oldPath = QDir::cleanPath(oldPath);
            finalPath = QValueSpaceSubscriber(oldPath).path();
        }
        watch(finalPath);
    } else if (cmd == QLatin1String("set") && 3 == cmds.count()) {
        set(cmds.at(1).trimmed(), cmds.at(2).trimmed());
    } else if (cmd == QLatin1String("clear") && 2 == cmds.count()) {
        clear(cmds.at(1).trimmed());
    } else if ((cmd == QLatin1String("subscribe") || cmd == QLatin1String("sub")) &&
               1 == cmds.count()) {
        subscribe();
    } else if ((cmd == QLatin1String("unsubscribe") || cmd == QLatin1String("unsub")) &&
               1 == cmds.count()) {
        unsubscribe();
    } else if ((cmd == QLatin1String("?") || cmd == QLatin1String("help")) && 1 == cmds.count()) {
        printHelp();
    } else if ((cmd == QLatin1String("quit") || cmd == QLatin1String("exit")) &&
               1 == cmds.count()) {
        quit();
    } else if (cmd == QLatin1String("suppress") && 1 == cmds.count()) {
        suppress();
    } else if (cmd == QLatin1String("watchers") && 1 == cmds.count()) {
        listwatchers();
    } else if (cmd == QLatin1String("subscriptions") && 1 == cmds.count()) {
        subscriptions();
    } else {
        printError();
    }
}

#ifdef USE_READLINE
extern "C" {
    char * item_generator(const char * text, int num);
    char * command_generator(const char * text, int num);
    char ** item_completion(const char * text, int start, int end);
}
#endif

LineInput::LineInput()
{
#if defined(USE_READLINE)
    rl_completion_append_character = '\0';
    rl_attempted_completion_function = item_completion;
    rl_completer_quote_characters = "\"";
    rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&(";
    rl_filename_quote_characters = " ";
    QObject::connect(this, SIGNAL(finished()), qApp, SLOT(quit()));
    QObject::connect(this, SIGNAL(terminated()), qApp, SLOT(quit()));
    start();
#elif defined(Q_OS_WIN)
    ts.open(stdin, QIODevice::ReadOnly);
    QObject::connect(this, SIGNAL(finished()), qApp, SLOT(quit()));
    QObject::connect(this, SIGNAL(terminated()), qApp, SLOT(quit()));
    start();

    fprintf(stdout, "%s > ", vse->path().constData());
    fflush(stdout);
#else
    ts.open(stdin, QIODevice::ReadOnly);
    sock = new QSocketNotifier(ts.handle(), QSocketNotifier::Read, this);
    QObject::connect(sock, SIGNAL(activated(int)), this, SLOT(readyRead()));

    fprintf(stdout, "%s > ", qPrintable(vse->path()));
    fflush(stdout);
#endif
}

#if !defined(USE_READLINE) && !defined(Q_OS_WIN)
void LineInput::readyRead()
{
    QByteArray line = ts.readLine();

    emit this->line(QString::fromLocal8Bit(line));

    if(terminateRequested)
        exit(0);

    fprintf(stdout, "%s > ", qPrintable(vse->path()));
    fflush(stdout);
}
#endif

#ifdef USE_READLINE
#define TEXTEVENTTYPE (QEvent::User + 10)
struct TextEvent : public QEvent
{
    TextEvent(char *line)
        : QEvent((QEvent::Type)TEXTEVENTTYPE), data(line) {}

    char * data;
};

bool LineInput::event(QEvent *e)
{
    if(e->type() == TEXTEVENTTYPE) {
        TextEvent * te = static_cast<TextEvent *>(e);
        emit line(te->data);
        free(te->data);

        mutex.lock();
        wait.wakeAll();
        mutex.unlock();

        return true;
    } else {
        return QThread::event(e);
    }
}

char ** item_completion(const char * text, int start, int end)
{
    char ** matches = (char **)NULL;
    const char * non_space = text;
    while(*non_space == ' ') ++non_space;


    if(start == non_space - text)
        matches = rl_completion_matches(text, command_generator);
    else
        matches = rl_completion_matches(text, item_generator);

    rl_attempted_completion_over = 1;
    return (matches);
}

char * command_generator(const char * t, int num)
{
    static QList<QByteArray> children;

    if(0 == num) {
        children.clear();

        // Command
        static const char * commands[] = { "help ",
                                           "quit ",
                                           "pwd ",
                                           "ls ",
                                           "subscribe ",
                                           "unsubscribe ",
                                           "suppress ",
                                           "subscriptions ",
                                           "set ",
                                           "clear ",
                                           "cd " };

        for(unsigned int ii = 0; ii < sizeof(commands) / sizeof(char *); ++ii)
            if(0 == ::strncmp(commands[ii], t, strlen(t)))
                children.append(commands[ii]);
    }

    if(children.isEmpty())
        return 0;

    char * rv = (char *)malloc(children.at(0).length() + 1);
    ::memcpy(rv, children.at(0).constData(), children.at(0).length() + 1);
    children.removeFirst();

    return rv;
}

char * item_generator(const char * t, int num)
{
    static QStringList children;

    rl_filename_completion_desired = 1;
    rl_filename_quoting_desired = 1;
    if(0 == num) {

        children.clear();

        // Path
        QString text = QString::fromLocal8Bit(t);
        QString textExt;
        QString textBase;

        int last = text.lastIndexOf('/');
        if(-1 == last) {
            textExt = text;
        } else {
            textBase = text.left(last);
            textExt = text.mid(last + 1);
        }

        QString vsBase;

        if (!textBase.startsWith(QLatin1Char('/'))) {
            QString in = vse->path();
            if (!in.endsWith(QLatin1Char('/')))
                vsBase = in + QLatin1Char('/') + textBase;
            else
                vsBase = in + textBase;
        } else {
            vsBase = textBase;
        }

        QValueSpaceSubscriber subscriber(vsBase);

        QStringList schildren = subscriber.subPaths();

        foreach(QString child, schildren) {
            if(child.startsWith(textExt)) {
                QString completion;
                completion.append(textBase);
                if(!completion.isEmpty())
                    completion.append(QLatin1Char('/'));
                completion.append(child.toAscii());
                completion.append(QLatin1Char('/'));
                children.append(completion);
            }
        }
    }

    if(children.isEmpty())
        return 0;

    QByteArray child = children.takeFirst().toLocal8Bit();
    char *rv = (char *)malloc(child.length() + 1);
    ::memcpy(rv, child.constData(), child.length() + 1);

    return rv;
}


void LineInput::run()
{
    while(true) {
        /* Get a line from the user. */
        mutex.lock();
        QString prompt = vse->path();
        prompt.append(" > ");
        mutex.unlock();
        char *line_read = readline (prompt.toLocal8Bit().constData());

        /* If the line has any text in it,
           save it on the history. */
        if (line_read && *line_read)
            add_history (line_read);

        mutex.lock();
        TextEvent * e = new TextEvent(line_read);
        QApplication::postEvent(this, e);
        wait.wait(&mutex);
        if(terminateRequested) {
            mutex.unlock();
            return;
        } else {
            mutex.unlock();
        }
    }
}
#endif

#ifdef Q_OS_WIN
void LineInput::run()
{
    while (!terminateRequested) {
        fprintf(stdout, "%s > ", vse->path().constData());
        fflush(stdout);

        QByteArray l = ts.readLine();
        emit line(QString::fromLocal8Bit(l.constData(), l.length()));
    }
}

#endif

void usage(char * app)
{
    fprintf(stderr, "Usage: %s [-s] [-d]\n", app);
    fprintf(stderr, "   -s     a valuespace manager instance is created\n");
    fprintf(stderr, "   -d     the tree content is dumped to command line\n");
    exit(-1);
}

void dodump(QValueSpaceSubscriber *subscriber)
{
    foreach (const QString &child, subscriber->subPaths()) {
        if (child.isEmpty())
            continue;

        QValueSpaceSubscriber subItem;
        subItem.setPath(subscriber);
        subItem.cd(child);
        dodump(&subItem);
    }

    QVariant var = subscriber->value();
    fprintf(stdout, "%s '%s' %s\n",
            subscriber->path().toAscii().constData(),
            variantToString(var).toAscii().constData(),
            var.typeName());
}

void VSExplorer::dump()
{
    QValueSpaceSubscriber subscriber;
    subscriber.setPath(&pwd);
    dodump(&subscriber);
    fflush(stdout);
}

void VSExplorer::pwdCmd()
{
    fprintf(stdout, "Working directory: %s\n", pwd.path().toLatin1().constData());
    fflush(stdout);
}


int main(int argc, char ** argv)
{
    QApplication app(argc, argv, true);

    bool manager = false;
    bool dump = false;
    for(int ii = 1; ii < argc; ++ii) {
        if(0 == ::strcmp(argv[ii], "-s"))
            manager = true;
        else if(0 == ::strcmp(argv[ii], "-d"))
            dump = true;
        else
            usage(argv[0]);
    }

    if(manager)
        QValueSpace::initValueSpaceServer();

    if(dump) {
        QValueSpaceSubscriber subscriber(QLatin1String("/"));
        dodump(&subscriber);
        return 0;
    } else {
        vse = new VSExplorer;
        LineInput li;


#ifdef Q_OS_WIN
        QObject::connect(&li, SIGNAL(line(QString)),
                         vse, SLOT(processLine(QString)), Qt::BlockingQueuedConnection);
#else
        QObject::connect(&li, SIGNAL(line(QString)),
                         vse, SLOT(processLine(QString)));
#endif

        int rv = app.exec();
        delete vse;
        return rv;
    }
}

#include "vsexplorer.moc"
