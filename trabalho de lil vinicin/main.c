#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 100
#define MAX_SALAS 10
#define MAX_MEDICOS 20
#define MAX_PACIENTES 50

typedef struct {
    int id;
    char nome[MAX_NOME];
    int idade;
    float peso;
    float altura;
    char sintomas[MAX_NOME];
    int prioridade;
    int faltas;
    int especialidade;
} Paciente;

typedef struct {
    int id;
    char nome[MAX_NOME];
    int especialidade;
    int horasTrabalhadas;
} Medico;

typedef struct {
    int id;
    int ocupado; // 0 = livre, 1 = ocupado
    int horario[5][8]; // 5 dias, 8 horas por dia
} Sala;

// Função para carregar os dados do arquivo de entrada
void carregarDados(const char *nomeArquivo, Paciente *pacientes, Medico *medicos, Sala *salas, int *numPacientes, int *numMedicos, int *numSalas) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo '%s'.\n", nomeArquivo);
        exit(1);
    }

    // Ler pacientes
    fscanf(arquivo, "%d", numPacientes);
    for (int i = 0; i < *numPacientes; i++) {
        fscanf(arquivo, "%d %s %d %f %f %s %d %d %d", &pacientes[i].id, pacientes[i].nome, &pacientes[i].idade,
               &pacientes[i].peso, &pacientes[i].altura, pacientes[i].sintomas, &pacientes[i].prioridade,
               &pacientes[i].faltas, &pacientes[i].especialidade);
    }

    // Ler salas
    fscanf(arquivo, "%d", numSalas);
    for (int i = 0; i < *numSalas; i++) {
        salas[i].id = i;
        salas[i].ocupado = 0;
        memset(salas[i].horario, 0, sizeof(salas[i].horario));
    }

    // Ler médicos
    fscanf(arquivo, "%d", numMedicos);
    for (int i = 0; i < *numMedicos; i++) {
        fscanf(arquivo, "%d %s %d %d", &medicos[i].id, medicos[i].nome, &medicos[i].especialidade, &medicos[i].horasTrabalhadas);
    }

    fclose(arquivo);
}

// Função para agendar consultas
void processarAgendamentos(Paciente *pacientes, Medico *medicos, Sala *salas, int numPacientes, int numMedicos, int numSalas, int *semanas) {
    int pacientesAtendidos = 0;
    *semanas = 0;

    while (pacientesAtendidos < numPacientes) {
        (*semanas)++;
        for (int dia = 0; dia < 5; dia++) { // Segunda a sexta
            for (int hora = 0; hora < 8; hora++) { // 8 horas de trabalho
                for (int s = 0; s < numSalas; s++) {
                    if (!salas[s].ocupado) { // Sala livre
                        for (int p = 0; p < numPacientes; p++) {
                            if (pacientes[p].prioridade > 0 && pacientes[p].faltas < 2) {
                                for (int m = 0; m < numMedicos; m++) {
                                    if (medicos[m].especialidade == pacientes[p].especialidade) {
                                        // Agendar consulta
                                        salas[s].ocupado = 1;
                                        salas[s].horario[dia][hora] = pacientes[p].id;
                                        medicos[m].horasTrabalhadas++;
                                        pacientesAtendidos++;
                                        pacientes[p].prioridade = 0; // Paciente atendido
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        salas[s].ocupado = 0; // Sala liberada
                    }
                }
            }
        }
    }
}

// Função para gerar o relatório
void gerarRelatorio(const Paciente *pacientes, const Medico *medicos, const Sala *salas, int numPacientes, int numMedicos, int numSalas, int semanas) {
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo) {
        printf("Erro ao criar o arquivo 'relatorio.txt'.\n");
        exit(1);
    }

    fprintf(arquivo, "Relatório de Ocupação das Salas:\n\n");
    for (int dia = 0; dia < 5; dia++) {
        for (int hora = 0; hora < 8; hora++) {
            fprintf(arquivo, "Dia %d, Hora %d:\n", dia + 1, hora + 8);
            for (int s = 0; s < numSalas; s++) {
                int pacienteId = salas[s].horario[dia][hora];
                if (pacienteId != 0) {
                    char *nomePaciente = "Desconhecido";
                    for (int p = 0; p < numPacientes; p++) {
                        if (pacientes[p].id == pacienteId) {
                            nomePaciente = (char *)pacientes[p].nome;
                            break;
                        }
                    }
                    fprintf(arquivo, "Sala %d - Paciente %s (ID %d)\n", salas[s].id, nomePaciente, pacienteId);
                }
            }
            fprintf(arquivo, "\n");
        }
    }

    fprintf(arquivo, "\nNúmero de semanas necessárias: %d\n\n", semanas);

    fprintf(arquivo, "Horas Trabalhadas por Médico:\n");
    Medico medicosOrdenados[MAX_MEDICOS];
    memcpy(medicosOrdenados, medicos, sizeof(Medico) * numMedicos);

    // Ordenar médicos por horas trabalhadas (decrescente)
    for (int i = 0; i < numMedicos - 1; i++) {
        for (int j = i + 1; j < numMedicos; j++) {
            if (medicosOrdenados[i].horasTrabalhadas < medicosOrdenados[j].horasTrabalhadas) {
                Medico temp = medicosOrdenados[i];
                medicosOrdenados[i] = medicosOrdenados[j];
                medicosOrdenados[j] = temp;
            }
        }
    }

    for (int i = 0; i < numMedicos; i++) {
        fprintf(arquivo, "Médico %s: %d horas\n", medicosOrdenados[i].nome, medicosOrdenados[i].horasTrabalhadas);
    }

    fclose(arquivo);
    printf("Relatório gerado em 'relatorio.txt'.\n");
}

// Função principal
int main() {
    Paciente pacientes[MAX_PACIENTES];
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];
    int numPacientes, numMedicos, numSalas, semanas;

    carregarDados("entrada.txt", pacientes, medicos, salas, &numPacientes, &numMedicos, &numSalas);
    processarAgendamentos(pacientes, medicos, salas, numPacientes, numMedicos, numSalas, &semanas);
    gerarRelatorio(pacientes, medicos, salas, numPacientes, numMedicos, numSalas, semanas);

    return 0;
}

