#ifndef RPCBANKSERVERHEADER_H
#define RPCBANKSERVERHEADER_H

//this is my "protocol"
//i get the size of the message and the type, then i use my dispenser for that type and get the type. i then pass off delegating the rest of the message (i am only the header) to the type itself via it's datastream operator overloads

#endif // RPCBANKSERVERHEADER_H
