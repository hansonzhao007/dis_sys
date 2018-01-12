# Totally ordered multicasting service
 
This is an n-node distributed system that provides a totally ordered multicasting service and a distributed locking scheme. The distributed system uses logical clock to timestamp messages sent/received between nodes. To start the distributed system, each node synchronize their logical clocks to the same initial value, based on which the ordering of events can be determined among the machines.

The sample code uses Berkeley Algorithm to obtain an initial agreement on the logical clocks. Totally ordered multicasting is  implemented using Lamport’s algorithm.

# Instruction about how to run the program
To compile the programs, you should enter into "server_side" folder and use `make` command.

To run the program:
```bash
./main [node_number]
```
`node_number`: This is used to set number of process. The maximum number can be set to 10.

For example, "./main 5" will create 5 process to simulate totally orderd multicasting

# Result
The simulation will be stored in "results" folder.

You can see file name like following:
- `Node0_Partial_Order.txt`: This file stores node0's partially order issue messages.
- `Node0_Total_Order.txt`: This files stores node0's totally order issue messages.

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


