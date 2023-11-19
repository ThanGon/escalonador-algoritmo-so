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

// bool organizarPorOrdemDeChegada(process p1, process p2) 
// { 
//     return p1.arrival_time < p2.arrival_time;
// }

bool organizarPorPids(process p1, process p2) 
{  
    return p1.pid < p2.pid;
}

// GERA PROCESSOS COM BURST TIME ALEATORIO
void geraProcessos(queue<int> *q, int nProcessos, int burst_cpy[], struct process p[]){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 15);
    for(int i = 0; i < nProcessos; i++) {
        p[i].pid = i+1;
        p[i].arrival_time = i;
        p[i].burst_time = dis(gen);
        burst_cpy[i] = p[i].burst_time;

        q->push(i);
    }
}

int main() {
    int nProcessos = NUM_PROCESSOS;
    int tempoQuantum = TEMPO_POR_PROCESSO;
    struct process p[nProcessos];
    float tempo_turnaround_medio;
    float tempo_espera_medio;
    float tempo_resposta_medio;
    float utilizacao_cpu;
    int tempo_turnaround_total = 0;
    int tempo_espera_total = 0;
    int tempo_resposta_total = 0;
    int tempo_ausente_total = 0;
    float throughput;
    // COPIA EM MEMORIA PARA TODOS OS TEMPOS DE BURSTS DOS PROCESSOS
    int burst_cpy[nProcessos];
    int idx;

    queue<int> q;

    cout << setprecision(2) << fixed;

    geraProcessos(&q, nProcessos, burst_cpy, p);

    int current_time = 0;
    int completed = 0;

    // LAÇO EM LOOP
    while(completed != nProcessos) {
        // CONSOME PRIMEIRO PROCESSO NA FILA
        idx = q.front();
        q.pop();


        // ENTRANDO NO ESCALONADOR
        if (burst_cpy[idx] == p[idx].burst_time) {
            p[idx].start_time = max(current_time, p[idx].arrival_time);
            tempo_ausente_total += p[idx].start_time - current_time;
            current_time = p[idx].start_time;
        }

        // TEMPO DE EXECUÇÃO DO PROCESSO
        if (burst_cpy[idx] - tempoQuantum > 0) {
            burst_cpy[idx] -= tempoQuantum;
            current_time += tempoQuantum;
            // RETORNA AO FINAL DA FILA
            q.push(idx);
        }
        // CONCLUSÃO DO PROCESSO 
        else {
            current_time += burst_cpy[idx];
            burst_cpy[idx] = 0;
            completed++;

            p[idx].completion_time = current_time;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            p[idx].response_time = p[idx].start_time - p[idx].arrival_time;

            tempo_turnaround_total += p[idx].turnaround_time;
            tempo_espera_total += p[idx].waiting_time;
                tempo_resposta_total += p[idx].response_time;
            }

    }

    tempo_turnaround_medio = (float) tempo_turnaround_total / nProcessos;
    tempo_espera_medio = (float) tempo_espera_total / nProcessos;
    tempo_resposta_medio = (float) tempo_resposta_total / nProcessos;
    utilizacao_cpu = ((p[nProcessos-1].completion_time - tempo_ausente_total) / (float) p[nProcessos-1].completion_time)*100;
    throughput = float(nProcessos) / (p[nProcessos-1].completion_time - p[0].arrival_time);

    sort(p,p+nProcessos,organizarPorPids);

    cout<<endl;
    // MONTA TABELA COM VALORES DOS PROCESSOS
    cout<<"PID\t"<<"AT(ms) "<<"BT(ms) "<<"ST(ms) "<<"CT(ms) "<<"TAT(ms) "<<"WT(ms) "<<"RT(ms) "<<"\n"<<endl;

    for(int i = 0; i < nProcessos; i++) {
        cout<<p[i].pid<<"\t"<<p[i].arrival_time<<"\t"<<p[i].burst_time<<"\t"<<p[i].start_time<<"\t"<<p[i].completion_time<<"\t"<<p[i].turnaround_time<<"\t"<<p[i].waiting_time<<"\t"<<p[i].response_time<<"\t"<<"\n"<<endl;
    }
    cout<<"Tempo de turnaroud medio = "<<tempo_turnaround_medio<<"ms"<<endl;
    cout<<"Tempo de espera medio = "<<tempo_espera_medio<<"ms"<<endl;
    cout<<"Tempo de resposta medio = "<<tempo_resposta_medio<<"ms"<<endl;
    cout<<"Utilizacao CPU = "<<utilizacao_cpu<<"%"<<endl;
    cout<<"Throughput = "<<throughput<<" processos/millisegundo"<<endl;
}


/*

Considerações:

AT - Tempo de chegada do processo
BT - Tempo de burst do processo (tempo determinado ate fim de execucao do processo)
ST - Tempo de inicio do processo
CT - Tempo de conclusao do processo
TAT - Tempo de turnaround do processo (tempo de conclusao efetiva desconsiderando)
WT - Tempo de espera do processo
RT - Tempo de resposta do processo (tempo registrado de quando o processo iniciou comparado ao tempo de chegada)

Equações:

TAT = CT - AT
WT = TAT - BT
RT = ST - AT

*/
