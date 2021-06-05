# socket_registration_program
college's model's last question solution

Note: It is winsock impl so it requires ws2_32.dll so it is included in cmakelist.txt so use cmake to compile cpp files into executables

run "server.exe –p port" command on terminal
e.g run "server.exe - p 8080"

run "client.exe –h serveraddress -p port -n IDNumber" command on terminal
e.g run "client.exe –h 127.0.0.1 -p 8080 -n 1023"

Note: First run server then you can create at most 10 simultaneous client threads(question's condition)

