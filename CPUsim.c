#include <stdio.h>     /* Standard Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <string.h>    /* Strings */

int count_threads = 0;                  /* counter for thread count */

int processes;                          /* Total number of processes */
int time_thread_s, time_thread_d;       /* context Switch time for same process diff thread and diff process  */
int heapSize = 0;           

int contex_switch = 0;                  /* Total context switch time */
int total_time = 0;                     /* Time instant at which we are while executing bursts */
int time_in_cpu = 0;

typedef struct CPU_EXECUTIONS
{
    int sequence;
    int cpu_time;
    int io_time;
} cpu_executions;

typedef struct THREAD
{
    int process;
    int thread_id;
    int arrival;
    int termination;
    int no_cpu_executions;
    int current_burst;      /* iterator for the burst array for a thread */
    cpu_executions *burst;
} thread;

typedef struct BASIC_INFO
{
    int service_time;
    int total_io_time;
} basic_thread_info;

typedef struct STATE
{
    int time;
    int process;
    int thread;
    char *initial_stage;
    char *final_stage;
    char *term_stage;
} state;

/*Initialize Heap*/
void initialize_heap(thread *heap)
{
    heapSize = 0;

    heap[0].arrival = -1000000;
    heap[0].termination = -1;
    heap[0].process = -1000000;
    heap[0].no_cpu_executions = -1000000;
    heap[0].thread_id = -1000000;
    heap[0].current_burst = -1000000;
    heap[0].burst = NULL;
}

/*Insert an element into the heap */
void Insert(thread *heap, thread element)
{
    heapSize += 1;

    /* added element to last */
    heap[heapSize].arrival = element.arrival;
    heap[heapSize].termination = element.termination;
    heap[heapSize].process = element.process;
    heap[heapSize].no_cpu_executions = element.no_cpu_executions;
    heap[heapSize].thread_id = element.thread_id;
    heap[heapSize].current_burst = element.current_burst;
    heap[heapSize].burst = element.burst;

    /*Adjust its position*/
    int current = heapSize;
    while ((current > 1) && (heap[current / 2].arrival > element.arrival))
    {
        if (heap[current / 2].arrival > element.arrival)
        {
            heap[current].arrival = heap[current / 2].arrival;
            heap[current].termination = heap[current / 2].termination;
            heap[current].process = heap[current / 2].process;
            heap[current].no_cpu_executions = heap[current / 2].no_cpu_executions;
            heap[current].thread_id = heap[current / 2].thread_id;
            heap[current].current_burst = heap[current / 2].current_burst;
            heap[current].burst = heap[current / 2].burst;

            current /= 2;
            continue;
        }
    }
    heap[current].arrival = element.arrival;
    heap[current].termination = element.termination;
    heap[current].process = element.process;
    heap[current].no_cpu_executions = element.no_cpu_executions;
    heap[current].thread_id = element.thread_id;
    heap[current].current_burst = element.current_burst;
    heap[current].burst = element.burst;
}

/* DONT FORGET TO COMPLETE DEALLOCATE MEMORY FOR THREAD */
thread del_minheap(thread *heap)
{
    thread min_element, last_element;
    int child, current;

    /* minimum element will be the root */
    min_element.arrival = heap[1].arrival;
    min_element.termination = heap[1].termination;
    min_element.process = heap[1].process;
    min_element.no_cpu_executions = heap[1].no_cpu_executions;
    min_element.thread_id = heap[1].thread_id;
    min_element.current_burst = heap[1].current_burst;
    min_element.burst = heap[1].burst;

    last_element.termination = heap[heapSize].termination;
    last_element.arrival = heap[heapSize].arrival;
    last_element.process = heap[heapSize].process;
    last_element.no_cpu_executions = heap[heapSize].no_cpu_executions;
    last_element.thread_id = heap[heapSize].thread_id;
    last_element.current_burst = heap[heapSize].current_burst;
    last_element.burst = heap[heapSize].burst;

    heapSize -= 1;

    for (current = 1; current * 2 <= heapSize; current = child)
    {
        child = current * 2;

        if ((child != heapSize) && (heap[child + 1].arrival < heap[child].arrival))
        {
            if (heap[child + 1].arrival < heap[child].arrival)
                child++;
            else if (heap[child + 1].arrival == heap[child].arrival)
            {
                if (heap[child + 1].process < heap[child].process)
                {
                    child++;
                }
            }
        }
        if (last_element.arrival >= heap[child].arrival)
        {
            if (last_element.arrival > heap[child].arrival)
            {
                heap[current].arrival = heap[child].arrival;
                heap[current].termination = heap[child].termination;
                heap[current].process = heap[child].process;
                heap[current].no_cpu_executions = heap[child].no_cpu_executions;
                heap[current].thread_id = heap[child].thread_id;
                heap[current].current_burst = heap[child].current_burst;
                heap[current].burst = heap[child].burst;
            }
            else if (last_element.arrival == heap[child].arrival)
            {
                if (last_element.process > heap[child].process)
                {
                    heap[current].arrival = heap[child].arrival;
                    heap[current].termination = heap[child].termination;
                    heap[current].process = heap[child].process;
                    heap[current].no_cpu_executions = heap[child].no_cpu_executions;
                    heap[current].thread_id = heap[child].thread_id;
                    heap[current].current_burst = heap[child].current_burst;
                    heap[current].burst = heap[child].burst;
                }
            }
        }
        else
        {
            break;
        }
    }

    heap[current].arrival = last_element.arrival;
    heap[current].termination = last_element.termination;
    heap[current].process = last_element.process;
    heap[current].no_cpu_executions = last_element.no_cpu_executions;
    heap[current].thread_id = last_element.thread_id;
    heap[current].current_burst = last_element.current_burst;
    heap[current].burst = last_element.burst;

    return min_element;    
}

// to print detailed information about the thread
void print_thread_array(thread *a, basic_thread_info *info)
{
    for (int i = 0; i < count_threads; i++)
    {
        printf("Thread %d Process %d:\n", a[i].thread_id, a[i].process);
        printf("\tarrival: %d\n", a[i].arrival);
        printf("\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", info[i].service_time, info[i].total_io_time, a[i].termination - a[i].arrival, a[i].termination);
    }
}

// Merging of two subarrays in sorted order
void merge(state a[], int l, int mid, int h)
{
    int i, j, k;
    i = k = l;
    j = mid;

    state b[h + 1];      // auxiliary array for performing merging
    while ((i <= mid - 1) && (j <= h))
    {
        if (a[i].time <= a[j].time)
        {
            b[k].process = a[i].process;
            b[k].thread = a[i].thread;
            b[k].time = a[i].time;
            b[k].initial_stage = a[i].initial_stage;
            b[k].final_stage = a[i].final_stage;
            b[k++].term_stage = a[i++].term_stage;
        }
        else
        {
            b[k].process = a[j].process;
            b[k].thread = a[j].thread;
            b[k].time = a[j].time;
            b[k].initial_stage = a[j].initial_stage;
            b[k].final_stage = a[j].final_stage;
            b[k++].term_stage = a[j++].term_stage;
        }
    }
    while (i <= mid - 1)
    {
        b[k].process = a[i].process;
        b[k].thread = a[i].thread;
        b[k].time = a[i].time;
        b[k].initial_stage = a[i].initial_stage;
        b[k].final_stage = a[i].final_stage;
        b[k++].term_stage = a[i++].term_stage;
    }
    while (j <= h)
    {
        b[k].process = a[j].process;
        b[k].thread = a[j].thread;
        b[k].time = a[j].time;
        b[k].initial_stage = a[j].initial_stage;
        b[k].final_stage = a[j].final_stage;
        b[k++].term_stage = a[j++].term_stage;
    }

    for (i = l; i <= h; i++)
    {
        a[i].process = b[i].process;
        a[i].thread = b[i].thread;
        a[i].time = b[i].time;
        a[i].initial_stage = b[i].initial_stage;
        a[i].final_stage = b[i].final_stage;
        a[i].term_stage = b[i].term_stage;
    }
}

// recursive merge sort for the state array for verbose mode
void merge_sort(state a[], int n, int l, int r)
{
    int mid = (l + r) / 2;
    if (l < r)
    {
        merge_sort(a, n, l, mid);
        merge_sort(a, n, mid + 1, r);
        merge(a, l, mid + 1, r);
    }
}

int main(int argc, char *argv[])
{
    /* INPUT SECTION */

    scanf("%d", &processes);
    scanf("%d", &time_thread_s);
    scanf("%d", &time_thread_d);

    int thread_for_process[processes + 1][2];
    memset(thread_for_process, 0, sizeof(thread_for_process));

    int process_entry[processes + 1];

    thread *thread_array = (thread *)malloc(100000 * sizeof(thread));
    basic_thread_info *t_info = (basic_thread_info *)malloc(100000 * sizeof(basic_thread_info));

    for (int i = 0; i < processes; i++)
    {
        int process_num, number_of_threads;
        scanf("%d", &process_num);
        scanf("%d", &number_of_threads);

        thread_for_process[process_num][0] = number_of_threads;

        long min_entry = 10000000001;
        for (int j = 0; j < number_of_threads; j++)
        {
            int thread_num, arrival_time, cpu_execution;

            scanf("%d", &thread_num);
            scanf("%d", &arrival_time);
            scanf("%d", &cpu_execution);

            if (arrival_time < min_entry)
            {
                min_entry = arrival_time;
            }

            thread_array[count_threads].process = process_num;
            thread_array[count_threads].thread_id = thread_num;
            thread_array[count_threads].current_burst = 0;
            thread_array[count_threads].arrival = arrival_time;
            thread_array[count_threads].termination = -1;
            thread_array[count_threads].no_cpu_executions = cpu_execution;
            thread_array[count_threads].burst = (cpu_executions *)malloc((cpu_execution) * sizeof(cpu_executions));

            t_info[count_threads].service_time = 0;
            t_info[count_threads].total_io_time = 0;

            for (int k = 0; k < cpu_execution; k++)
            {
                int sub_thread_num, cpu_time, io_time;
                if (k != cpu_execution - 1)
                {
                    scanf("%d", &sub_thread_num);
                    scanf("%d", &cpu_time);
                    scanf("%d", &io_time);

                    thread_array[count_threads].burst[k].sequence = sub_thread_num;
                    thread_array[count_threads].burst[k].cpu_time = cpu_time;
                    thread_array[count_threads].burst[k].io_time = io_time;

                    t_info[count_threads].service_time += cpu_time;
                    t_info[count_threads].total_io_time += io_time;

                    time_in_cpu += cpu_time;
                }
                else
                {
                    scanf("%d", &sub_thread_num);
                    scanf("%d", &cpu_time);

                    thread_array[count_threads].burst[k].sequence = sub_thread_num;
                    thread_array[count_threads].burst[k].cpu_time = cpu_time;
                    thread_array[count_threads].burst[k].io_time = 0;

                    t_info[count_threads].service_time += cpu_time;

                    time_in_cpu += cpu_time;
                }
            }

            process_entry[process_num] = min_entry;
            count_threads += 1;
        }
    }

    fflush(stdin);
    /* INPUT SECTION FINISHED */

    /* HEAP CREATED */

    thread thread_heap[count_threads + 5];

    initialize_heap(thread_heap);

    for (int i = 0; i < count_threads; i++)
    {
        Insert(thread_heap, thread_array[i]);
    }

    /* CHECKING WHICH OF THE 3 FLAGS ARE PRESENT */

    int round_robin = 0, detailed = 0, verbose = 0;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-r") == 0)
        {
            round_robin = 1;
        }
        if (strcmp(argv[i], "-d") == 0)
        {
            detailed = 1;
        }
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }
    }

    /* FIRST COME FIRST SERVE ALGORITHM */

    if (round_robin == 0)
    {
        int last_process = 0, last_thread = 0;
        int x = 0;

        state *thread_states = (state *)malloc(100000 * sizeof(state));
        int st = 0;     /* Counter for counting the no. of state changes */

        /* new to ready state change added to state change array */
        for (int i = 0; i < count_threads; i++)
        {
            thread_states[st].process = thread_array[i].process;
            thread_states[st].thread = thread_array[i].thread_id;
            thread_states[st].time = thread_array[i].arrival;
            thread_states[st].initial_stage = (char *)malloc(100);
            thread_states[st].final_stage = (char *)malloc(100);
            thread_states[st].term_stage = NULL;
            strcpy(thread_states[st].initial_stage, "new");
            strcpy(thread_states[st].final_stage, "ready");
            st += 1;
        }

        /* Performing burst and other state changes till heap is not empty */ 
        while (heapSize != 0)
        {
            thread now = del_minheap(thread_heap);

            /* if the time of arrival of the thread is more than total time means cpu has been idle for that time and hence added to total time */
            if (total_time < now.arrival)
            {
                total_time += now.arrival - total_time;
            }

            /* checking which context switch time applicable */
            
            if ((last_process) && (last_thread) && (last_process != now.process))      /* diff processes */
            {
                contex_switch += time_thread_d;
                total_time += time_thread_d;
            }
            else if ((last_process) && (last_thread) && ((last_process == now.process) && (last_thread != now.thread_id)))  /* same process diff threads */
            {
                contex_switch += time_thread_s;
                total_time += time_thread_s;
            }

            /* ready to running state change added to state change array */
            thread_states[st].process = now.process;
            thread_states[st].thread = now.thread_id;
            thread_states[st].time = total_time;
            thread_states[st].term_stage = NULL;
            thread_states[st].initial_stage = (char *)malloc(100);
            thread_states[st].final_stage = (char *)malloc(100);
            strcpy(thread_states[st].initial_stage, "ready");
            strcpy(thread_states[st].final_stage, "running");
            st += 1;

            total_time += now.burst[now.current_burst].cpu_time;

            /* if not last burst of thread then blocked state achieved from running*/
            if (now.current_burst < now.no_cpu_executions - 1 && now.burst[now.current_burst].io_time > 0)
            {
                thread_states[st].process = now.process;
                thread_states[st].thread = now.thread_id;
                thread_states[st].time = total_time;
                thread_states[st].term_stage = NULL;
                thread_states[st].initial_stage = (char *)malloc(100);
                thread_states[st].final_stage = (char *)malloc(100);
                strcpy(thread_states[st].initial_stage, "running");
                strcpy(thread_states[st].final_stage, "blocked");
                st += 1;
            }
            else if (now.current_burst < now.no_cpu_executions - 1 && now.burst[now.current_burst].io_time == 0)
            {
                /* If I/O time of thread is 0 other than last thread then it goes to running to ready stae directly */
                thread_states[st].process = now.process;
                thread_states[st].thread = now.thread_id;
                thread_states[st].time = total_time;
                thread_states[st].term_stage = NULL;
                thread_states[st].initial_stage = (char *)malloc(100);
                thread_states[st].final_stage = (char *)malloc(100);
                strcpy(thread_states[st].initial_stage, "running");
                strcpy(thread_states[st].final_stage, "ready");
                st += 1;
            }
            /* setting up next arrival of thread */
            now.arrival = total_time + now.burst[now.current_burst].io_time;

            /* blocked to ready state change added to state change array at next arrival*/
            if (now.current_burst < now.no_cpu_executions - 1 && now.burst[now.current_burst].io_time > 0)
            {
                thread_states[st].process = now.process;
                thread_states[st].thread = now.thread_id;
                thread_states[st].time = now.arrival;
                thread_states[st].term_stage = NULL;
                thread_states[st].initial_stage = (char *)malloc(100);
                thread_states[st].final_stage = (char *)malloc(100);
                strcpy(thread_states[st].initial_stage, "blocked");
                strcpy(thread_states[st].final_stage, "ready");
                st += 1;
            }

            /* current burst executed moving to next one */
            now.current_burst += 1;

            /* LAST process and thread UPDATED */
            last_process = now.process;
            last_thread = now.thread_id;

            // IF ALL BURST OVER THEN DON'T INSERT
            if (now.current_burst < now.no_cpu_executions)
            {
                Insert(thread_heap, now);
            }
            else
            {
                // find process and thread from thread array
                // termination time and update to original thread array O(n) traversal

                int i = 0;
                for (i = 0; i < count_threads; i++)
                {
                    if (now.process == thread_array[i].process && now.thread_id == thread_array[i].thread_id)
                    {
                        thread_array[i].termination = total_time;
                        break;
                    }
                }

                if (thread_for_process[now.process][0] - 1 != 0)
                {
                    thread_for_process[now.process][0] -= 1;
                }
                else
                {
                    thread_for_process[now.process][1] = total_time;
                }

                /* running to terminated state change added to state change array */
                thread_states[st].process = now.process;
                thread_states[st].thread = now.thread_id;
                thread_states[st].time = total_time;
                thread_states[st].term_stage = (char *)malloc(100000);
                thread_states[st].initial_stage = (char *)malloc(100);
                thread_states[st].final_stage = (char *)malloc(100);
                strcpy(thread_states[st].initial_stage, "running");
                strcpy(thread_states[st].final_stage, "terminated");
                strcpy(thread_states[st].term_stage, "");

                char *temp = (char *)malloc(100000);
                sprintf(temp, "Thread %d Process %d:\n", now.thread_id, now.process);
                strcat(thread_states[st].term_stage, temp);
                sprintf(temp, "\tarrival: %d\n", thread_array[i].arrival);
                strcat(thread_states[st].term_stage, temp);
                sprintf(temp, "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", t_info[i].service_time, t_info[i].total_io_time, total_time - thread_array[i].arrival, total_time);
                strcat(thread_states[st].term_stage, temp);

                st += 1;

                free(temp);
            }
        }

        for (int i = 1; i < processes + 1; i++)
        {
            x += thread_for_process[i][1] - process_entry[i];
        }

        /* FOR PROCESSES */
        double avg_turnaround = ((double)(x)) / ((double)(processes));

        double cpu_utils = (((double)time_in_cpu) / ((double)total_time));
        char percent = '%';

        printf("FCFS Scheduling\n");
        printf("Total Time required is %d time units\n", total_time);
        printf("Average Turnaround Time is %.1f time units\n", avg_turnaround);
        printf("CPU Utilization is %.1f%c\n", cpu_utils * 100, percent);

        if (detailed)
            print_thread_array(thread_array, t_info);

        if (verbose)
        {
            // STABLE SORTING by MERGE SORT ..

            merge_sort(thread_states, st, 0, st - 1);

            for (int i = 0; i < st; i++)
            {
                printf("At time %d: Thread %d of Process %d moves from %s to %s\n", thread_states[i].time, thread_states[i].thread, thread_states[i].process, thread_states[i].initial_stage, thread_states[i].final_stage);
                if (thread_states[i].term_stage != NULL)
                    printf("%s", thread_states[i].term_stage);
            }
        }

        /* Freeing memory for states array */
        for (int i = 0; i < st; i++)
        {
            free(thread_states[i].initial_stage);
            free(thread_states[i].final_stage);
            free(thread_states[i].term_stage);
        }
        free(thread_states);
    }

    /* ROUND ROBIN SCHEDULING ALGORITHM */
    else
    {
        char *qstring = argv[argc - 1];

        int quantum = atoi(qstring);        
        /* reading the quantum from command line and changing it to integer */

        int last_process = 0, last_thread = 0;
        int x = 0;

        state *thread_states = (state *)malloc(100000 * sizeof(state));
        int st = 0;

        /* new to ready state change added to state change array for all threads*/
        for (int i = 0; i < count_threads; i++)
        {
            thread_states[st].process = thread_array[i].process;
            thread_states[st].thread = thread_array[i].thread_id;
            thread_states[st].time = thread_array[i].arrival;
            thread_states[st].initial_stage = (char *)malloc(100);
            thread_states[st].final_stage = (char *)malloc(100);
            thread_states[st].term_stage = NULL;
            strcpy(thread_states[st].initial_stage, "new");
            strcpy(thread_states[st].final_stage, "ready");
            st += 1;
        }

        /* Performing burst and other state changes till heap is not empty for specific quantums*/ 
        while (heapSize != 0)
        {
            thread now = del_minheap(thread_heap);

            /* if the time of arrival of the thread is more than total time means cpu has been idle for that time and hence added to total time */
            if (total_time < now.arrival)
            {
                total_time += now.arrival - total_time;
            }

            /* checking which context switch time applicable */

            if ((last_process) && (last_thread) && (last_process != now.process))
            {
                contex_switch += time_thread_d;
                total_time += time_thread_d;
            }
            else if ((last_process) && (last_thread) && ((last_process == now.process) && (last_thread != now.thread_id)))
            {
                contex_switch += time_thread_s;
                total_time += time_thread_s;
            }

            /* ready to running */

            thread_states[st].process = now.process;
            thread_states[st].thread = now.thread_id;
            thread_states[st].time = total_time;
            thread_states[st].term_stage = NULL;
            thread_states[st].initial_stage = (char *)malloc(100);
            thread_states[st].final_stage = (char *)malloc(100);
            strcpy(thread_states[st].initial_stage, "ready");
            strcpy(thread_states[st].final_stage, "running");
            st += 1;

            /* If current burst time is less than Quantum then it will be executed fully and going to blocked state if present*/
            if (now.burst[now.current_burst].cpu_time <= quantum)
            {
                total_time += now.burst[now.current_burst].cpu_time;
                now.burst[now.current_burst].cpu_time = 0;

                if (now.current_burst != now.no_cpu_executions - 1 && now.burst[now.current_burst].io_time > 0)
                {
                    thread_states[st].process = now.process;
                    thread_states[st].thread = now.thread_id;
                    thread_states[st].time = total_time;
                    thread_states[st].term_stage = NULL;
                    thread_states[st].initial_stage = (char *)malloc(100);
                    thread_states[st].final_stage = (char *)malloc(100);
                    strcpy(thread_states[st].initial_stage, "running");
                    strcpy(thread_states[st].final_stage, "blocked");
                    st += 1;
                }
                else if (now.current_burst != now.no_cpu_executions - 1 && now.burst[now.current_burst].io_time == 0)
                {
                    /* If I/O time of thread is 0 other than last thread then it goes to running to ready stae directly */
                    thread_states[st].process = now.process;
                    thread_states[st].thread = now.thread_id;
                    thread_states[st].time = total_time;
                    thread_states[st].term_stage = NULL;
                    thread_states[st].initial_stage = (char *)malloc(100);
                    thread_states[st].final_stage = (char *)malloc(100);
                    strcpy(thread_states[st].initial_stage, "running");
                    strcpy(thread_states[st].final_stage, "ready");
                    st += 1;
                }

                // updating next arrival
                now.arrival = total_time + now.burst[now.current_burst].io_time;

                if (now.current_burst != now.no_cpu_executions - 1 && now.burst[now.current_burst].io_time != 0)
                {
                    thread_states[st].process = now.process;
                    thread_states[st].thread = now.thread_id;
                    thread_states[st].time = now.arrival;
                    thread_states[st].term_stage = NULL;
                    thread_states[st].initial_stage = (char *)malloc(100);
                    thread_states[st].final_stage = (char *)malloc(100);
                    strcpy(thread_states[st].initial_stage, "blocked");
                    strcpy(thread_states[st].final_stage, "ready");
                    st += 1;
                }
                now.current_burst += 1;
            }  
            else                
            {
                /* If current burst time is more than quantum then execute it for quantum time and update its cputime to current_cputime - quantum */
                total_time += quantum;
                /* Next Arrival would be same as this instant */
                now.arrival = total_time;

                thread_states[st].process = now.process;
                thread_states[st].thread = now.thread_id;
                thread_states[st].time = total_time;
                thread_states[st].term_stage = NULL;
                thread_states[st].initial_stage = (char *)malloc(100);
                thread_states[st].final_stage = (char *)malloc(100);
                strcpy(thread_states[st].initial_stage, "running");
                strcpy(thread_states[st].final_stage, "ready");
                st += 1;

                now.burst[now.current_burst].cpu_time -= quantum;
            }

            // LAST UPDATED
            last_process = now.process;
            last_thread = now.thread_id;

            /* if last thread and cputime also 0 then terminate */
            if (now.current_burst == now.no_cpu_executions && now.burst[now.current_burst - 1].cpu_time == 0)
            {
                // find process and thread from thread array
                // termination time and update to original thread array O(n) traversal

                int i = 0;
                for (i = 0; i < count_threads; i++)
                {
                    if (now.process == thread_array[i].process && now.thread_id == thread_array[i].thread_id)
                    {
                        thread_array[i].termination = total_time;
                        break;
                    }
                }

                if (thread_for_process[now.process][0] - 1 != 0)
                {
                    thread_for_process[now.process][0] -= 1;
                }
                else
                {
                    thread_for_process[now.process][1] = total_time;
                }

                thread_states[st].process = now.process;
                thread_states[st].thread = now.thread_id;
                thread_states[st].time = total_time;
                thread_states[st].term_stage = (char *)malloc(100000);
                thread_states[st].initial_stage = (char *)malloc(100);
                thread_states[st].final_stage = (char *)malloc(100);
                strcpy(thread_states[st].initial_stage, "running");
                strcpy(thread_states[st].final_stage, "terminated");
                strcpy(thread_states[st].term_stage, "");

                char *temp = (char *)malloc(100000);
                sprintf(temp, "Thread %d Process %d:\n", now.thread_id, now.process);
                strcat(thread_states[st].term_stage, temp);
                sprintf(temp, "\tarrival: %d\n", thread_array[i].arrival);
                strcat(thread_states[st].term_stage, temp);
                sprintf(temp, "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", t_info[i].service_time, t_info[i].total_io_time, total_time - thread_array[i].arrival, total_time);
                strcat(thread_states[st].term_stage, temp);

                st += 1;

                free(temp);
            }
            else
            {
                Insert(thread_heap, now);
            }
        }

        for (int i = 1; i < processes + 1; i++)
        {
            // printf("%d %d\n",thread_for_process[i][1],process_entry[i]);
            x += thread_for_process[i][1] - process_entry[i];
        }
        /* FOR PROCESSES */
        double avg_turnaround = ((double)(x)) / ((double)(processes));

        double cpu_utils = (((double)time_in_cpu) / ((double)total_time));
        char percent = '%';

        printf("Round Robin Scheduling (quantum = %d time units)\n", quantum);
        printf("Total Time required is %d time units\n", total_time);
        printf("Average Turnaround Time is %.1f time units\n", avg_turnaround);
        printf("CPU Utilization is %.1f%c\n", cpu_utils * 100, percent);

        if (detailed)
            print_thread_array(thread_array, t_info);

        if (verbose)
        {
            // STABLE SORTING by MERGE SORT ..

            merge_sort(thread_states, st, 0, st - 1);

            for (int i = 0; i < st; i++)
            {
                printf("At time %d: Thread %d of Process %d moves from %s to %s\n", thread_states[i].time, thread_states[i].thread, thread_states[i].process, thread_states[i].initial_stage, thread_states[i].final_stage);
                if (thread_states[i].term_stage != NULL)
                    printf("%s", thread_states[i].term_stage);
            }
        }

        for (int i = 0; i < st; i++)
        {
            free(thread_states[i].initial_stage);
            free(thread_states[i].final_stage);
            free(thread_states[i].term_stage);
        }
        free(thread_states);
    }

    /* MEMORY CLEARING SECTION */

    for (int i = 0; i < count_threads; i++)
        free(thread_array[i].burst);

    free(thread_array);
    free(t_info);

    exit(0);
}
