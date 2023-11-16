#include <iostream>
#include <algorithm> 
#include <iomanip>
#include <queue> 
#include <random>
#include <cstring>

#define NUM_PROCESSOS 10;
#define TEMPO_POR_PROCESSO 1;

using namespace std;

struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
};

// bool compare1(process p1, process p2) 
// { 
//     return p1.arrival_time < p2.arrival_time;
// }

bool organizarPorPids(process p1, process p2) 
{  
    return p1.pid < p2.pid;
}

void geraProcessos(queue<int> *q, int nProcessos, int burst_cpy[], struct process p[]){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 15);
    for(int i = 0; i < nProcessos; i++) {
        p[i].pid = i+1;
        p[i].arrival_time = dis(gen);
        p[i].burst_time = dis(gen);
        burst_cpy[i] = p[i].burst_time;

        q->push(i);
    }
}

int main() {
    int nProcessos = NUM_PROCESSOS;
    int tempoQuantum = TEMPO_POR_PROCESSO;
    struct process p[nProcessos];
    float avg_turnaround_time;
    float avg_waiting_time;
    float avg_response_time;
    float cpu_utilisation;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int total_idle_time = 0;
    float throughput;
    int burst_cpy[nProcessos];
    int idx;

    queue<int> q;

    cout << setprecision(2) << fixed;

    geraProcessos(&q, nProcessos, burst_cpy, p);

    int current_time = 0;
    int completed = 0;

    while(completed != nProcessos) {
        idx = q.front();
        q.pop();

        if (burst_cpy[idx] == p[idx].burst_time) {
            p[idx].start_time = max(current_time, p[idx].arrival_time);
            total_idle_time += p[idx].start_time - current_time;
            current_time = p[idx].start_time;
        }

        if (burst_cpy[idx] - tempoQuantum > 0) {
            burst_cpy[idx] -= tempoQuantum;
            current_time += tempoQuantum;
            q.push(idx);
        } else {
            current_time += burst_cpy[idx];
            burst_cpy[idx] = 0;
            completed++;

            p[idx].completion_time = current_time;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            p[idx].response_time = p[idx].start_time - p[idx].arrival_time;

            total_turnaround_time += p[idx].turnaround_time;
            total_waiting_time += p[idx].waiting_time;
            total_response_time += p[idx].response_time;
        }

    }

    avg_turnaround_time = (float) total_turnaround_time / nProcessos;
    avg_waiting_time = (float) total_waiting_time / nProcessos;
    avg_response_time = (float) total_response_time / nProcessos;
    cpu_utilisation = ((p[nProcessos-1].completion_time - total_idle_time) / (float) p[nProcessos-1].completion_time)*100;
    throughput = float(nProcessos) / (p[nProcessos-1].completion_time - p[0].arrival_time);

    sort(p,p+nProcessos,organizarPorPids);

    cout<<endl;
    cout<<"#P\t"<<"AT\t"<<"BT\t"<<"ST\t"<<"CT\t"<<"TAT\t"<<"WT\t"<<"RT\t"<<"\n"<<endl;

    for(int i = 0; i < nProcessos; i++) {
        cout<<p[i].pid<<"\t"<<p[i].arrival_time<<"\t"<<p[i].burst_time<<"\t"<<p[i].start_time<<"\t"<<p[i].completion_time<<"\t"<<p[i].turnaround_time<<"\t"<<p[i].waiting_time<<"\t"<<p[i].response_time<<"\t"<<"\n"<<endl;
    }
    cout<<"Average Turnaround Time = "<<avg_turnaround_time<<endl;
    cout<<"Average Waiting Time = "<<avg_waiting_time<<endl;
    cout<<"Average Response Time = "<<avg_response_time<<endl;
    cout<<"CPU Utilization = "<<cpu_utilisation<<"%"<<endl;
    cout<<"Throughput = "<<throughput<<" process/unit time"<<endl;


}

/*

AT - Arrival Time of the process
BT - Burst time of the process
ST - Start time of the process
CT - Completion time of the process
TAT - Turnaround time of the process
WT - Waiting time of the process
RT - Response time of the process

Formulas used:

TAT = CT - AT
WT = TAT - BT
RT = ST - AT

*/