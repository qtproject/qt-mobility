#!/bin/sh
#
# Maemo unit tests runner. This is executed by CI machinery when test package is integrated.
#

#set -x

PACKAGE=libbearer-test
TEST_DATA_DIR=/usr/share
TEST_BIN_DIR=/usr/lib
TESTS="tst_qnetworkconfiguration tst_qnetworkconfigurationmanager tst_qnetworksession"

mkdir -p "/var/log/$PACKAGE"

#make a symbolic link to /tmp/$PACKAGE from /var/log/$PACKAGE because ats3 fetches the test run logs from /tmp/$PACKAGE
#/tmp is too small in size to store the actual logs

#If the symbolic link cannot be found create a new one
if [ -z "`readlink "/tmp/$PACKAGE"`" ]; then
    echo "Create a symbolic link from /tmp/$PACKAGE to /var/log/$PACKAGE"
    ln -s "/var/log/$PACKAGE" "/tmp/$PACKAGE"
else
    echo "Symbolic link from /tmp/$PACKAGE to /var/log/$PACKAGE already exists, no need to create new one"
fi

#Run the tests from /var/log/$PACKAGE to store the logs there
cd "/var/log/$PACKAGE"

for test_name in $TESTS; do
    if [ -x $TEST_BIN_DIR/$PACKAGE/$test_name ]; then
	$TEST_BIN_DIR/$PACKAGE/$test_name >> $PACKAGE.log 2>&1
    else
	echo "FAIL: Test $TEST_BIN_DIR/$PACKAGE/$test_name not found."  >> $PACKAGE.log
    fi
done

cd $TEST_DATA_DIR/$PACKAGE
