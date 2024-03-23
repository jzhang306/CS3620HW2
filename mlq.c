#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority; // Priority level (higher value indicates higher priority)
    int remaining_time; // For Round Robin scheduling
    int start_time; //to calculate the response time.
    int completion_time; //to calculate the turnaround time.
};

// Example test processes
struct Process processes[] = {
    // ... (previous processes)
    {11, 10, 6, 1},
    {12, 11, 8, 3},
    {13, 12, 4, 2},
    {14, 13, 7, 1},
    {15, 14, 3, 3},
    {16, 15, 6, 2},
    {17, 16, 5, 1},
    {18, 17, 4, 3},
    {19, 18, 7, 2},
    {20, 19, 9, 1},
};


/*Here I implement a queue data structure to store the processes, and help to achieve the RR scheduling
all processes' addresses are stored in pointers, and the pointers are stored again.
Thus, the processes in the priority queue could be directly accessed and changed.*/
typedef struct {
    struct Process** items; // Pointer to an array of pointers to struct Process
    int capacity;
    int front;
    int rear;
    int size;
} ProcessQueue;

void initQueue(ProcessQueue* q, int initialCapacity) {
    q->capacity = initialCapacity;
    q->items = (struct Process**)malloc(initialCapacity * sizeof(struct Process*));
    if (q->items == NULL) {
        printf("Failed to allocate memory.\n");
        exit(-1);
    }
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}
// It is equivalent to the isEmpty to check the running queue
int isFull(ProcessQueue* q) {
    return q->size == q->capacity;
}

int isEmpty(ProcessQueue* q) {
    return q->size == 0;
}
//No duplicate process!
int processExists(ProcessQueue* q, int pid) {
    int count = 0;
    for (int i = q->front; count < q->size; i = (i + 1) % q->capacity, ++count) {
        if (q->items[i]->pid == pid) {
            return 1;
        }
    }
    return 0;
}

int enqueue(ProcessQueue* q, struct Process* item) {
    if (isFull(q) || processExists(q, item->pid)) {
        return 0;
    }
    q->rear = (q->rear + 1) % q->capacity;
    q->items[q->rear] = item;
    q->size++;
    return 1;
}

struct Process* dequeue(ProcessQueue* q) {
    struct Process* errorProcess = NULL; // Use NULL to indicate an error or empty queue
    if (isEmpty(q)) {
        return errorProcess;
    }
    struct Process* item = q->items[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return item;
}
// Just for debugging.
void printQueue(ProcessQueue* q) {
    int count = 0;
    printf("Queue PIDs: ");
    for (int i = q->front; count < q->size; i = (i + 1) % q->capacity, ++count) {
        printf("%d ", q->items[i]->pid);
    }
    printf("\n");
}

void freeQueue(ProcessQueue* q) {
    free(q->items);
    q->items = NULL;
    q->capacity = 0;
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

//Global variables initialization:
int current_time = 0;

void schedule_processes_FIFO(struct Process queue[], int queue_size) {
    float total_response_time = 0,total_turnaround_time = 0;
    for (int i = 0; i < queue_size; ++i) {
        // Wait until the process arrives (if needed)
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        int response_time = current_time - processes[i].arrival_time;// The response is just when one process finished.
        total_response_time += response_time;
        // Execute the process
        current_time += processes[i].burst_time;

        // Calculate turnaround time and response time
        int turnaround_time = current_time - processes[i].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("The process ID: %d starts with a response time: %d, and completes with a turaround time: %d\n",queue[i].pid, response_time, turnaround_time);
    }
    printf("The average turnaround time is: %.2f, The average response time is: %.2f\n",total_turnaround_time/queue_size,total_response_time/queue_size);
}

//This helper function finds the earliest arrived process that is not started yet.
//For RR scheduling since there might be empty running queue.
struct Process* find_ealiest_arrived_process(struct Process queue[], int queue_size){
    int earliest_arrival_time = INT_MAX;
    struct Process* earliest_process = NULL;
    for (int i = 0; i < queue_size; ++i){
        if (queue[i].arrival_time < earliest_arrival_time && queue[i].remaining_time == queue[i].burst_time){
            earliest_arrival_time = queue[i].arrival_time;
            earliest_process = &queue[i];
        }
    }
    return earliest_process;
}

void schedule_process_roundrobin(struct Process queue[], int queue_size, int time_quantum) {
    //initialize the ready queue and running queue, and a pointer that will later points to the process.
    ProcessQueue ready_queue, running_queue;
    initQueue(&ready_queue, 10);
    initQueue(&running_queue, 1);
    struct Process *running_process = NULL;
    float total_turnaround_time = 0 , total_response_time = 0;
    int num_completed = 0;

    while (num_completed < queue_size){
        if (isEmpty(&running_queue)) {
            running_process = find_ealiest_arrived_process(queue,queue_size);
            if (running_process != NULL) enqueue(&running_queue, running_process);//Check if the process has been assigned.
        }
        //Check whether the process is started or not, if not, record the current time as execution time for calculating response time.
        if (running_process->remaining_time == running_process->burst_time){
            if (running_process->arrival_time < current_time)running_process->start_time = current_time;
            else {
                running_process->start_time = running_process->arrival_time;
                current_time = running_process->start_time;}
            int response_time = running_process->start_time - running_process->arrival_time;

            total_response_time += response_time;     
            printf("The process ID %d starts and has a response time: %d\n", running_process->pid, response_time);
        }
        //Execute the process either by quantum or remaining time, and get the turnaround time if it is completed.
        if (running_process->remaining_time > time_quantum){
            running_process->remaining_time -= time_quantum;
            current_time += time_quantum;
            }else{
                current_time += running_process->remaining_time;
                running_process->remaining_time = 0;
                running_process->completion_time = current_time;
                int turnaround_time = running_process->completion_time - running_process->arrival_time;
                total_turnaround_time += turnaround_time;
                num_completed ++;
                printf("The process id %d completes, and has a turnaround time: %d\n",running_process->pid, turnaround_time);
            }

        /*After the execution, we could easily know that if there are any candidate process(es) to store in the ready queue.
        The real scenario would be the MLQ predicts processes for the ready queue, not adding them after the execution.*/
        for (int j = 0; j < queue_size; ++j){
            if (running_process->pid != queue[j].pid && queue[j].remaining_time != 0){
                if (queue[j].arrival_time <= current_time) enqueue(&ready_queue, &queue[j]);
            }
        }
        /*For the running process, we need to dequeue it from the running queue.
        If the ready queue is not empty, then fill the running queue with the first element in ready queue.
        If the process is not finished, enqueue it to the ready queue.*/  
        if (running_process->remaining_time == 0){
            dequeue(&running_queue);
            if (!isEmpty(&ready_queue)){
                running_process = dequeue(&ready_queue);
                enqueue(&running_queue,running_process);
            }
        }else{
            enqueue(&ready_queue,dequeue(&running_queue));
            running_process = dequeue(&ready_queue);
            enqueue(&running_queue,running_process);
        }
    }
    //Need to clear the memory allocated for queue.
    freeQueue(&ready_queue);
    freeQueue(&running_queue);
    float average_turnaround_time = total_turnaround_time/queue_size;
    float average_response_time = total_response_time/queue_size;
    printf("The average turnaround time is %.2f, and the average response time is %.2f, using a Round Robin scheduling with a time quantum %d\n", average_turnaround_time, average_response_time, time_quantum);
}


void adjust_priorities_based_on_feedback(struct Process processes[], int num_processes) {
    int waiting_time_threshold = 4; // Example threshold for waiting time
    // Simulate calculation of waiting time and adjust priorities
    for (int i = 0; i < num_processes; ++i) {
        // Example: If a process's burst time is greater than a threshold, indicating it might have waited too long, increase its priority.
        if (processes[i].burst_time > waiting_time_threshold && processes[i].priority > 1) {
            processes[i].priority--; // Increase priority by decreasing the priority number
        }
    }
}


void schedule_processes(struct Process processes[], int num_processes) {
    // Initialize remaining time.
    for (int i = 0; i < num_processes; ++i) {
        processes[i].remaining_time = processes[i].burst_time;
    }
    //uncomment to adjust priorities based on a mock feedback mechanism before scheduling
    //adjust_priorities_based_on_feedback(processes, num_processes);

    // Separate queues for different priority levels
    struct Process high_priority_queue[10];
    struct Process medium_priority_queue[10];
    struct Process low_priority_queue[10];
    int high_size = 0, medium_size = 0, low_size = 0;

    // Distribute processes to respective queues based on priority.
    for (int i = 0; i < num_processes; ++i) {
        if (processes[i].priority == 1) {
            high_priority_queue[high_size++] = processes[i];
            
        } else if (processes[i].priority == 2) {
            medium_priority_queue[medium_size++] = processes[i];
        } else {
            low_priority_queue[low_size++] = processes[i];
        }
    }
    // Execute processes from each queue
    printf("\nProcess Execution Order:\n");

    // High-priority queue
    printf("\nThe high priority queue is scheduled by Round Robin.\n");
    schedule_process_roundrobin(high_priority_queue, high_size, 2);

    // Medium-priority queue
    printf("\nThe medium priority queue is scheduled by Round Robin.\n");
    schedule_process_roundrobin(medium_priority_queue, medium_size, 2);

    // Low-priority queue
    printf("\nThe low priority queue is scheduled by FIFO.\n");
    schedule_processes_FIFO(low_priority_queue, low_size);
    printf("All processes complete.");
}

int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    schedule_processes(processes, num_processes);
    return 0;
}
