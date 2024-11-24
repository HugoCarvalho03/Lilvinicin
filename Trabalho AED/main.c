#include <stdio.h>   // Biblioteca para entrada e sa�da de dados
#include <stdlib.h>  // Biblioteca para fun��es utilit�rias, como aloca��o de mem�ria e controle de processos
#include <string.h>  // Biblioteca para manipula��o de strings
#include <time.h>    // Biblioteca para fun��es relacionadas ao tempo

// Defini��es de constantes
#define MAX_PACIENTES 1000 // N�mero m�ximo de pacientes
#define MAX_MEDICOS 100    // N�mero m�ximo de m�dicos
#define MAX_SALAS 50       // N�mero m�ximo de salas
#define HORAS_POR_DIA 8   // Quantidade de horas de funcionamento por dia
#define DIAS_POR_SEMANA 5  // Dias de funcionamento por semana

// Estrutura para armazenar os dados de um paciente
typedef struct {
    int id;                // Identificador �nico do paciente
    char nome[50];         // Nome do paciente
    int prioridade;        // Prioridade de atendimento
    char especialidade[30];// Especialidade necess�ria
    int retorno;           // Indicador de retorno 1 ,para consulta nos �ltimos 7 dias, 2 para consulta nos �ltimos 14 dias, 3 para consulta nos �ltimos 21 dias;
    int faltas;            // Quantidade de faltas do paciente
} Paciente;

// Estrutura para armazenar os dados de um m�dico
typedef struct {
    int id;                // Identificador �nico do m�dico
    char nome[50];         // Nome do m�dico
    char especialidade[30];// Especialidade do m�dico
    int horasTrabalhadas;  // Quantidade de horas trabalhadas
} Medico;

// Estrutura para armazenar os dados de uma sala
typedef struct {
    int id;                // Identificador �nico da sala
    char especialidade[30];// Especialidade da sala
    int ocupada[HORAS_POR_DIA][DIAS_POR_SEMANA]; // Matriz para indicar ocupa��o por hora e dia
} Sala;

// Estrutura para armazenar os dados de uma consulta
typedef struct {
    int dia;          // Dia da consulta
    int hora;         // Hora da consulta
    int salaId;       // ID da sala onde ser� realizada
    int medicoId;     // ID do m�dico respons�vel
    int pacienteId;   // ID do paciente
} Consulta;

// Declara��o dos arrays globais e contadores
Paciente pacientes[MAX_PACIENTES];    // Lista de pacientes
Medico medicos[MAX_MEDICOS];          // Lista de m�dicos
Sala salas[MAX_SALAS];                // Lista de salas
Consulta consultas[MAX_PACIENTES];    // Lista de consultas alocadas
int totalPacientes = 0, totalMedicos = 0, totalSalas = 0, totalConsultas = 0; // Contadores

// Fun��o para ler os dados do arquivo de entrada
void lerEntrada(const char *arquivoEntrada) {
    FILE *file = fopen(arquivoEntrada, "r"); // Abre o arquivo em modo leitura
    if (!file) {                             // Verifica se houve erro ao abrir o arquivo
        printf("Erro ao abrir o arquivo de entrada.\n");
        exit(1); // Encerra o programa com erro
    }

    // L� o total de pacientes, m�dicos e salas
    fscanf(file, "%d %d %d", &totalPacientes, &totalMedicos, &totalSalas);

    // L� os dados de cada paciente
    for (int i = 0; i < totalPacientes; i++) {
        pacientes[i].id = i + 1; // Define o ID do paciente
        fscanf(file, "%s %d %s %d", pacientes[i].nome, &pacientes[i].prioridade, pacientes[i].especialidade, &pacientes[i].retorno);
        pacientes[i].faltas = 0;  // Inicializa o campo faltas
    }

    // L� os dados de cada m�dico
    for (int i = 0; i < totalMedicos; i++) {
        medicos[i].id = i + 1; // Define o ID do m�dico
        fscanf(file, "%s %s", medicos[i].nome, medicos[i].especialidade); // L� os dados do m�dico
        medicos[i].horasTrabalhadas = 0; // Inicializa as horas trabalhadas
    }

    // L� os dados de cada sala
    for (int i = 0; i < totalSalas; i++) {
        salas[i].id = i + 1; // Define o ID da sala
        fscanf(file, "%s", salas[i].especialidade); // L� a especialidade da sala
        memset(salas[i].ocupada, 0, sizeof(salas[i].ocupada)); // Inicializa a matriz de ocupa��o
    }

    fclose(file); // Fecha o arquivo de entrada
}

// Fun��o para comparar pacientes por prioridade (ordem decrescente)
int compararPacientes(const void *a, const void *b) {
    Paciente *p1 = (Paciente *)a;
    Paciente *p2 = (Paciente *)b;

    // Diferen�a entre os valores de retorno
    int diferencaRetorno = abs(p1->retorno - p2->retorno);

    // Se a diferen�a de retorno for maior que 1, prioriza pelo retorno
    if (diferencaRetorno > 1) {
        return p2->retorno - p1->retorno; // Ordem decrescente de retorno
    }

    // Caso contr�rio, prioriza pela prioridade
    return p2->prioridade - p1->prioridade; // Ordem decrescente de prioridade
}
// Fun��o para alocar consultas
void alocarConsultas() {
    int dia = 0, hora = 0; // Vari�veis para controlar o hor�rio e o dia

    // Itera sobre todos os pacientes
    for (int i = 0; i < totalPacientes; i++) {
        Paciente *p = &pacientes[i]; // Aponta para o paciente atual
        int alocado = 0;            // Flag para verificar se o paciente foi alocado

        // Itera sobre todas as salas
        for (int j = 0; j < totalSalas && !alocado; j++) {
            Sala *s = &salas[j]; // Aponta para a sala atual

            // Verifica se a especialidade da sala � compat�vel
            if (strcmp(p->especialidade, s->especialidade) == 0) {
                // Itera sobre todos os m�dicos
                for (int k = 0; k < totalMedicos; k++) {
                    Medico *m = &medicos[k]; // Aponta para o m�dico atual

                    // Verifica se o m�dico est� dispon�vel e tem a especialidade correta
                    if (strcmp(m->especialidade, p->especialidade) == 0 && !s->ocupada[hora][dia]) {
                        // Cria a consulta e atualiza os dados
                        consultas[totalConsultas++] = (Consulta){dia, hora, s->id, m->id, p->id};
                        m->horasTrabalhadas++;      // Incrementa as horas trabalhadas do m�dico
                        s->ocupada[hora][dia] = 1;  // Marca o hor�rio como ocupado
                        alocado = 1;                // Marca o paciente como alocado
                        break;
                    }
                }
            }
        }

        // Verifica se o paciente n�o p�de ser alocado
        if (!alocado) {
            printf("Erro: Paciente %d (%s) n�o p�de ser alocado.\n", p->id, p->nome);
            exit(1); // Encerra o programa com erro
        }

        // Avan�a para o pr�ximo hor�rio
        hora++;
        if (hora == HORAS_POR_DIA) { // Verifica se todas as horas do dia foram usadas
            hora = 0;
            dia++; // Avan�a para o pr�ximo dia
            if (dia == DIAS_POR_SEMANA) { // Verifica se todos os dias da semana foram usados
                dia = 0;
            }
        }
    }
}

// Fun��o para gerar um relat�rio de consultas e horas trabalhadas
void gerarRelatorio(const char *arquivoSaida) {
    FILE *file = fopen(arquivoSaida, "w"); // Abre o arquivo de sa�da em modo escrita
    if (!file) {                          // Verifica se houve erro ao abrir o arquivo
        printf("Erro ao abrir o arquivo de sa�da.\n");
        exit(1); // Encerra o programa com erro
    }

    // Escreve o relat�rio de consultas
    fprintf(file, "Relat�rio de Consultas:\n");
    for (int i = 0; i < totalConsultas; i++) {
        Consulta *c = &consultas[i]; // Aponta para a consulta atual
        fprintf(file, "Dia %d, Hora %d: Sala %d, M�dico %s, Paciente %s\n",
                c->dia + 1, c->hora + 8, c->salaId,
                medicos[c->medicoId - 1].nome, pacientes[c->pacienteId - 1].nome);
    }

    // Escreve o relat�rio de horas trabalhadas por m�dico
    fprintf(file, "\nHoras Trabalhadas por M�dico:\n");
    for (int i = 0; i < totalMedicos; i++) {
        fprintf(file, "M�dico %d (%s): %d horas\n", medicos[i].id, medicos[i].nome, medicos[i].horasTrabalhadas);
    }

    fclose(file); // Fecha o arquivo de sa�da
}

// Fun��o principal do programa
int main() {
    srand(time(NULL)); // Inicializa o gerador de n�meros aleat�rios com o tempo atual

    lerEntrada("entrada.txt"); // L� os dados do arquivo de entrada

    qsort(pacientes, totalPacientes, sizeof(Paciente), compararPacientes); // Ordena os pacientes por prioridade

    alocarConsultas(); // Aloca as consultas

    gerarRelatorio("saida.txt"); // Gera o relat�rio de sa�da

    printf("Processamento conclu�do. Verifique o arquivo 'saida.txt'.\n");
    return 0; // Retorna 0 para indicar sucesso
}
