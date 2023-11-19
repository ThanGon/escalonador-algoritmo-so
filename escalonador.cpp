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

#pragma region "Funcoes Gerais"
// GERA PROCESSOS COM BURST TIME ALEATORIO (distribuicao uniforme entre 0 e 15 ms)
void geraProcessos(queue<int> *q, int nProcessos, struct process p[]){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 15);
    for(int i = 0; i < nProcessos; i++) {
        p[i].pid = i+1;
        p[i].arrival_time = dis(gen);
        p[i].burst_time = dis(gen);

        q->push(i);
    }
}

bool organizarPorPids(process p1, process p2) 
{  
    return p1.pid < p2.pid;
}

void montarTabela(int nProcessos, struct process p[], float tempo_turnaround_medio, float tempo_espera_medio, float tempo_resposta_medio, float utilizacao_cpu, float throughput) {
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
#pragma endregion

#pragma region "Escalonador Round Robin"
void copiaTemposBurst(int burst_cpy[], struct process p[], int nProcessos) {
    for(int i = 0; i < nProcessos; i++) {
        burst_cpy[i] = p[i].burst_time;
    }
}

void escalonador_round_robin(int nProcessos, struct process p[], queue<int> q, int tempo_atual, int completo, int *tempo_turnaround_total, int *tempo_espera_total, int *tempo_resposta_total, int *tempo_ausente_total) {
    int tempoQuantum = TEMPO_POR_PROCESSO;

    // COPIA PARA TODOS OS TEMPOS DE BURSTS DOS PROCESSOS
    int burst_cpy[nProcessos];

    int idx;

    // COPIA EM MEMORIA PARA TODOS OS TEMPOS DE BURSTS DOS PROCESSOS
    copiaTemposBurst(burst_cpy, p, nProcessos);

    // LAÇO EM LOOP
    while(completo != nProcessos) {
        // CONSOME PRIMEIRO PROCESSO NA FILA
        idx = q.front();
        q.pop();


        // ENTRANDO NO ESCALONADOR
        if (burst_cpy[idx] == p[idx].burst_time) {
            p[idx].start_time = max(tempo_atual, p[idx].arrival_time);
            tempo_ausente_total += p[idx].start_time - tempo_atual;
            tempo_atual = p[idx].start_time;
        }

        // TEMPO DE EXECUÇÃO DO PROCESSO
        if (burst_cpy[idx] - tempoQuantum > 0) {
            burst_cpy[idx] -= tempoQuantum;
            tempo_atual += tempoQuantum;
            // RETORNA AO FINAL DA FILA
            q.push(idx);
        }
        // CONCLUSÃO DO PROCESSO 
        else {
            tempo_atual += burst_cpy[idx];
            burst_cpy[idx] = 0;
            completo++;

            p[idx].completion_time = tempo_atual;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            p[idx].response_time = p[idx].start_time - p[idx].arrival_time;

            *tempo_turnaround_total += p[idx].turnaround_time;
            *tempo_espera_total += p[idx].waiting_time;
            *tempo_resposta_total += p[idx].response_time;
            }
    }

}



#pragma endregion

#pragma region "Escalonador Shortest Job First"


std::pair<int, int> calculaLimites(int nProcessos, struct process p[]) {
    int tempo_chegada_min = 100000;
    int tempo_conclusao_max = -1;
    for(int i = 0; i < nProcessos; i++) {
        tempo_chegada_min = min(tempo_chegada_min,p[i].arrival_time);
        tempo_conclusao_max = max(tempo_conclusao_max,p[i].completion_time);
    }
    return make_pair(tempo_chegada_min, tempo_conclusao_max);
}

void escalonador_sjf(int nProcessos, struct process p[], int tempo_atual, int completo, int *tempo_turnaround_total, int *tempo_espera_total, int *tempo_resposta_total, int *tempo_ausente_total) {
    int idx;
    int tempo_processo_previo = 0;
    int is_completed[nProcessos];
    memset(is_completed,0,sizeof(is_completed));

    while(completo != nProcessos) {
        idx = -1;
        int minimo = 100000;

        // VARRE PELO PROCESSO COM MENOR TEMPO DE BURST (SHORTEST JOB)
        for(int i = 0; i < nProcessos; i++) {
            if(p[i].arrival_time <= tempo_atual && is_completed[i] == 0) {
                if(p[i].burst_time < minimo) {
                    minimo = p[i].burst_time;
                    idx = i;
                }
                if(p[i].burst_time == minimo) {
                    if(p[i].arrival_time < p[idx].arrival_time) {
                        minimo = p[i].burst_time;
                        idx = i;
                    }
                }
            }
        }
        if(idx != -1) {
            p[idx].start_time = tempo_atual;
            p[idx].completion_time = p[idx].start_time + p[idx].burst_time;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            p[idx].response_time = p[idx].start_time - p[idx].arrival_time;
            
            *tempo_turnaround_total += p[idx].turnaround_time;
            *tempo_espera_total += p[idx].waiting_time;
            *tempo_resposta_total += p[idx].response_time;
            *tempo_ausente_total += p[idx].start_time - tempo_processo_previo;

            is_completed[idx] = 1;
            completo++;
            tempo_atual = p[idx].completion_time;
            tempo_processo_previo = tempo_atual;
        }
        else {
            tempo_atual++;
        }
        
    }
}

#pragma endregion

int main() {
    int nProcessos = NUM_PROCESSOS;
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

    queue<int> q;

    cout << setprecision(2) << fixed;

    geraProcessos(&q, nProcessos, p);

    int tempo_atual = 0;
    int completo = 0;

    escalonador_round_robin(nProcessos, p, q, tempo_atual, completo, &tempo_turnaround_total, &tempo_espera_total, &tempo_resposta_total, &tempo_ausente_total);

    sort(p,p+nProcessos,organizarPorPids);

    tempo_turnaround_medio = (float) tempo_turnaround_total / nProcessos;
    tempo_espera_medio = (float) tempo_espera_total / nProcessos;
    tempo_resposta_medio = (float) tempo_resposta_total / nProcessos;
    utilizacao_cpu = ((p[nProcessos-1].completion_time - tempo_ausente_total) / (float) p[nProcessos-1].completion_time)*100;
    throughput = float(nProcessos) / (p[nProcessos-1].completion_time - p[0].arrival_time);


    cout<<endl;

    cout<<"Escalonador Round Robin"<<endl;
    montarTabela(nProcessos, p, tempo_turnaround_medio, tempo_espera_medio, tempo_resposta_medio, utilizacao_cpu, throughput);
    cout<<"\n"<<endl;

    escalonador_sjf(nProcessos, p, tempo_atual, completo, &tempo_turnaround_total, &tempo_espera_total, &tempo_resposta_total, &tempo_ausente_total);

    tempo_turnaround_medio = (float) tempo_turnaround_total / nProcessos;
    tempo_espera_medio = (float) tempo_espera_total / nProcessos;
    tempo_resposta_medio = (float) tempo_resposta_total / nProcessos;
    int tempo_chegada_min, tempo_conclusao_max;
    std::tie(tempo_chegada_min, tempo_conclusao_max) = calculaLimites(nProcessos, p);
    utilizacao_cpu = ((tempo_conclusao_max - tempo_ausente_total) / (float) tempo_conclusao_max)*100;
    throughput = float(nProcessos) / (tempo_conclusao_max - tempo_chegada_min);

    cout<<"Escalonador Shortest Job First"<<endl;
    montarTabela(nProcessos, p, tempo_turnaround_medio, tempo_espera_medio, tempo_resposta_medio, utilizacao_cpu, throughput);
    cout<<"\n"<<endl;
}


/*

Considerações:

AT - Tempo de chegada do processo
BT - Tempo de burst do processo (tempo determinado ate fim de execucao do processo)
ST - Tempo de inicio do processo
CT - Tempo de conclusao do processo (tempo de conclusao relativa ao comeco do tempo)
TAT - Tempo de turnaround do processo (tempo de conclusao efetiva)
WT - Tempo de espera do processo
RT - Tempo de resposta do processo (tempo registrado de quando o processo iniciou comparado ao tempo de chegada)

Equações:

TAT = CT - AT
WT = TAT - BT
RT = ST - AT

*/


