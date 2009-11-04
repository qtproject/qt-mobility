set QT=%1

set CETEST=%QT%\bin\cetest.exe
set CETEST_ARGS=-cache %QT%\.qmake.cache -libpath \Windows -f

%CETEST% %CETEST_ARGS% tests\auto\qvaluespace\qvaluespace.pro
%CETEST% %CETEST_ARGS% tests\auto\qvaluespaceprovider\qvaluespaceprovider.pro
%CETEST% %CETEST_ARGS% tests\auto\qvaluespacesubscriber\tst_qvaluespacesubscriber\tst_qvaluespacesubscriber.pro
%CETEST% %CETEST_ARGS% tests\auto\qvaluespacesubscriber\tst_qvaluespacesubscriber_oop\tst_qvaluespacesubscriber_oop.pro

