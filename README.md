# Project2
Name: Xingsheng Zhao
Student ID: -

# Instruction about how to run the program
---
To compile the programs, you should enter into "server_side" folder and use `make` command.

To run the program:

1. Start testing
./main [node_number]

node_number: This is used to set number of process.

For example, "./main 5" will create 5 process to simulate totally orderd multicasting

# How you implemented the programs
I implements several libs:

lc_msg_api.c, lc_msg_api.h: This lib implements some basic data structures and some operation related to message.

lc_util.c, lc_util.h: This lib implements socket recieve service and Lamport's logical clock algorithm.

# Result
The simulation will be stored in "results" folder.

You can see file name like following:
- "Node0_Partial_Order.txt": Here store node0's partially order issue messages.
- "Node0_Total_Order.txt": Here store node0's totally order issue messages.

# Notice
There are some bugs in this project, some times it may have abnormal termination.

```shell
 Error : Node 4 connect Node 5 Failed 
 Error : Node 2 connect Node 5 Failed 
Process 28964 terminated
normal termination, exit status = 0
Process 28960 terminated
normal termination, exit status = 0
 Error : Node 1 connect Node 6 Failed 
 Error : Node 0 connect Node 5 Failed 
 Error : Node 3 connect Node 5 Failed 
Process 28959 terminated
normal termination, exit status = 0
...

```

When this happens, you could restart the program.


