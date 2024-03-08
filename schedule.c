#include <stdio.h>
#include <stdbool.h>//for completed or not
#include <limits.h>//just a big number as a temporal variable for finding min.


struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time; // Additional field for SRJF to track remaining burst time
    bool completed;     // Flag to mark if the process has completed (used in SJF)
};

// Example test processes
// Example test processes
struct Process processes[] = {
    {1, 0, 10},
    {2, 2, 5},
    {3, 4, 8},
// Add more processes here
};


void schedule_processes_FIFO(struct Process processes[], int num_processes) {
    int current_time = 0; // Initialize current time
    float total_response_time = 0,total_turnaround_time = 0;
    printf("Process Execution Order:\n");

    for (int i = 0; i < num_processes; ++i) {
        int response_time = 0;
        // Wait until the process arrives (if needed)
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        response_time = current_time - processes[i].arrival_time;// The response is just when one process finished.
        total_response_time += response_time;
        // Execute the process
        printf("Executing Process %d (Burst Time: %d)\nResponse Time: %d\n", processes[i].pid, processes[i].burst_time,response_time);
        current_time += processes[i].burst_time;

        // Calculate turnaround time and response time
        int turnaround_time = current_time - processes[i].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[i].pid, turnaround_time);
        // Response time measurement need to be implemented.
    }
    printf("The average turnaround time is: %.2f\nThe average response time is: %.2f\n",total_turnaround_time/num_processes,total_response_time/num_processes);
}


// Function to implement Shortest Job First (SJF) scheduling
void schedule_processes_SJF(struct Process processes[], int num_processes) {
    printf("Process Execution Order:\n");
    int current_time = 0, num_completed = 0, total_turnaround_time = 0, total_response_time = 0;//set up the start time and a counter for completed process(es).
    
    while (num_completed != num_processes) {
        int min_job_index = -1;// A temporal variable for the minimum burst time index.
        int min_job_duration = INT_MAX; // A temporal variable for the minimum burst time - choose a large number.
        for (int i = 0; i < num_processes; i++) { // Find the shortest, non-completed process that has arrived
            if (processes[i].arrival_time <= current_time && !processes[i].completed && processes[i].burst_time < min_job_duration) {
                min_job_index = i;
                min_job_duration = processes[i].burst_time;
            }
        }
        if (min_job_index == -1) {current_time++;continue;} // If no process is found, increment current time

        // Before counting the burst time, print out the response time for the current process.
        int response_time = current_time - processes[min_job_index].arrival_time;
        total_response_time += response_time;
        // Count the burst time, and update that this process is completed.
        current_time += processes[min_job_index].burst_time;
        processes[min_job_index].completed = true;
        num_completed++;
        // The turnaround time when one process finishes.
        int turnaround_time = current_time - processes[min_job_index].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("Executing Process %d (Burst Time: %d)\nTurnaround Time: %d\nResponse time: %d\n", processes[min_job_index].pid, processes[min_job_index].burst_time,turnaround_time,response_time);
    }
    float avg_turnaround_time = total_turnaround_time /num_completed;
    float avg_response_time = total_response_time/num_completed;
    printf("The average turnaround time using SJR: %.2f\nThe average response time using SJR: %.2f\n", avg_turnaround_time, avg_response_time);
}


// Function to implement Shortest Remaining Job First (SRJF) scheduling
void schedule_processes_SRJF(struct Process processes[], int num_processes) {
    printf("Process Execution Order:\n");
    int current_time = 0, completed = 0;
    float total_turnaround_time = 0, total_response_time = 0;
    while (completed != num_processes) {

        int response_time = 0, shortest_index = -1, min_remaining_time = INT_MAX; 
        for (int i = 0; i < num_processes; i++) {
            // Find process with shortest remaining time that has arrived
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time < min_remaining_time && processes[i].remaining_time > 0) {
                shortest_index = i;
                min_remaining_time = processes[i].remaining_time;
            }
        }//We find the shortest index, check if the process started ever before, if not, record as the reponse time.
        if (processes[shortest_index].burst_time == processes[shortest_index].remaining_time) {
            response_time = current_time - processes[shortest_index].arrival_time;
            printf("Process %d starts with a response Time: %d\n",processes[shortest_index].pid,response_time);
            }
        if (shortest_index == -1) {current_time++;continue;}// If no process is ready to execute, increment current time.
        // Execute the process for 1 time unit, so the shortest job will consume 1 time unit.
        processes[shortest_index].remaining_time--;
        current_time++;
        if (processes[shortest_index].remaining_time == 0) {//job is done.
            completed++;
            int turnaround_time = current_time - processes[shortest_index].arrival_time;
            total_turnaround_time += turnaround_time;
            printf("Process %d completes. Turnaround Time: %d\n", processes[shortest_index].pid, turnaround_time);
        }
        total_response_time += response_time;
    }
    printf("The average turnaround time is %.2f\nThe average response time is: %.2f\n", total_turnaround_time/completed,total_response_time/completed);
}


int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    printf("\nFirst In First Out (FIFO) Scheduling\n");
    schedule_processes_FIFO (processes, num_processes);
    // Initialize remaining times for SRJF and completion flags for SJF
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completed = false;
    }

    // Schedule processes using SJF
    printf("\nShortest Job First (SJF) Scheduling\n");
    schedule_processes_SJF(processes, num_processes);

    // Reset the remaining time from SJF
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].burst_time;
    }

    // Schedule processes using SRJF
    printf("\nShortest Remaining Job First (SRJF) Scheduling\n");
    schedule_processes_SRJF(processes, num_processes);

    return 0;
}
