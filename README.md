## Cloud Drive
Cloud drive server and client to store files remotely, implemented using C/C++.

## Command for Client
Command | Function
:-:|-
PUT&nbsp;<filename_1>&nbsp;<filename_2> | Uplaod local file <filename_1> and rename it to <filename_2> in cloud drive.
GET&nbsp;<filename_1>&nbsp;<filename_2> | Download <filename_1> and rename it to <filename_2> in local drive.
LIST | Show the list of all files in cloud drive.
EXIT | Close connection to cloud drive server and terminate.
## How to Run
Compile server/client source code.
```
$ g++ -o server server.cpp
$ g++ -o client client.cpp
```

Start server on TCP port <port_number>.
```
$ ./server <port_number>
```

Start client and connect to server with IP <IP_addr> and TCP port <port_number>.
```
$ ./client <IP_addr> <port_number>
```
