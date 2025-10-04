#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <climits>
#include <limits>
using namespace std;

struct Process {
    int id;
    string name;
    int arrivalTime;
    int burstTime;
    int priority;
    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;
    bool started;
    
    Process(int i, string n, int at, int bt, int p) {
        id = i;
        name = n;
        arrivalTime = at;
        burstTime = bt;
        priority = p;
        remainingTime = bt;
        completionTime = 0;
        waitingTime = 0;
        turnaroundTime = 0;
        responseTime = -1;
        started = false;
    }
};

vector<Process> processes;

// Utility function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Utility function to get valid integer input
int getValidInteger(const string& prompt, int minValue = 0, int maxValue = INT_MAX) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minValue && value <= maxValue) {
                clearInputBuffer();
                return value;
            } else {
                cout << "Error: Value must be between " << minValue << " and " << maxValue << endl;
            }
        } else {
            cout << "Error: Invalid input. Please enter a valid integer.\n";
            clearInputBuffer();
        }
    }
}

// Utility function to get valid string input
string getValidString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (!value.empty() && value.length() <= 20) {
            clearInputBuffer();
            return value;
        } else {
            cout << "Error: Name must be 1-20 characters long.\n";
        }
    }
}

// Check if there are any processes
bool checkProcessesExist() {
    if (processes.empty()) {
        cout << "\nError: No processes available! Please add processes first.\n";
        return false;
    }
    return true;
}

void displayResults(vector<Process> &procs) {
    if (procs.empty()) {
        cout << "\nError: No processes to display!\n";
        return;
    }
    
    cout << "\n" << string(90, '=') << endl;
    cout << left << setw(12) << "Process" 
         << setw(10) << "Arrival" 
         << setw(10) << "Burst"
         << setw(12) << "Completion"
         << setw(12) << "Turnaround"
         << setw(10) << "Waiting"
         << setw(10) << "Response" << endl;
    cout << string(90, '-') << endl;
    
    double avgWT = 0, avgTAT = 0, avgRT = 0;
    
    for (auto &p : procs) {
        cout << left << setw(12) << p.name 
             << setw(10) << p.arrivalTime 
             << setw(10) << p.burstTime
             << setw(12) << p.completionTime
             << setw(12) << p.turnaroundTime
             << setw(10) << p.waitingTime
             << setw(10) << p.responseTime << endl;
        
        avgWT += p.waitingTime;
        avgTAT += p.turnaroundTime;
        avgRT += p.responseTime;
    }
    
    int n = procs.size();
    cout << string(90, '=') << endl;
    cout << fixed << setprecision(2);
    cout << "Average Waiting Time: " << avgWT / n << endl;
    cout << "Average Turnaround Time: " << avgTAT / n << endl;
    cout << "Average Response Time: " << avgRT / n << endl;
    
    // Calculate and display CPU utilization
    int maxCompletionTime = 0;
    int totalBurstTime = 0;
    for (auto &p : procs) {
        maxCompletionTime = max(maxCompletionTime, p.completionTime);
        totalBurstTime += p.burstTime;
    }
    if (maxCompletionTime > 0) {
        double cpuUtilization = (double)totalBurstTime / maxCompletionTime * 100;
        cout << "CPU Utilization: " << cpuUtilization << "%" << endl;
    }
    
    cout << string(90, '=') << endl;
}

void fcfs() {
    if (!checkProcessesExist()) return;
    
    cout << "\n*** FCFS (First Come First Served) ***\n";
    vector<Process> procs = processes;
    
    sort(procs.begin(), procs.end(), [](Process &a, Process &b) {
        if (a.arrivalTime == b.arrivalTime) {
            return a.id < b.id; // Tie-breaker: use process ID
        }
        return a.arrivalTime < b.arrivalTime;
    });
    
    int currentTime = 0;
    
    for (auto &p : procs) {
        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }
        
        p.responseTime = currentTime - p.arrivalTime;
        currentTime += p.burstTime;
        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
    }
    
    displayResults(procs);
}

void sjf() {
    if (!checkProcessesExist()) return;
    
    cout << "\n*** SJF (Shortest Job First - Non-Preemptive) ***\n";
    vector<Process> procs = processes;
    vector<Process> completed;
    vector<bool> isCompleted(procs.size(), false);
    int currentTime = 0;
    int completedCount = 0;
    int n = procs.size();
    
    while (completedCount < n) {
        int shortest = -1;
        int minBurst = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (!isCompleted[i] && procs[i].arrivalTime <= currentTime) {
                if (procs[i].burstTime < minBurst || 
                    (procs[i].burstTime == minBurst && procs[i].arrivalTime < procs[shortest].arrivalTime)) {
                    minBurst = procs[i].burstTime;
                    shortest = i;
                }
            }
        }
        
        if (shortest == -1) {
            // No process is ready, jump to next arrival
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!isCompleted[i] && procs[i].arrivalTime > currentTime) {
                    nextArrival = min(nextArrival, procs[i].arrivalTime);
                }
            }
            currentTime = (nextArrival == INT_MAX) ? currentTime + 1 : nextArrival;
            continue;
        }
        
        procs[shortest].responseTime = currentTime - procs[shortest].arrivalTime;
        currentTime += procs[shortest].burstTime;
        procs[shortest].completionTime = currentTime;
        procs[shortest].turnaroundTime = procs[shortest].completionTime - procs[shortest].arrivalTime;
        procs[shortest].waitingTime = procs[shortest].turnaroundTime - procs[shortest].burstTime;
        
        isCompleted[shortest] = true;
        completedCount++;
    }
    
    displayResults(procs);
}

void srtf() {
    if (!checkProcessesExist()) return;
    
    cout << "\n*** SRTF (Shortest Remaining Time First - Preemptive) ***\n";
    vector<Process> procs = processes;
    int currentTime = 0;
    int completed = 0;
    int n = procs.size();
    
    // Find the maximum completion time possible
    int maxTime = 0;
    for (auto &p : procs) {
        maxTime += p.burstTime;
    }
    maxTime += procs[n-1].arrivalTime + 1000; // Safety buffer
    
    while (completed < n && currentTime < maxTime) {
        int shortest = -1;
        int minRemaining = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (procs[i].arrivalTime <= currentTime && procs[i].remainingTime > 0) {
                if (procs[i].remainingTime < minRemaining ||
                    (procs[i].remainingTime == minRemaining && procs[i].arrivalTime < procs[shortest].arrivalTime)) {
                    minRemaining = procs[i].remainingTime;
                    shortest = i;
                }
            }
        }
        
        if (shortest == -1) {
            currentTime++;
            continue;
        }
        
        if (!procs[shortest].started) {
            procs[shortest].responseTime = currentTime - procs[shortest].arrivalTime;
            procs[shortest].started = true;
        }
        
        procs[shortest].remainingTime--;
        currentTime++;
        
        if (procs[shortest].remainingTime == 0) {
            completed++;
            procs[shortest].completionTime = currentTime;
            procs[shortest].turnaroundTime = procs[shortest].completionTime - procs[shortest].arrivalTime;
            procs[shortest].waitingTime = procs[shortest].turnaroundTime - procs[shortest].burstTime;
        }
    }
    
    if (completed < n) {
        cout << "\nWarning: Not all processes could be completed (possible infinite loop detected).\n";
    }
    
    displayResults(procs);
}

void priorityNonPreemptive() {
    if (!checkProcessesExist()) return;
    
    cout << "\n*** Priority Scheduling (Non-Preemptive) ***\n";
    cout << "Note: Lower priority number = Higher priority\n";
    
    vector<Process> procs = processes;
    vector<bool> isCompleted(procs.size(), false);
    int currentTime = 0;
    int completedCount = 0;
    int n = procs.size();
    
    while (completedCount < n) {
        int highest = -1;
        int minPriority = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (!isCompleted[i] && procs[i].arrivalTime <= currentTime) {
                if (procs[i].priority < minPriority ||
                    (procs[i].priority == minPriority && procs[i].arrivalTime < procs[highest].arrivalTime)) {
                    minPriority = procs[i].priority;
                    highest = i;
                }
            }
        }
        
        if (highest == -1) {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!isCompleted[i] && procs[i].arrivalTime > currentTime) {
                    nextArrival = min(nextArrival, procs[i].arrivalTime);
                }
            }
            currentTime = (nextArrival == INT_MAX) ? currentTime + 1 : nextArrival;
            continue;
        }
        
        procs[highest].responseTime = currentTime - procs[highest].arrivalTime;
        currentTime += procs[highest].burstTime;
        procs[highest].completionTime = currentTime;
        procs[highest].turnaroundTime = procs[highest].completionTime - procs[highest].arrivalTime;
        procs[highest].waitingTime = procs[highest].turnaroundTime - procs[highest].burstTime;
        
        isCompleted[highest] = true;
        completedCount++;
    }
    
    displayResults(procs);
}

void priorityPreemptive() {
    if (!checkProcessesExist()) return;
    
    cout << "\n*** Priority Scheduling (Preemptive) ***\n";
    cout << "Note: Lower priority number = Higher priority\n";
    
    vector<Process> procs = processes;
    int currentTime = 0;
    int completed = 0;
    int n = procs.size();
    
    int maxTime = 0;
    for (auto &p : procs) {
        maxTime += p.burstTime;
    }
    maxTime += procs[n-1].arrivalTime + 1000;
    
    while (completed < n && currentTime < maxTime) {
        int highest = -1;
        int minPriority = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (procs[i].arrivalTime <= currentTime && procs[i].remainingTime > 0) {
                if (procs[i].priority < minPriority ||
                    (procs[i].priority == minPriority && procs[i].arrivalTime < procs[highest].arrivalTime)) {
                    minPriority = procs[i].priority;
                    highest = i;
                }
            }
        }
        
        if (highest == -1) {
            currentTime++;
            continue;
        }
        
        if (!procs[highest].started) {
            procs[highest].responseTime = currentTime - procs[highest].arrivalTime;
            procs[highest].started = true;
        }
        
        procs[highest].remainingTime--;
        currentTime++;
        
        if (procs[highest].remainingTime == 0) {
            completed++;
            procs[highest].completionTime = currentTime;
            procs[highest].turnaroundTime = procs[highest].completionTime - procs[highest].arrivalTime;
            procs[highest].waitingTime = procs[highest].turnaroundTime - procs[highest].burstTime;
        }
    }
    
    if (completed < n) {
        cout << "\nWarning: Not all processes could be completed (possible infinite loop detected).\n";
    }
    
    displayResults(procs);
}

void roundRobin() {
    if (!checkProcessesExist()) return;
    
    int quantum = getValidInteger("\nEnter Time Quantum (1-100): ", 1, 100);
    
    cout << "\n*** Round Robin (Time Quantum = " << quantum << ") ***\n";
    vector<Process> procs = processes;
    queue<int> readyQueue;
    vector<bool> inQueue(procs.size(), false);
    int currentTime = 0;
    int completed = 0;
    int n = procs.size();
    
    int maxTime = 0;
    for (auto &p : procs) {
        maxTime += p.burstTime;
    }
    maxTime += procs[n-1].arrivalTime + 1000;
    
    // Add processes that arrive at time 0
    for (int i = 0; i < n; i++) {
        if (procs[i].arrivalTime <= currentTime) {
            readyQueue.push(i);
            inQueue[i] = true;
        }
    }
    
    while (completed < n && currentTime < maxTime) {
        if (readyQueue.empty()) {
            // Jump to next arrival time
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (procs[i].remainingTime > 0 && procs[i].arrivalTime > currentTime) {
                    nextArrival = min(nextArrival, procs[i].arrivalTime);
                }
            }
            
            if (nextArrival == INT_MAX) break;
            currentTime = nextArrival;
            
            for (int i = 0; i < n; i++) {
                if (procs[i].arrivalTime <= currentTime && !inQueue[i] && procs[i].remainingTime > 0) {
                    readyQueue.push(i);
                    inQueue[i] = true;
                }
            }
            continue;
        }
        
        int idx = readyQueue.front();
        readyQueue.pop();
        inQueue[idx] = false;
        
        if (!procs[idx].started) {
            procs[idx].responseTime = currentTime - procs[idx].arrivalTime;
            procs[idx].started = true;
        }
        
        int execTime = min(quantum, procs[idx].remainingTime);
        procs[idx].remainingTime -= execTime;
        currentTime += execTime;
        
        // Add newly arrived processes
        for (int i = 0; i < n; i++) {
            if (procs[i].arrivalTime <= currentTime && !inQueue[i] && procs[i].remainingTime > 0 && i != idx) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }
        
        if (procs[idx].remainingTime > 0) {
            readyQueue.push(idx);
            inQueue[idx] = true;
        } else {
            completed++;
            procs[idx].completionTime = currentTime;
            procs[idx].turnaroundTime = procs[idx].completionTime - procs[idx].arrivalTime;
            procs[idx].waitingTime = procs[idx].turnaroundTime - procs[idx].burstTime;
        }
    }
    
    if (completed < n) {
        cout << "\nWarning: Not all processes could be completed.\n";
    }
    
    displayResults(procs);
}

void addProcess() {
    if (processes.size() >= 100) {
        cout << "\nError: Maximum process limit (100) reached!\n";
        return;
    }
    
    cout << "\n--- Add New Process ---\n";
    string name = getValidString("Enter Process Name (1-20 chars): ");
    
    // Check for duplicate names
    for (auto &p : processes) {
        if (p.name == name) {
            cout << "Warning: A process with this name already exists.\n";
            break;
        }
    }
    
    int arrival = getValidInteger("Enter Arrival Time (0-1000): ", 0, 1000);
    int burst = getValidInteger("Enter Burst Time (1-100): ", 1, 100);
    int priority = getValidInteger("Enter Priority (0-99, lower = higher priority): ", 0, 99);
    
    processes.push_back(Process(processes.size() + 1, name, arrival, burst, priority));
    cout << "\n✓ Process '" << name << "' added successfully!\n";
}

void displayProcesses() {
    if (!checkProcessesExist()) return;
    
    cout << "\n" << string(70, '=') << endl;
    cout << "                    CURRENT PROCESSES" << endl;
    cout << string(70, '=') << endl;
    cout << left << setw(12) << "Process" 
         << setw(15) << "Arrival Time" 
         << setw(15) << "Burst Time"
         << setw(12) << "Priority" << endl;
    cout << string(70, '-') << endl;
    
    for (auto &p : processes) {
        cout << left << setw(12) << p.name 
             << setw(15) << p.arrivalTime 
             << setw(15) << p.burstTime
             << setw(12) << p.priority << endl;
    }
    cout << string(70, '=') << endl;
    cout << "Total Processes: " << processes.size() << endl;
}

void loadSampleData() {
    processes.clear();
    processes.push_back(Process(1, "P1", 0, 5, 2));
    processes.push_back(Process(2, "P2", 1, 3, 1));
    processes.push_back(Process(3, "P3", 2, 8, 3));
    processes.push_back(Process(4, "P4", 3, 6, 2));
    cout << "\n✓ Sample processes loaded successfully!\n";
    displayProcesses();
}

void clearAllProcesses() {
    if (processes.empty()) {
        cout << "\nNo processes to clear.\n";
        return;
    }
    
    char confirm;
    cout << "\nAre you sure you want to clear all processes? (y/n): ";
    cin >> confirm;
    clearInputBuffer();
    
    if (confirm == 'y' || confirm == 'Y') {
        processes.clear();
        cout << "\n✓ All processes cleared successfully!\n";
    } else {
        cout << "\nOperation cancelled.\n";
    }
}

int main() {
    int choice;
    
    cout << "\n";
    cout << "========================================================\n";
    cout << "       CPU SCHEDULING ALGORITHMS SIMULATOR\n";
    cout << "========================================================\n";
    
    do {
        cout << "\n============ MENU ============\n";
        cout << "1.  Add Process\n";
        cout << "2.  Display All Processes\n";
        cout << "3.  Load Sample Data\n";
        cout << "4.  Clear All Processes\n";
        cout << "5.  Run FCFS\n";
        cout << "6.  Run SJF (Non-Preemptive)\n";
        cout << "7.  Run SRTF (Preemptive SJF)\n";
        cout << "8.  Run Priority (Non-Preemptive)\n";
        cout << "9.  Run Priority (Preemptive)\n";
        cout << "10. Run Round Robin\n";
        cout << "0.  Exit\n";
        cout << "==============================\n";
        
        choice = getValidInteger("Enter choice: ", 0, 10);
        
        switch (choice) {
            case 1: addProcess(); break;
            case 2: displayProcesses(); break;
            case 3: loadSampleData(); break;
            case 4: clearAllProcesses(); break;
            case 5: fcfs(); break;
            case 6: sjf(); break;
            case 7: srtf(); break;
            case 8: priorityNonPreemptive(); break;
            case 9: priorityPreemptive(); break;
            case 10: roundRobin(); break;
            case 0: 
                cout << "\n========================================================\n";
                cout << "   Thank you for using the CPU Scheduling Simulator!\n";
                cout << "========================================================\n\n";
                break;
        }
    } while (choice != 0);
    
    return 0;
}
