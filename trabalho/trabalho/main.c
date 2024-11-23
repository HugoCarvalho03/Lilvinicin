#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PACIENTES 1000
#define MAX_MEDICOS 100
#define MAX_SALAS 50
#define HORAS_POR_DIA 10
#define DIAS_POR_SEMANA 5

typedef struct {
    int id;
    char nome[50];
    int prioridade;
    char especialidade[30];
    int retorno;
    int faltas;
} Paciente;

typedef struct {
    int id;
    char nome[50];
    char especialidade[30];
    int horasTrabalhadas;
} Medico;

typedef struct {
    int id;
    char especialidade[30];
    int ocupada[HORAS_POR_DIA][DIAS_POR_SEMANA];
} Sala;

typedef struct {
    int dia;
    int hora;
    int salaId;
    int medicoId;
    int pacienteId;
} Consulta;

Paciente pacientes[MAX_PACIENTES];
Medico medicos[MAX_MEDICOS];
Sala salas[MAX_SALAS];
Consulta consultas[MAX_PACIENTES];
int totalPacientes = 0, totalMedicos = 0, totalSalas = 0, totalConsultas = 0;

void lerEntrada(const char *arquivoEntrada) {
    FILE *file = fopen(arquivoEntrada, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        exit(1);
    }

    fscanf(file, "%d %d %d", &totalPacientes, &totalMedicos, &totalSalas);

    for (int i = 0; i < totalPacientes; i++) {
        pacientes[i].id = i + 1;
        fscanf(file, "%s %d %s", pacientes[i].nome, &pacientes[i].prioridade, pacientes[i].especialidade);
        pacientes[i].retorno = 0;
        pacientes[i].faltas = 0;
    }

    for (int i = 0; i < totalMedicos; i++) {
        medicos[i].id = i + 1;
        fscanf(file, "%s %s", medicos[i].nome, medicos[i].especialidade);
        medicos[i].horasTrabalhadas = 0;
    }

    for (int i = 0; i < totalSalas; i++) {
        salas[i].id = i + 1;
        fscanf(file, "%s", salas[i].especialidade);
        memset(salas[i].ocupada, 0, sizeof(salas[i].ocupada));
    }

    fclose(file);
}

int compararPacientes(const void *a, const void *b) {
    Paciente *p1 = (Paciente *)a;
    Paciente *p2 = (Paciente *)b;
    return p2->prioridade - p1->prioridade; // Ordem decrescente de prioridade
}

void alocarConsultas() {
    int dia = 0, hora = 0;

    for (int i = 0; i < totalPacientes; i++) {
        Paciente *p = &pacientes[i];
        int alocado = 0;
        for (int j = 0; j < totalSalas && !alocado; j++) {
            Sala *s = &salas[j];
            if (strcmp(p->especialidade, s->especialidade) == 0) {
                for (int k = 0; k < totalMedicos; k++) {
                    Medico *m = &medicos[k];
                    if (strcmp(m->especialidade, p->especialidade) == 0 && !s->ocupada[hora][dia]) {
                        consultas[totalConsultas++] = (Consulta){dia, hora, s->id, m->id, p->id};
                        m->horasTrabalhadas++;
                        s->ocupada[hora][dia] = 1;
                        alocado = 1;
                        break;
                    }
                }
            }
        }

        if (!alocado) {
            printf("Erro: Paciente %d (%s) não pôde ser alocado.\n", p->id, p->nome);
            exit(1);
        }

        hora++;
        if (hora == HORAS_POR_DIA) {
            hora = 0;
            dia++;
            if (dia == DIAS_POR_SEMANA) {
                dia = 0;
            }
        }
    }
}

void gerarRelatorio(const char *arquivoSaida) {
    FILE *file = fopen(arquivoSaida, "w");
    if (!file) {
        printf("Erro ao abrir o arquivo de saída.\n");
        exit(1);
    }

    fprintf(file, "Relatório de Consultas:\n");
    for (int i = 0; i < totalConsultas; i++) {
        Consulta *c = &consultas[i];
        fprintf(file, "Dia %d, Hora %d: Sala %d, Médico %s, Paciente %s\n",
                c->dia + 1, c->hora + 8, c->salaId,
                medicos[c->medicoId - 1].nome, pacientes[c->pacienteId - 1].nome);
    }

    fprintf(file, "\nHoras Trabalhadas por Médico:\n");
    for (int i = 0; i < totalMedicos; i++) {
        fprintf(file, "Médico %d (%s): %d horas\n", medicos[i].id, medicos[i].nome, medicos[i].horasTrabalhadas);
    }

    fclose(file);
}

int main() {
    srand(time(NULL));

    lerEntrada("entrada.txt");

    qsort(pacientes, totalPacientes, sizeof(Paciente), compararPacientes);

    alocarConsultas();

    gerarRelatorio("saida.txt");

    printf("Processamento concluído. Verifique o arquivo 'saida.txt'.\n");
    return 0;
}

