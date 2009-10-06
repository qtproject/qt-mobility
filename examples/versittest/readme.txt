Following configurations should be made to run the testdriver application.

Test folder 					: \epoc32\winscw\c\data\testvcards in emulator or c:\data\testvcards in hardware
Test files 						: Should be located under testfolder and should have extension .vcf                          
Excluding some fields : File excludefields.txt should be located under testfolder
                        If you want to exclude vCard field "X-MYFIELD" from the comparsion, add line X-MYFIELD to the excludefields.txt.
                        Each field name should be located in a separate line.
                        
Note 								  : Example testfiles are under testvcards folder. 
                        Copy them above mentioned location if you want to use those.
