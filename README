# Project1
Name: Xingsheng Zhao
Student ID: -

# Instruction about how to run the program
---
To compile the programs, you should enter into each folder and use `make` command.

To run the program:

1. Start the server
./server

2. Run the client
./client SERVER_IP MESSAGE_TYPE

There are four types of message:
- upload
- downld
- rename
- delete

For example, you can use `./client 0.0.0.0 upload` to upload a file. After connection is made, there will be a notification then you can type the name of the file you want to upload.


# How you implemented the programs
Basically, I implements a class:`FileServerUtil` to do all the works, including decoding the message, delete operation, create operation, and etc.

I learned how to write makefile to make compile process more easier, and how to utilize multi_thread programming.

If many clients want to do write operations on the same file at the same time, it may cause collections and damage the data. That's why locking is necessary for multi-threaded server. It makes sure at any time there will be and only be one client that can do write operations on a file.




