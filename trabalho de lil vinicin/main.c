#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 100
#define MAX_SINTOMAS 200
#define MAX_SALAS 10
#define MAX_MEDICOS 20
#define MAX_PACIENTES 50

typedef struct {
    int id;
    char nome[MAX_NOME];
    int idade;
    float peso;
    float altura;
    char sintomas[MAX_SINTOMAS];
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
    int ocupada;
    int horario[5][8];
} Sala;

// Função para carregar os dados do arquivo de entrada
void carregarDados(const char *nomeArquivo, Paciente *pacientes, Medico *medicos, Sala *salas, int *numPacientes, int *numSalas, int *numMedicos) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fscanf(arquivo, "%d", numPacientes);
    for (int i = 0; i < *numPacientes; i++) {
        Paciente p;
        fscanf(arquivo, "%d %s %d %f %f %s %d %d %d", &p.id, p.nome, &p.idade, &p.peso, &p.altura, p.sintomas, &p.prioridade, &p.faltas, &p.especialidade);
        pacientes[i] = p;
    }

    fscanf(arquivo, "%d", numSalas);
    for (int i = 0; i < *numSalas; i++) {
        Sala s = {i, 0, {{0}}};
        salas[i] = s;
    }

    fscanf(arquivo, "%d", numMedicos);
    for (int i = 0; i < *numMedicos; i++) {
        Medico m;
        fscanf(arquivo, "%d %s %d %d", &m.id, m.nome, &m.especialidade, &m.horasTrabalhadas);
        medicos[i] = m;
    }

    fclose(arquivo);
}

void processarAgendamentos(Paciente *pacientes, Medico *medicos, Sala *salas, int numPacientes, int numMedicos, int numSalas) {
    for (int dia = 0; dia < 5; dia++) { // Segunda a sexta
        for (int hora = 0; hora < 8; hora++) { // Horário comercial, das 8h às 16h
            for (int i = 0; i < numPacientes; i++) {
                if (pacientes[i].prioridade > 0 && pacientes[i].faltas < 2) {
                    for (int j = 0; j < numSalas; j++) {
                        if (salas[j].ocupada == 0) { // Sala disponível
                            int medIndex = pacientes[i].especialidade % numMedicos; // Escolhe médico baseado na especialidade
                            if (medicos[medIndex].especialidade == pacientes[i].especialidade) {
                                // Agendamento
                                salas[j].ocupada = 1;
                                salas[j].horario[dia][hora] = pacientes[i].id;
                                medicos[medIndex].horasTrabalhadas++;
                                printf("Paciente %s agendado com médico %s na sala %d, dia %d, hora %d\n", pacientes[i].nome, medicos[medIndex].nome, salas[j].id, dia + 1, hora + 8);

                                // Simulação de falta (opcional)
                                if (rand() % 100 < 5) { // 5% chance de faltar
                                    pacientes[i].faltas++;
                                    if (pacientes[i].faltas == 2) {
                                        printf("Paciente %s removido após faltar duas vezes\n", pacientes[i].nome);
                                    } else {
                                        pacientes[i].prioridade--;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void gerarRelatorio(Paciente *pacientes, Medico *medicos, Sala *salas, int numPacientes, int numMedicos, int numSalas) {
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de relatório.\n");
        return;
    }

    fprintf(arquivo, "Relatório de Ocupação das Salas:\n\n");
    for (int dia = 0; dia < 5; dia++) {
        for (int hora = 0; hora < 8; hora++) {
            fprintf(arquivo, "Dia %d, Hora %d:\n", dia + 1, hora + 8);
            for (int i = 0; i < numSalas; i++) {
                int pacienteId = salas[i].horario[dia][hora];
                if (pacienteId != 0) { // Verifica se há um paciente agendado
                    // Procura o nome do paciente pelo ID
                    char *nomePaciente = "Desconhecido";
                    for (int j = 0; j < numPacientes; j++) {
                        if (pacientes[j].id == pacienteId) {
                            nomePaciente = pacientes[j].nome;
                            break;
                        }
                    }
                    fprintf(arquivo, "Sala %d - Paciente %s (ID %d)\n", salas[i].id, nomePaciente, pacienteId);
                }
            }
            fprintf(arquivo, "\n");
        }
    }

    fprintf(arquivo, "\nHoras Trabalhadas por Médico:\n\n");
    for (int i = 0; i < numMedicos; i++) {
        fprintf(arquivo, "Médico %s: %d horas\n", medicos[i].nome, medicos[i].horasTrabalhadas);
    }

    fclose(arquivo);
}




int main() {
    srand(time(NULL));  // Semente para simulação das faltas

    Paciente pacientes[MAX_PACIENTES];
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];

    int numPacientes, numSalas, numMedicos;

    carregarDados("C:\\Users\\Hugo Carvalho\\Documents\\so cadeira\\output\\entrada.txt", pacientes, medicos, salas, &numPacientes, &numSalas, &numMedicos);
    processarAgendamentos(pacientes, medicos, salas, numPacientes, numMedicos, numSalas);
    gerarRelatorio(pacientes, medicos, salas, numPacientes, numMedicos, numSalas);

    printf("Processo concluído. Relatório gerado em 'relatorio.txt'.\n");
    return 0;
}
