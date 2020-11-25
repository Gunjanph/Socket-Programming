# Socket Programming 
The client will create connection and send requests to download file from server. Progress will also be printed.


## Running the code

Server

```bash
cd server
gcc server.c -o server
./server
```
Client

```bash
cd client
gcc client.c -o client
./client
```

## Commands
These commands need to be run in client. If any command other than these are given then a messege will be printed.

### get
- get `<filename>`
- get `<file1>` `<file2>` `<file3>`
- It will give error if no file is given.
- It can also handle multiple file requests.
### exit
- connection between the client and server is lost and both the codes exit.

## Requirements
- File greater than 1 GB can be downloaded within 1 sec.
- Sockets are used.
- No external library is used.
- All the errors are handled.
