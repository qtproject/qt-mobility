#ifndef QWMPGLOBAL_H
#define QWMPGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) && defined(QT_MAKEDLL)
#   if defined(QT_BUILD_WMP_LIB)
#       define Q_WMP_EXPORT Q_DECL_EXPORT
#   else
#       define Q_WMP_EXPORT Q_DECL_IMPORT
#   endif
#elif defined(Q_OS_WIN) && defined(QT_DLL)
#   define Q_WMP_EXPORT Q_DECL_IMPORT
#endif
#if !defined(Q_WMP_EXPORT)
#   if defined(QT_SHARED)
#       define Q_WMP_EXPORT Q_DECL_EXPORT
#   else
#       define Q_WMP_EXPORT
#   endif
#endif

#endif
