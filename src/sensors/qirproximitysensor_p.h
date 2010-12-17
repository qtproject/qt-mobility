
#ifndef QIRPROXIMITYSENSOR_P_H
#define QIRPROXIMITYSENSOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QTM_BEGIN_NAMESPACE

class QIRProximityReadingPrivate
{
public:
    QIRProximityReadingPrivate()
        : reflectance(0)
    {
    }

    /*
     * Note that this class is copied so you may need to implement
     * a copy constructor if you have complex types or pointers
     * as values.
     */

    qreal reflectance;
};

QTM_END_NAMESPACE

#endif
