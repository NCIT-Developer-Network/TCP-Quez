/*
//========================================================
//This is a TCP quiz where the client can create the question and submit to the server along with the answer. The server stores it into the buffer and send the random question from the buffer to all the client. Timer is set for two minutes to anser the question otherwise answer will be shown to all the clients and draws the new random question.
//========================================================
//Developed by: Kamal Panthi 
//Email: kamal210@hotmail.com
//        
//To Compile type:"gcc -g tcpquizclientserver.c -o tcpquizclientserver" 

//or Use Makefile to compile.
--> type make 


//To run server type: "./tcpquizclientserver portnumber"
//To run client type: "./tcpquizclientserver hostname portnumber nickname"



//======================================================== 

Acknowledgement:
http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/unix-socket-faq.html
http://www.hiraeth.com/alan/tutorials/courses/tcpip.html
http://www.redbooks.ibm.com/Redbooks.nsf/RedbookAbstracts/gg243376.html
http://www.tenouk.com/cnlinuxsockettutorials.html
http://www.cprogramming.com/tutorial/random.html

Stevens, W.R.: Unix Network Programming, The Sockets Networking API, Vol. 1, 3rd Ed., Prentice Hall, 2004.
Internetworking with TCP/IP Vol. 3: Client-Server Programming and Application, Linux/POSIX Socket Version (Comer, D.E., Stevens, D.), 2000. 


//======================================================== 

*/

 
