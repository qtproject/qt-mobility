#include "tap.h"

bool TapSensorFilter::filter(QTapReading *reading)
{
    QString output;
    switch (reading->tapDirection()) {
    case QTapReading::X:         output = "X";         break;
    case QTapReading::Y:         output = "Y";         break;
    case QTapReading::Z:         output = "Z";         break;
    case QTapReading::X_Pos:     output = "X pos";     break;
    case QTapReading::Y_Pos:     output = "Y pos";     break;
    case QTapReading::Z_Pos:     output = "Z pos";     break;
    case QTapReading::X_Neg:     output = "X neg";     break;
    case QTapReading::Y_Neg:     output = "Y neg";     break;
    case QTapReading::Z_Neg:     output = "Z neg";     break;
    case QTapReading::Undefined: output = "Undefined"; break;
    default: output = "Invalid enum value";
    }
    emit reading->isDoubleTap()?doubleHappened(output):singleHappened(output);
    return false; // don't store the reading in the sensor
}

