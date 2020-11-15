## Cloud Drive
Cloud drive server and client to store files remotely, implemented using C/C++.

Client can login by entering client ID to manage files across sessions.

## Command for Client
Command | Function
:-:|-
PUT&nbsp;<filename_1>&nbsp;<filename_2> | Upload local file `<filename_1>` and rename it to `<filename_2>` in cloud drive.
GET&nbsp;<filename_1>&nbsp;<filename_2> | Download `<filename_1>` and rename it to `<filename_2>` in local drive.
LIST | Show the list of all files in cloud drive.
EXIT | Close connection to server and terminate.
## How to Run
Compile server/client source code.
```
$ g++ -o server server.cpp
$ g++ -o client client.cpp
```

Start server on TCP port `<port_number>`.
```
$ ./server <port_number>
```

Start client and connect to server with IPv4 address `<IP_addr>` and TCP port `<port_number>`.
```
$ ./client <IP_addr> <port_number>
```

## Versions
There are multiple versions of cloud drive server/client.

### select_login
- Support login/logout by client ID to manage cloud drive across sessions.

- Support multiple client connections (Concurrent connections of the same ID not allowed).

- Implemented using select().

### select
- Support multiple client connections.

- Implemented using select().

### fork
- Support multiple client connections.

- Implemented using fork().

### iterative
- Support single client connection.

## Note
Client should enter command only after the previous one is already processed by server.

Server stores files in RAM.
