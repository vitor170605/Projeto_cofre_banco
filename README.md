# Projeto_cofre_banco
Projeto em C simula operações bancárias concorrentes  com múltiplas threads. O objetivo é corrigir a Condição de Corrida  no acesso ao saldo global. A solução usa um Mutex (Windows API) para proteger a Seção Crítica. Isso garante o acesso exclusivo ao saldo, alcançando o resultado final esperado de 800.00
