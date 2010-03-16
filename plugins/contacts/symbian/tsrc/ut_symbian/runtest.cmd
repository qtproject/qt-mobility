call del MON.sym
call del MON.dat
call del profile.txt

call qmake
call bldmake bldfiles
call abld clean
call abld reallyclean
call ctcwrap -i d -C "EXCLUDE+*\moc*.cpp" -C "EXCLUDE+..\..\src\transform\cnttransformanniversarysimple.cpp" -C "EXCLUDE+..\..\src\transform\cnttransformavatarsimple.cpp" -C "EXCLUDE+..\..\src\transform\cnttransformempty.cpp" -C "EXCLUDE+..\..\src\transform\cntthumbnailcreator.cpp" -C "EXCLUDE+..\..\src\filtering\cntsymbianfilterdbms.cpp" -C "EXCLUDE+.\*.*" abld build winscw udeb

call \epoc32\release\winscw\udeb\ut_symbian.exe
call ctcpost MON.sym MON.dat -p profile.txt
call ctc2html -i profile.txt -nsb
