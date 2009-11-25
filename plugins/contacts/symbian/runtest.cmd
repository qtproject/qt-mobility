call cd ..
call del MON.sym
call del MON.dat
call del profile.txt

call qmake
call bldmake bldfiles
call abld clean
call abld reallyclean
call ctcwrap -i d -C "EXCLUDE+*\moc*.cpp" -C "EXCLUDE+symbian\src\cntsymbianfilterdbms.cpp" -C "EXCLUDE+symbian\src\cntsymbiansorterdbms.cpp" -C "EXCLUDE+symbian\src\cnttransformanniversarysimple.cpp" -C "EXCLUDE+symbian\src\cnttransformavatarsimple.cpp" abld build winscw udeb

call \epoc32\release\winscw\udeb\ut_transformcontactdata.exe -xml -o c:\testresults_transformcontactdata.xml
call \epoc32\release\winscw\udeb\ut_cntfiltering.exe -xml -o c:\testresults_cntfiltering.xml
call \epoc32\release\winscw\udeb\ut_cntsymbiandatabase.exe -xml -o c:\testresults_symbiandatabase.xml
call \epoc32\release\winscw\udeb\ut_cntsymbianengine.exe -xml -o c:\testresults_symbianengine.xml
call \epoc32\release\winscw\udeb\ut_cntrelationship.exe -xml -o c:\testresults_relationship.xml

call ctcpost MON.sym MON.dat -p profile.txt
call ctc2html -i profile.txt -nsb
call cd symbian