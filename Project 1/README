Name: Keshav Saraogi
Lab: Project 1, CIS 3207
Date: September 16, 2020

#This is my take on the psuedocode behind the implementation of the stimulator. 

Introduction: With the help of the Giorgio's Discrete Event Simulator Lab, we will try and gain an understanding of how processes work and move through a computing system, understand a perspective about processes, CPU, Disks and understand virtualization overall. Through my simulator, I would provide 1 CPU and 2 Disks.  
Note: The term FIFO and queue are being used interchangeably throughout the explanation.

Procedure:
Main function
With 3 different FIFO queues and 1 Priority Queue being initialized as Linked List and the variables such as SEED, INIT_TIME all the way up to DISK2_MAX being initialized to the provided values in the module, there would be 2 events/ jobs that would be added to the priority queue waiting to be completed. Job1 would be passed on the CPU and simulation gets completed. 
While we know queue is not empty as there is a job remaining,
And while eventNode (A node tracking the events) is not equal to NULL and the time taken is less than the finish time, 
We can pop the event from the que taking track of the events and put that event into the CPU queue. 
We set the task to be completed to the first event that is left to be done. 
A variable created let’s say (currentTime) that is set to the time of the task as to keep track of the time taken of the task.
Now that the task has arrived, we would provide a switch case where we would have what type of task that needs to be done. 
	Case: arrival -- proceed to a method gets processed by the CPU
	Case: finish -- proceed to a method that puts the CPU into IDLE state
	Case: DISK_ARRIVAL -- proceed to a method that processes the         arrival of a task on a disk
	Case: DISK_FINISH -- proceed to a method that processes the job that lets us know what happens after the disk job has finished
	Case: end -- simulations are finished, break from the switch. 
End of main Function

Arrival -- 
If a task arrives on the CPU
	A new event gets created and gets appended onto the event queue. 
	This event is now sent to the vent queue.
	Here check for the CPU State: 
	If busy: 
		Pass
	If not busy: 
Pop the task from the CPU queue and compute its time with CPU_MIN and CPU_MAX
		Create a finished event by the CPU (An event that CPU finished)
		 Append the event into the event queue. 
		Change the CPU state to BUSY. 

Finish -- 
If the task is finished at the CPU: 
	Change the CPU state to IDLE
Use a probability method with 2 arguments yes or no (for example):
	If yes (that the disk is in use): 
		Create a new disk arrival event
		Append the event into the Event FIFO.
	If not (that the disk is not in use): 
		pass
	
DISK_ARRIVAL -- 
If a task arrives at the CPU
Find the disk that takes shortest amount of time to reach. I don’t know if I should include a case for equal time as it would be almost impossible to calculate a time that matches exactly for two different disks. 
Once the disk is selected, send the event to the disk queue. 
Compute the amount of time taken with the help of DISK_MIN and DISK_MAX. 
Once completed, create a disk finished event (An event that DISK finished). 
Append the event into the event queue. 
Change the DISK state to IDLE since the work done by the disk is competed.

DISK_FINISH -- 
If a task that came to the disk is now finished:
	Find a job that is either in disk 1 or disk 2
	Create a CPU arrival event, and append the event into the event FIFO
	Change the DISK state to IDLE as the work done by the disk is now done

