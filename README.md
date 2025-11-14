# 🛡️ Projeto: Sincronização em Sistemas Concorrentes (Cofre Bancário)

Este projeto em linguagem **C** é um estudo de caso prático focado na disciplina de **Sistemas Operacionais** ou **Programação Concorrente**. Ele demonstra a vulnerabilidade de **recursos compartilhados** em um ambiente multithreaded e a aplicação de mecanismos de sincronização para garantir a **integridade dos dados**.

---

## 1. Resumo Executivo

O projeto simula um sistema de **transações bancárias** onde múltiplas **threads** (representando agências) acessam e modificam um **saldo centralizado** (`g_saldoBancario`).

O código inicial é deliberadamente introduzido com uma **Condição de Corrida (Race Condition)**. O objetivo é a implementação e verificação de um **Mutex (Mutual Exclusion)**, utilizando a API do **Windows**, como solução robusta para proteger a **Seção Crítica** e assegurar a acuracidade das operações financeiras.

---

## 2. Análise do Problema: Condição de Corrida

A instabilidade do sistema é causada pelo fluxo de execução **não atômico** nas funções transacionais (`operacaoDeposito` e `operacaoSaque`), que consiste em:

$$\text{Lê} \rightarrow \text{Processa (Sleep)} \rightarrow \text{Escreve}$$

* A inclusão da função `Sleep()` simula a **latência** inerente ao processamento real.
* O **scheduler** do sistema operacional pode preempcionar uma thread após a leitura do saldo, permitindo que outra thread execute uma transação e atualize o valor.
* Ao retornar, a thread original sobrescreve o dado atualizado com seu **valor obsoleto**, resultando em uma perda irrecuperável de transações e **divergência no saldo final**.

---

## 3. Solução Técnica: Aplicação de Mutex

A solução implementada utiliza o **Mutex**, um objeto de sincronização de núcleo (**kernel object**), para impor a **exclusão mútua** na **Seção Crítica**, garantindo que apenas **uma thread** possa modificar o recurso compartilhado por vez.

| Ação | Função Windows API | Descrição |
| :--- | :--- | :--- |
| **Inicialização** | `CreateMutex()` | Cria e inicializa o objeto Mutex (`hMutex`). |
| **Aquisição/Espera** | `WaitForSingleObject()` | Bloqueia a thread solicitante até que o Mutex seja liberado por outra thread (espera ocupada). |
| **Liberação** | `ReleaseMutex()` | Libera a posse do Mutex, permitindo que a próxima thread em espera prossiga. |

---

## 4. Estrutura e Dados

O projeto é construído em **C**, utilizando a API de threading e sincronização do **Windows**.

| Variável/Estrutura | Tipo | Descrição |
| :--- | :--- | :--- |
| `g_saldoBancario` | `double` | O recurso **compartilhado** e **vulnerável**. |
| `hMutex` | `HANDLE` | O objeto **Mutex** para controle de acesso. |
| `ThreadArgs` | `struct` | Estrutura para passagem de parâmetros (nome, tipo, valor) às threads. |

### 4.1. Verificação de Integridade

Com base nas transações configuradas, o programa verifica se a sincronização foi bem-sucedida comparando o resultado final com o valor esperado.

* **Saldo Inicial:** $1000.00$
* **Total de Transações:** $+500.00 - 300.00 + 200.00 - 700.00 + 100.00$
* **Saldo Final Esperado:** **$800.00**

---

## 5. Compilação e Execução

Para compilar e executar este projeto:

* **Requisito:** Compilador C compatível com a API do Windows (e.g., **MinGW**).

### Compilação:

```bash
gcc trabalho_c.c -o cofre.exe -std=c99
Nota: Adicionei -std=c99 (ou similar) por boa prática, caso utilize features mais recentes do C.

Execução:
Bash

./cofre.exe
O output da execução demonstrará o escalonamento das threads e, por fim, o saldo final validado.
