This is the 2nd homework assignment for Operating System, the standard output is in the output.txt file.<br>
There is a simple feedback mechanism in mlq.c, to implement that function, just uncomment:<br>
//adjust_priorities_based_on_feedback(processes, num_processes); in the schedule_process function.


1. From homework 2 assignment, I learned many process scheduling policies: including FIFO, SJF, SRJF, RR, and a real life scenario - MLQ.
2. Implementing the SJF is straightforward:<br>
Always looking for the minimum burst time job, and execute it.<br>
The SRJF is a preemptive version of SJF:<br>
Instead of running the whole process, my SRJF only runs one time unit for the current process, then check if there is any process having less remaining time, update the process and repeat.<br>
3. Implementing MLQ requires a round robin scheduling:<br>
I created a queue data structure that stores processes' addresses to directly access to the process.<br>
The ready queue is preemptive, storing the next processes for the running queue, and the running queue has only one slot for the current running process.<br>
The running process will run for a time slice, searching for the next candidate processes, and then dequeue from the ready queue.<br>
The next running process will dequeue from the ready queue, and enequeue to the running queue.<br>
The program will iteratively proceed until all processes are finished.

### Credit:<br>
For calculating response, turnaround time:<br>
https://www.baeldung.com/cs/cpu-scheduling<br>
For round and robin scheduling, I just use the algorithm from this website, but implement it in a totally different way:<br>
https://www.geeksforgeeks.org/program-for-round-robin-scheduling-for-the-same-arrival-time/<Br>
For SJF scheduling:<br>
https://www.geeksforgeeks.org/program-for-shortest-job-first-or-sjf-cpu-scheduling-set-1-non-preemptive/<br>
