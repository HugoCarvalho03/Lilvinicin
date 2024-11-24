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
    int idade;
    float peso;
    float altura;
    char telefone[15];
    char sintomas[100];
    char medicacoes[100];
    int retorno; // Flag para indicar consulta de retorno
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
int semanasNecessarias = 0;

void lerEntrada(const char *arquivoEntrada) {
    FILE *file = fopen(arquivoEntrada, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        exit(1);
    }

    fscanf(file, "%d %d %d", &totalPacientes, &totalMedicos, &totalSalas);

    for (int i = 0; i < totalPacientes; i++) {
        pacientes[i].id = i + 1;
        fscanf(file, "%s %d %s %d %f %f %s %s %s", 
               pacientes[i].nome, &pacientes[i].prioridade, pacientes[i].especialidade,
               &pacientes[i].idade, &pacientes[i].peso, &pacientes[i].altura,
               pacientes[i].telefone, pacientes[i].sintomas, pacientes[i].medicacoes);
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
    return p2->prioridade - p1->prioridade;
}

void alocarConsultas() {
    int dia = 0, hora = 0;

    while (totalPacientes > 0) {
        for (int i = 0; i < totalPacientes; ) {
            Paciente *p = &pacientes[i];
            int alocado = 0;

            for (int j = 0; j < totalSalas && !alocado; j++) {
                Sala *s = &salas[j];
                if (strcmp(p->especialidade, s->especialidade) == 0) {
                    for (int k = 0; k < totalMedicos; k++) {
                        Medico *m = &medicos[k];
                        if (strcmp(m->especialidade, p->especialidade) == 0 && !s->ocupada[hora][dia]) {
                            if ((rand() % 100) < 5) { // 5% de chance de falta
                                p->faltas++;
                                printf("Paciente %s faltou à consulta.\n", p->nome);
                                if (p->faltas >= 2) {
                                    printf("Paciente %s removido por excesso de faltas.\n", p->nome);
                                    memmove(&pacientes[i], &pacientes[i + 1], 
                                            (totalPacientes - i - 1) * sizeof(Paciente));
                                    totalPacientes--;
                                    alocado = 1; // Pula o processamento deste índice
                                    continue;
                                } else {
                                    p->prioridade = p->prioridade > 1 ? p->prioridade - 1 : 1;
                                    break;
                                }
                            }

                            consultas[totalConsultas++] = (Consulta){dia, hora, s->id, m->id, p->id};
                            m->horasTrabalhadas++;
                            s->ocupada[hora][dia] = 1;
                            alocado = 1;

                            if (p->retorno) {
                                p->retorno = 0;
                            } else {
                                p->retorno = 1;
                            }
                            break;
                        }
                    }
                }
            }

            if (!alocado) {
                i++; // Apenas avança se o paciente não foi removido
            }
        }

        hora++;
        if (hora == HORAS_POR_DIA) {
            hora = 0;
            dia++;
            if (dia == DIAS_POR_SEMANA) {
                dia = 0;
                semanasNecessarias++;
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
        fprintf(file, "Semana %d, Dia %d, Hora %d: Sala %d, Médico %s, Paciente %s\n",
                (c->dia / DIAS_POR_SEMANA) + 1, (c->dia % DIAS_POR_SEMANA) + 1, c->hora + 8, c->salaId,
                medicos[c->medicoId - 1].nome, pacientes[c->pacienteId - 1].nome);
    }

    fprintf(file, "\nHoras Trabalhadas por Médico:\n");
    for (int i = 0; i < totalMedicos; i++) {
        fprintf(file, "Médico %d (%s): %d horas\n", medicos[i].id, medicos[i].nome, medicos[i].horasTrabalhadas);
    }

    fprintf(file, "\nNúmero total de semanas necessárias: %d\n", semanasNecessarias);

    fprintf(file, "\nPacientes Removidos por Faltas:\n");
    for (int i = 0; i < totalPacientes; i++) {
        if (pacientes[i].faltas >= 2) {
            fprintf(file, "Paciente %s foi removido por excesso de faltas.\n", pacientes[i].nome);
        }
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
