/*
 * TAREFA: O COFRE DO BANCO (COM BUG E VALORES VARIÁVEIS)
 * OBJETIVO: Este codigo tem um bug de condicao de corrida.
 * O saldo final impresso esta incorreto.
 * MISSAO: Corrigir o bug usando um Mutex (API do Windows).
 */

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#define N_THREADS 5 


double g_saldoBancario = 1000.0; 
HANDLE hMutex; 


typedef struct {
    const char* nome;
    int tipoOperacao; 
    double valor;     
} ThreadArgs;



void operacaoDeposito(const char* nome, double valor) {
    
    WaitForSingleObject(hMutex, INFINITE);

    printf("%s [Agencia] quer depositar %.2f...\n", nome, valor);
    fflush(stdout);
    
    double saldoLocal = g_saldoBancario; 
    printf("%s [Agencia]... leu o saldo de %.2f\n", nome, saldoLocal);
    
    Sleep(2000); 
    
    saldoLocal += valor; 
    
    g_saldoBancario = saldoLocal; 
    
    printf("%s [Agencia]... DEPOSITOU %.2f. Novo saldo (na sua visao): %.2f\n", nome, valor, g_saldoBancario);
    fflush(stdout);

    ReleaseMutex(hMutex);
    
}


void operacaoSaque(const char* nome, double valor) {
    // --- INÍCIO DA SESSÃO CRÍTICA ---
    WaitForSingleObject(hMutex, INFINITE);

    printf("%s [Agencia] quer sacar %.2f...\n", nome, valor);
    fflush(stdout);

    double saldoLocal = g_saldoBancario; 
    printf("%s [Agencia]... leu o saldo de %.2f\n", nome, saldoLocal);

    Sleep(1500); 
    
    saldoLocal -= valor; 
    
    g_saldoBancario = saldoLocal; 

    printf("%s [Agencia]... SACOU %.2f. Novo saldo (na sua visao): %.2f\n", nome, valor, g_saldoBancario);
    fflush(stdout);

    ReleaseMutex(hMutex);

}


DWORD WINAPI threadRoutine(LPVOID lpParam) {
    ThreadArgs* args = (ThreadArgs*)lpParam;
    
    if (args->tipoOperacao == 1) {
        operacaoDeposito(args->nome, args->valor);
    } else if (args->tipoOperacao == 2) {
        operacaoSaque(args->nome, args->valor);
    }
    
    free(args); 
    return 0;
}

int main() {

    const char* nomes[N_THREADS] = {"Agencia_A", "Agencia_B", "Caixa_C", "Agencia_D", "Caixa_E"};
    int operacoes[N_THREADS] = {1, 2, 1, 2, 1}; 
    double valores[N_THREADS] = {500.0, 300.0, 200.0, 700.0, 100.0};
    

    // 1. Inicializa o Mutex
    hMutex = CreateMutex(
        NULL,              
        FALSE,             
        NULL               
    );

    if (hMutex == NULL) {
        printf("Erro ao criar Mutex: %lu\n", GetLastError());
        return 1;
    }

    HANDLE threads[N_THREADS];
    ThreadArgs* threadArgs[N_THREADS];

    printf("Simulacao iniciada. Saldo inicial: %.2f\n", g_saldoBancario);
    printf("-------------------------------------------\n");

    int i = 0;
    for (i; i < N_THREADS; ++i) {
        threadArgs[i] = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        if (threadArgs[i] == NULL) {
            perror("Erro ao alocar memoria para args");
            return 1;
        }

        threadArgs[i]->nome = nomes[i];
        threadArgs[i]->tipoOperacao = operacoes[i];
        threadArgs[i]->valor = valores[i];

        threads[i] = CreateThread(
            NULL, 0, threadRoutine, threadArgs[i], 0, NULL
        );
        if (threads[i] == NULL) {
            printf("Erro ao criar thread: %lu\n", GetLastError());
            return 1;
        }
    }

    
    WaitForMultipleObjects(N_THREADS, threads, TRUE, INFINITE);

    printf("-------------------------------------------\n");
    printf("Todas as agencias terminaram as operacoes.\n");
    printf("SALDO FINAL (CORRIGIDO): %.2f\n", g_saldoBancario);
    printf("Saldo esperado (correto): 800.00\n");

    
    i = 0;
    for (i; i < N_THREADS; ++i) {
        CloseHandle(threads[i]);
    }
    CloseHandle(hMutex); // Fecha o handle do Mutex

    return 0;
}
