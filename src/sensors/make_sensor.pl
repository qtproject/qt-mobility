#!/usr/bin/perl

use strict;
use warnings;

use Carp;
local $Carp::CarpLevel;# = 1;

my $sensor = get_arg();
my $sensorbase = $sensor;
$sensorbase =~ s/Sensor$//;
my $reading = $sensorbase.'Reading';
my $reading_private = $reading.'Private';
my $filter = $sensorbase.'Filter';

my $filebase;
eval {
    $filebase = get_arg();
};
if ($@) {
    $filebase = lc($sensor);
}

my $pheader = $filebase."_p.h";
my $header = $filebase.".h";
my $source = $filebase.".cpp";

my $pguard = uc($pheader);
$pguard =~ s/\./_/g;

my $guard = uc($header);
$guard =~ s/\./_/g;

if (! -e $pheader) {
    print "Creating $pheader\n";
    open OUT, ">$pheader" or die $!;
    print OUT '
#ifndef '.$pguard.'
#define '.$pguard.'

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

#include "qsensor_p.h"

QTM_BEGIN_NAMESPACE

struct '.$reading_private.' : public QSensorReadingPrivate
{
    '.$reading_private.'()
        : myprop(0)
    {
    }

    qreal myprop;
};

QTM_END_NAMESPACE

#endif
';
    close OUT;
}

if (! -e $header) {
    print "Creating $header\n";
    open OUT, ">$header" or die $!;
    print OUT '
#ifndef '.$guard.'
#define '.$guard.'

#include <qsensor.h>

QTM_BEGIN_NAMESPACE

class '.$reading_private.';

class Q_SENSORS_EXPORT '.$reading.' : public QSensorReading
{
    Q_OBJECT
    Q_PROPERTY(qreal myprop READ myprop)
    DECLARE_READING('.$reading.')
public:
    qreal myprop() const;
    void setMyprop(qreal myprop);
};

// begin generated code
// end generated code

QTM_END_NAMESPACE

#endif
';
    close OUT;
}

if (-e $header) {
    open IN, "$header" or die $!;
    my @data = <IN>;
    close IN;
    my @tags = grep /^\/\/ (begin|end) generated code/, @data;
    if (scalar(@tags) == 2 &&
    $tags[0] eq "// begin generated code\n" &&
    $tags[1] eq "// end generated code\n") {
        print "Updating generated code in $header\n";
        open OUT, ">$header" or die $!;
        my $skip = 0;
        for (@data) {
            if ($_ eq $tags[0]) {
                print OUT $_;
                print OUT '
class Q_SENSORS_EXPORT '.$filter.' : public QSensorFilter
{
public:
    virtual bool filter('.$reading.' *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<'.$reading.'*>(reading)); }
};

class Q_SENSORS_EXPORT '.$sensor.' : public QSensor
{
    Q_OBJECT
public:
    explicit '.$sensor.'(QObject *parent = 0) : QSensor(parent)
    { setType('.$sensor.'::type); }
    virtual ~'.$sensor.'() {}
    '.$reading.' *reading() const { return static_cast<'.$reading.'*>(QSensor::reading()); }
    static const char *type;
};
';
                $skip = 1;
            }
            if ($_ eq $tags[1]) {
                $skip = 0;
            }
            if (!$skip) {
                print OUT $_;
            }
        }
    }
}

if (! -e $source) {
    print "Creating $source\n";
    open OUT, ">$source" or die $!;
    print OUT '
#include <'.$header.'>
#include "'.$pheader.'"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING('.$reading.')

/*!
    \class '.$reading.'
    \ingroup sensors

    \preliminary
    \brief The '.$reading.' class holds readings from the [X] sensor.

    [Fill this out]
*/

/*!
    \property '.$reading.'::myprop
    \brief [what does it hold?]
*/

/*!
    Returns [what?]
*/
qreal '.$reading.'::myprop() const
{
    return d->myprop;
}

/*!
    Sets [what?] to \a myprop.
*/
void '.$reading.'::setMyprop(qreal myprop)
{
    d->myprop = myprop;
}

// =====================================================================

// begin generated code
// end generated code

#include "moc_'.$source.'"
QTM_END_NAMESPACE
';
    close OUT;
}

if (-e $source) {
    open IN, "$source" or die $!;
    my @data = <IN>;
    close IN;
    my @tags = grep /^\/\/ (begin|end) generated code/, @data;
    if (scalar(@tags) == 2 &&
    $tags[0] eq "// begin generated code\n" &&
    $tags[1] eq "// end generated code\n") {
        print "Updating generated code in $source\n";
        open OUT, ">$source" or die $!;
        my $skip = 0;
        for (@data) {
            if ($_ eq $tags[0]) {
                print OUT $_;
                print OUT '
/*!
    \class '.$filter.'
    \ingroup sensors_helpers

    \preliminary
    \brief The '.$filter.' class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to '.$reading.'
    instead of QSensorReading.
*/

/*!
    \fn '.$filter.'::filter('.$reading.' *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *'.$sensor.'::type("'.$sensor.'");

/*!
    \class '.$sensor.'
    \ingroup sensors_helpers

    \preliminary
    \brief The '.$sensor.' class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a '.$reading.' instead of a QSensorReading.

    For details about how the sensor works, see \l '.$reading.'.

    \sa '.$reading.'
*/

/*!
    \fn '.$sensor.'::'.$sensor.'(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn '.$sensor.'::~'.$sensor.'()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn '.$sensor.'::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/
';
                $skip = 1;
            }
            if ($_ eq $tags[1]) {
                $skip = 0;
            }
            if (!$skip) {
                print OUT $_;
            }
        }
    }
}


exit 0;


sub get_arg
{
    if (scalar(@ARGV) == 0) {
        croak "Missing arg(s)";
    }
    return shift(@ARGV);
}

