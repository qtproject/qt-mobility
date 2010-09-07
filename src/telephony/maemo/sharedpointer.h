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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSharedPointer_H
#define QSharedPointer_H

#include <QtCore/QObject>

namespace DBus
{

class ReferenceCounter;
class WeakData;
template <class T> class SharedPointer;
template <class T> class WeakPointer;

class WeakData
{
    Q_DISABLE_COPY(WeakData)

public:
    WeakData(ReferenceCounter *d) : d(d), weakref(0) { }

    ReferenceCounter *d;
    mutable QAtomicInt weakref;
};

class ReferenceCounter
{
    Q_DISABLE_COPY(ReferenceCounter)

public:
    inline ReferenceCounter() : strongref(0), wd(0) { }
    inline virtual ~ReferenceCounter() { if (wd) { wd->d = 0; } }

    inline void ref() { strongref.ref(); }
    inline bool deref() { return strongref.deref(); }

    mutable QAtomicInt strongref;
    WeakData *wd;
};

template <class T>
class SharedPointer
{
public:
    inline SharedPointer() : d(0) { }
    explicit inline SharedPointer(T *d) : d(d) { if (d) { d->ref(); } }
    inline SharedPointer(const SharedPointer<T> &o) : d(o.d) { if (d) { d->ref(); } }
    explicit inline SharedPointer(const WeakPointer<T> &o)
    {
        if (o.wd && o.wd->d) {
            d = static_cast<T*>(o.wd->d);
            d->ref();
        } else {
            d = 0;
        }
    }
    inline ~SharedPointer()
    {
        if (d && !d->deref()) {
            delete d;
        }
    }

    inline void reset()
    {
        SharedPointer<T>().swap(*this);
    }

    inline T *data() const { return d; }
    inline const T *constData() const { return d; }
    inline T *operator->() { return d; }
    inline T *operator->() const { return d; }

    inline bool isNull() const { return !d; }
    inline operator bool() const { return !isNull(); }
    inline bool operator!() const { return isNull(); }

    inline bool operator==(const SharedPointer<T> &o) const { return d == o.d; }
    inline bool operator!=(const SharedPointer<T> &o) const { return d != o.d; }
    inline bool operator==(const T *ptr) const { return d == ptr; }
    inline bool operator!=(const T *ptr) const { return d != ptr; }

    inline SharedPointer<T> &operator=(const SharedPointer<T> &o)
    {
        SharedPointer<T>(o).swap(*this);
        return *this;
    }

    inline void swap(SharedPointer<T> &o)
    {
        T *tmp = d;
        d = o.d;
        o.d = tmp;
    }

    template <class X>
    static inline SharedPointer<T> staticCast(const SharedPointer<X> &src)
    {
        return SharedPointer<T>(static_cast<T*>(src.data()));
    }

    template <class X>
    static inline SharedPointer<T> dynamicCast(const SharedPointer<X> &src)
    {
        return SharedPointer<T>(dynamic_cast<T*>(src.data()));
    }

    template <class X>
    static inline SharedPointer<T> constCast(const SharedPointer<X> &src)
    {
        return SharedPointer<T>(const_cast<T*>(src.data()));
    }

private:
    friend class WeakPointer<T>;

    T *d;
};

template <class T>
class WeakPointer
{
public:
    inline WeakPointer() : wd(0) { }
    inline WeakPointer(const WeakPointer<T> &o) : wd(o.wd) { if (wd) { wd->weakref.ref(); } }
    inline WeakPointer(const SharedPointer<T> &o)
    {
        if (o.d) {
            if (!o.d->wd) {
                o.d->wd = new WeakData(o.d);
            }
            wd = o.d->wd;
            wd->weakref.ref();
        } else {
            wd = 0;
        }
    }
    inline ~WeakPointer()
    {
        if (wd && !wd->weakref.deref()) {
            if (wd->d) {
                wd->d->wd = 0;
            }
            delete wd;
        }
    }

    inline bool isNull() const { return !wd || !wd->d || wd->d->strongref == 0; }
    inline operator bool() const { return !isNull(); }
    inline bool operator!() const { return isNull(); }

    inline WeakPointer<T> &operator=(const WeakPointer<T> &o)
    {
        WeakPointer<T>(o).swap(*this);
        return *this;
    }

    inline WeakPointer<T> &operator=(const SharedPointer<T> &o)
    {
        WeakPointer<T>(o).swap(*this);
        return *this;
    }

    inline void swap(WeakPointer<T> &o)
    {
        WeakData *tmp = wd;
        wd = o.wd;
        o.wd = tmp;
    }

    SharedPointer<T> toStrongRef() const { return SharedPointer<T>(*this); }

private:
    friend class SharedPointer<T>;

    WeakData *wd;
};

} // DBus

#endif
