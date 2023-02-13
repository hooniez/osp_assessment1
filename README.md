# osp_assessment1

The assignment is about implementing a Load Balancing technique, specifically Static Load Balancing (S-LBAN), in the form of a map/reduce problem in C/C++ code. The purpose of this implementation is to evaluate the accuracy of Load Balancing and whether it is worth the effort in terms of net performance. The assignment involves dividing a problem into smaller components, mapping the input to available resources, and prioritizing job scheduling to solve each component problem. The solutions to these component problems are then combined by the reduce() process to form the global solution. The performance of each method will be measured and documented, and in later methods, adjustments will be made based on the performance metrics obtained. The ultimate goal is to minimize idle time and reduce the waiting time for reduce(). The assignment is based on the key trade-offs between multiple approaches to operating system design, and the student is expected to analyze and evaluate appropriate design choices. The solution must not use busy waiting and should minimize the amount of shared state between threads to avoid conflicts. The code must be built using a Makefile and must run on the CS servers using a supported c++ compiler. The program must have a graceful exit to account for thread starvation.

## How to run each task
### Task1
- type make
- ./Task1Filter DirtyFile CleanFile 
- For Task1, use combined.txt inside original_data

### Task2
- type make
- ./Task2 DirtyFile CleanFile

### Task3
- type make
- ./Task3 DirtyFile CleanFile

### Task4
- type make
- ./Task4 DirtyFile CleanFile

### Task5
- type make
- cat DirtyFile | ./Task5 CleanFile
