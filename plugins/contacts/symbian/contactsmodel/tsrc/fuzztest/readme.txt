The fuzz test is used to send a range of requests/messages to target server.
It takes 3 parameters from command line/shell:
1. target server name etc cntsrv for contact server
2. minium range of the IPC code
3. maxium range of the IPC code

The example is like:
Fuzztest cntsrv 0 300 -- this will invoke fuzz test to send IPC request from 0 to 300.