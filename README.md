# Message oriented client-server communication
 
This code implements a simple file upload and download service based on the client-server model. The file server supports four basic operations: UPLOAD, DOWNLOAD, DELETE, and RENAME.

# Instruction about how to run the program
---
To compile the programs, you should enter into each folder and use `make` command.

1. run the server:
```bash
./server
```

2. run the client:
```bash
./client SERVER_IP MESSAGE_TYPE
```

There are four types of message:
- upload
- downld
- rename
- delete

For example, you can use `./client 0.0.0.0 upload` to upload a file. After connection is made, there will be a notification then you can type the name of the file you want to upload.


