
# CPU SIMULATION

The Following Program will be Simulating the CPU Scheduling.The goal is to develop a CPU scheduling algorithm that will complete the execution of a group of multi-threaded processes in an OS that understands threads (kernel threads).

## Specifications 

Given a set of processes to execute with CPU and I/O requirements, the CPU simulator will simulate the execution of the threads based on your developed CPU scheduling policies:
<br>

    1. First Come First Serve Algorithm 
    2. Round Robin Algorithm


### Input Format

<br>

The simulator input includes:
- number of processes that require execution
- the thread switch overhead time (i.e., the number of time units required to switch to a
new thread in the SAME process)
- the process switch overhead time (i.e., the number of time units required to switch to a
new thread in a DIFFERENT process)
- for each process
    
    1. the number of threads.
    2. the arrival time for each thread
    3. the number of CPU execution bursts each thread requires (the CPU execution
    bursts are separated by the time it takes for the thread to do I/O)
    4. the CPU time and the I/O time

##### Example Input 

    2 3 7       Number of Processes | Switch Time (Same Process) | Switch Time (Different Process)
    1 2         Process Number | Num Threads
    1 0 3       Thread Number | Arrival Time | Num CPU execution bursts that each thread requires
    1 10 20     Thread Number | CPU Burst Time | I/O Time
    2 10 20
    3 10
    2 5 2
    1 50 10
    2 50
    2 2
    1 0 1
    1 100
    2 50 2
    1 100 20
    2 100

<br> 

### Simulation Outputs

The simulation will collect the following statistics:

- The total time required to execute all the threads in all the processes
- The CPU utilization (NOT the CPU efficiency)
- The average turnaround time for all the processes
- The service time (or CPU time), I/O time and turnaround time for each individual thread.

<br>


##### 1. Simplely Passing Input Will Give output According to First Come First Serve Algorithm.

    $ ./sim < input_file

        FCFS Scheduling
        Total Time required is 344 time units
        Average Turnaround Time is 27.9 time units
        CPU Utilization is 94%

##### 2. Passing Input with command -d Will Give detailed output about each thread according to First Come First Serve Algorithm.

    $ ./sim -d < input_file

    FCFS Scheduling
    Total Time required is 471 time units
    Average Turnaround Time is 417.5 time units
    CPU Utilization is 91.3%
    Thread 1 Process 1:
	arrival: 0
    service time: 30 units, I/O time: 40 units, turnaround time: 364 units, finish time: 364 units
    Thread 2 Process 1:
	arrival: 5
	service time: 100 units, I/O time: 10 units, turnaround time: 346 units, finish time: 351 units
    Thread 1 Process 2:
	arrival: 0
	service time: 100 units, I/O time: 0 units, turnaround time: 117 units, finish time: 117 units
    Thread 2 Process 2:
	arrival: 50
	service time: 200 units, I/O time: 20 units, turnaround time: 421 units, finish time: 471 units

##### 3. Passing Input with command -v will Give detailed output in verbose format like which tread completed how much and transition from state according to First Come First Serve Algorithm.

    $ ./sim -v < input_file

    FCFS Scheduling
    Total Time required is 471 time units
    Average Turnaround Time is 417.5 time units
    CPU Utilization is 91.3%
    At time 0: Thread 1 of Process 1 moves from new to ready
    At time 0: Thread 1 of Process 2 moves from new to ready
    At time 0: Thread 1 of Process 1 moves from ready to running
    At time 5: Thread 2 of Process 1 moves from new to ready
    At time 10: Thread 1 of Process 1 moves from running to blocked
    At time 17: Thread 1 of Process 2 moves from ready to running
    At time 30: Thread 1 of Process 1 moves from blocked to ready
    At time 50: Thread 2 of Process 2 moves from new to ready
    At time 117: Thread 1 of Process 2 moves from running to terminated
    Thread 1 Process 2:
	arrival: 0
	service time: 100 units, I/O time: 0 units, turnaround time: 117 units, finish time: 117 units
    At time 124: Thread 2 of Process 1 moves from ready to running
    At time 174: Thread 2 of Process 1 moves from running to blocked
    At time 177: Thread 1 of Process 1 moves from ready to running
    At time 184: Thread 2 of Process 1 moves from blocked to ready
    At time 187: Thread 1 of Process 1 moves from running to blocked
    At time 194: Thread 2 of Process 2 moves from ready to running
    At time 207: Thread 1 of Process 1 moves from blocked to ready
    At time 294: Thread 2 of Process 2 moves from running to blocked
    At time 301: Thread 2 of Process 1 moves from ready to running
    At time 314: Thread 2 of Process 2 moves from blocked to ready
    At time 351: Thread 2 of Process 1 moves from running to terminated
    Thread 2 Process 1:
	arrival: 5
	service time: 100 units, I/O time: 10 units, turnaround time: 346 units, finish time: 351 units
    At time 354: Thread 1 of Process 1 moves from ready to running
    At time 364: Thread 1 of Process 1 moves from running to terminated
    Thread 1 Process 1:
	arrival: 0
	service time: 30 units, I/O time: 40 units, turnaround time: 364 units, finish time: 364 units
    At time 371: Thread 2 of Process 2 moves from ready to running
    At time 471: Thread 2 of Process 2 moves from running to terminated
    Thread 2 Process 2:
	arrival: 50
	service time: 200 units, I/O time: 20 units, turnaround time: 421 units, finish time: 471 units

##### 4. Passing Input with command -r with the time quantum (Eg. 10 below) along with all 3 above combinations will give results according to Round Robin Algorithm.

    $ ./simcpu -d –r 10 < input_file

    Round Robin Scheduling (quantum = 10 time units)
    Total Time required is 140 units
    Average Turnaround Time is 9.2 time units
    CPU Utilization is 100%
    Thread 1 of Process 1:
    arrival time: 0
    service time: 32 units, I/O time: 3 units, turnaround time: 45 units, finish time: 45 units
    Thread 2 of Process 1:
    arrival time: 5
    service time: 102 units, I/O time: 15 units, turnaround time: 135 units, finish time: 140 units
    Thread 1 of Process 2:
    arrival time: 10
    service time: 22 units, I/O time: 3 units, turnaround time: 45 units, finish time: 55 units

<br>

### Assumptions : 

1. Assuming an infinite number of I/O devices, i.e. threads do not need to wait to do the I/O.
2. No overhead is associated with placing a thread on, or removing a thread from, an I/O device (e.g., no overhead is associated with an "interrupt" to indicate I/O completion)
3. All simulation parameters are integers.


## Description to Simulation and Algorithms

###### Structures Used : 

    struct thread --> 
        int process;               -   Stores Process ID
        int thread_id;             -   Stores Thread ID
        int arrival;               -   Stores Initial Arrival Time for Each Thread
        int termination;           -   Stores the time of termination
        int no_cpu_executions;     -   Stores number of bursts in a particular thread
        int current_burst;         -   Iterator for the burst which is being executed currently for a thread
        cpu_executions *burst;     -   Array of CPU_Execution Structure.

<br>

    struct cpu_executions --> 
        int sequence;              -   Number of the Burst 
        int cpu_time;              -   CPU burst time needed for execution
        int io_time;               -   Input / Output Time needed


<br>

    struct basic_thread_info -->
        int service_time           -   Total CPU time for each thread AKA Service time
        int total_io_time;         -   Total I/O time for a thread

<br>


    struct state -->
    
    [This structure is basically used for the verbose output of the CPU Scheduling.
    It contains the initial and final state in which a thread/process is at a particular time instant
    Used for printing the change of states in Chronological order.]

        int time;                   -   the time instant at which change of this state happened 
        int process;                -   process
        int thread;                 -   thread
        char *initial_stage;        -   initial state of the thread and process
        char *final_stage;          -   final stage of the thread and process
        char *term_stage;           -   If thread terminates then detailed information about that thread is stored


<br>

### Helper Functions

    1. void initialize_heap(thread *heap) 
Initializing the Heap making HeapSize = 0 and setting First element as most minimum. [-INT_MAX]

    2. void Insert(thread *heap, thread element)
Element from thread array in main function will be inserted

**Priority while insertion -** 
*Lower the Arrival Time Higher the Priority.*




Finding the position by upward traversal in heap by checking if parent arrival is more,  then swapping the nodes.

    3. thread del_minheap(thread *heap)
Root will be deleted which will be the minimum element i.e. minimum arrival time Rest of the elements will be adjusted with next minimum element as next root.

**Priority while deletion-** 
*Lower the Arrival Time Higher the Priority.*
else if equal arrival time:
*Lower the Process ID Higher the Priority.*

    

    4. merge(states) and merge_sort(state_array)

**THIS IS IMPORTANT FUNCTION FOR ARRANGING THE STATE CHANGES IN SORTED AS WELL AS CHRONOLOGICAL ORDER. ( for Verbose mode output )**

Since **Merge Sort** is a *STABLE* sorting algorithm, hence it will sort the state-changes with respect to the time of occurence of that transition.
If time occurence of the two state changes are same then it will print the state change in Chronological order i.e that has first occured at that instant also.

###### ** We are not using Heap Sort as it is not a stable sorting algorithm..

## Functionalities 

##### 1. Initializing the Heap - 
Minimum Priority Queue is applied here as we need the thread which has came first into the queue based on arrival time and on basis of processes if arrival is same Priority / Ready Queue of the threads has been initialized.

##### 2. Inserting into the Ready Queue - 
All threads in thread_array has been inserted into the queue.

##### 3. Checking for Flags - 
Checking what has to be outputed depending on the flags that has been entered to command line:

    -r:
        Round Robin Scheduling algorithm
    else:
        First Come First Serve algorithm


###### First Come First Serve algorithm - 

    While HeapSize != 0:
        extract the root 

        if time_instant_now < arrival:
            Add rest idle time in total time
        
        if process different:
            Add context time for different processes change
        if process same and thread different:
            Add context time for different thread change

        total_time += cpu time for this burst
        
        next arrival of this thread += time_now + i/o_time 

        current burst_index += 1

        if thread not terminated:
            insert again to heap
        else:
            if final thread of a process:
                store termination time of the process to calculate average turnaround time for the processes


###### Round Robin Algorithm - 

    While HeapSize != 0:
        extract the root [most prior]

        if time_instant_now < arrival:
            we will add rest idle time in total time
        
        if process different:
            add context time for different processes change
        if process same and thread different:
            add context time for different thread change

        /* ready to running */

        if burst_time > quantum:
            simplify process for quantum time
            update time         
            /* Arrival would be earlier + quantum */
        else:
            total_time += cpu_time

            if not last burst:
                /*running to blocked*/

                arrival += i/o-time

                /* at this arrival time */ 
                /*blocked to ready*/
            
        
        if thread not terminated:
            insert again to heap
        else:
            if final thread of a process:
                store termination time of the process to calculate average turnaround time for the processes
                /*running to terminated*/
        

##### 3. Freeing the Dynamic Memory Allocated at the end.

