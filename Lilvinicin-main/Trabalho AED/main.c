#include <stdio.h>
#include <string.h>

#define MAX_PACIENTES 100
#define MAX_MEDICOS 50
#define MAX_SALAS 20
#define MAX_CONSULTAS 100

// Estruturas
typedef struct {
    int id;
    char nome[50];
    char especialidade[30];
} Medico;

typedef struct {
    int id;
    char nome[50];
} Sala;

typedef struct {
    int id;
    char nome[50];
    char especialidade[30];
    int retorno; // 0: Não é retorno, 1: Retorno
} Paciente;

typedef struct {
    int pacienteId;
    int medicoId;
    int salaId;
    int dia;
    int hora;
} Consulta;

// Variáveis globais
Paciente pacientes[MAX_PACIENTES];
Medico medicos[MAX_MEDICOS];
Sala salas[MAX_SALAS];
Consulta consultas[MAX_CONSULTAS];

int totalPacientes = 0, totalMedicos = 0, totalSalas = 0, totalConsultas = 0;
int agenda[MAX_MEDICOS][7][8] = {0}; // Agenda: médico x dias da semana x horários
int salasOcupadas[MAX_SALAS][7][8] = {0}; // Salas: sala x dias da semana x horários

// Função para alocar pacientes
void alocarPacientes() {
    for (int i = 0; i < totalPacientes; i++) {
        Paciente *p = &pacientes[i];
        int alocado = 0;

        for (int j = 0; j < totalMedicos && !alocado; j++) {
            Medico *m = &medicos[j];

            // Verificar especialidade
            if (strcmp(p->especialidade, m->especialidade) != 0)
                continue;

            for (int dia = 0; dia < 7 && !alocado; dia++) {
                for (int hora = 0; hora < 8 && !alocado; hora++) {
                    if (!agenda[m->id][dia][hora]) {
                        for (int k = 0; k < totalSalas; k++) {
                            if (!salasOcupadas[salas[k].id][dia][hora]) {
                                // Alocar consulta
                                consultas[totalConsultas++] = (Consulta){p->id, m->id, salas[k].id, dia, hora};
                                agenda[m->id][dia][hora] = 1;
                                salasOcupadas[salas[k].id][dia][hora] = 1;
                                alocado = 1;
                                break;
                            }
                        }
                    }
                }
            }

            // Se não foi alocado, tenta o próximo horário disponível
            if (!alocado) {
                for (int dia = 0; dia < 7 && !alocado; dia++) {
                    for (int hora = 1; hora < 8 && !alocado; hora++) {
                        if (!agenda[m->id][dia][hora]) {
                            for (int k = 0; k < totalSalas; k++) {
                                if (!salasOcupadas[salas[k].id][dia][hora]) {
                                    consultas[totalConsultas++] = (Consulta){p->id, m->id, salas[k].id, dia, hora};
                                    agenda[m->id][dia][hora] = 1;
                                    salasOcupadas[salas[k].id][dia][hora] = 1;
                                    alocado = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Se não foi alocado, registrar erro
        if (!alocado) {
            fprintf(stderr, "ERRO: Paciente %d (%s) não pôde ser alocado.\n", p->id, p->nome);
            fprintf(stderr, "  - Motivo: Nenhum médico ou sala disponível para a especialidade %s.\n", p->especialidade);
        }
    }
}

// Função para calcular as horas trabalhadas de cada médico
void calcularHorasTrabalhadas() {
    int horasTrabalhadas[MAX_MEDICOS] = {0}; // Inicializa a contagem de horas para cada médico

    for (int i = 0; i < totalConsultas; i++) {
        Consulta c = consultas[i];
        horasTrabalhadas[c.medicoId]++;
    }

    // Exibir as horas trabalhadas de cada médico
    printf("\nHORAS TRABALHADAS POR CADA MÉDICO:\n");
    for (int i = 0; i < totalMedicos; i++) {
        printf("Médico: %s (ID: %d)\n", medicos[i].nome, medicos[i].id);
        printf("  - Horas trabalhadas: %d\n\n", horasTrabalhadas[i]);
    }
}

// Função para imprimir a saída em um arquivo
void imprimirSaida() {
    FILE *file = fopen("saida.txt", "w"); // Abre o arquivo para escrita (modo 'w' sobrescreve o arquivo)
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para escrita!\n");
        return;
    }

    // Escreve as consultas alocadas
    fprintf(file, "\nCONSULTAS ALOCADAS:\n");
    for (int i = 0; i < totalConsultas; i++) {
        Consulta c = consultas[i];
        Paciente *p = &pacientes[c.pacienteId];
        Medico *m = &medicos[c.medicoId];
        Sala *s = &salas[c.salaId];

        const char *diasSemana[] = {"Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado", "Domingo"};
        fprintf(file, "Paciente: %s (ID: %d)\n", p->nome, p->id);
        fprintf(file, "  - Especialidade: %s\n", p->especialidade);
        fprintf(file, "  - Médico: %s (ID: %d)\n", m->nome, m->id);
        fprintf(file, "  - Sala: %s (ID: %d)\n", s->nome, s->id);
        fprintf(file, "  - Dia: %s\n", diasSemana[c.dia]);
        fprintf(file, "  - Hora: %dh\n\n", c.hora + 8); // Assume que as consultas começam às 8h
    }

    // Escreve os pacientes não alocados
    fprintf(file, "\nPACIENTES NÃO ALOCADOS:\n");
    for (int i = 0; i < totalPacientes; i++) {
        Paciente *p = &pacientes[i];
        int alocado = 0;

        for (int j = 0; j < totalConsultas; j++) {
            if (consultas[j].pacienteId == p->id) {
                alocado = 1;
                break;
            }
        }

        if (!alocado) {
            fprintf(file, "ERRO: Paciente %d (%s) não pôde ser alocado.\n", p->id, p->nome);
            fprintf(file, "  - Motivo: Nenhum médico ou sala disponível para a especialidade %s.\n", p->especialidade);
        }
    }

    // Escreve as horas trabalhadas de cada médico
    fprintf(file, "\nHORAS TRABALHADAS POR CADA MÉDICO:\n");
    int horasTrabalhadas[MAX_MEDICOS] = {0};
    for (int i = 0; i < totalConsultas; i++) {
        Consulta c = consultas[i];
        horasTrabalhadas[c.medicoId]++;
    }
    for (int i = 0; i < totalMedicos; i++) {
        fprintf(file, "Médico: %s (ID: %d)\n", medicos[i].nome, medicos[i].id);
        fprintf(file, "  - Horas trabalhadas: %d\n\n", horasTrabalhadas[i]);
    }

    fclose(file); // Fecha o arquivo após escrever
}

// Função principal
int main() {
    // Exemplo de dados de entrada
    medicos[totalMedicos++] = (Medico){0, "Dr. Silva", "Cardiologia"};
    medicos[totalMedicos++] = (Medico){1, "Dra. Clara", "Dermatologia"};

    salas[totalSalas++] = (Sala){0, "Sala 101"};
    salas[totalSalas++] = (Sala){1, "Sala 102"};

    pacientes[totalPacientes++] = (Paciente){0, "João", "Cardiologia", 0};
    pacientes[totalPacientes++] = (Paciente){1, "Maria", "Dermatologia", 0};
    pacientes[totalPacientes++] = (Paciente){2, "Beatriz", "Pediatria", 0};
    pacientes[totalPacientes++] = (Paciente){3, "Lucas", "Cardiologia", 0};

    // Processa a alocação dos pacientes
    alocarPacientes();

    // Calcula e imprime as horas trabalhadas
    calcularHorasTrabalhadas();

    // Imprime a saída no arquivo
    imprimirSaida();

    printf("Relatório gerado no arquivo 'saida.txt'.\n");

    return 0;
}
