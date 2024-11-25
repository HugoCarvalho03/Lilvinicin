#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Constantes
#define MAX_PACIENTES 1000
#define MAX_SALAS 100
#define MAX_ESPECIALIDADES 50
#define MAX_MEDICOS 200
#define MAX_CONSULTAS 5000
// Estruturas
typedef struct {
    int id;
    char nome[50];
    int idade;
    float altura;
    float peso;
    char sintomas[100];
    int prioridade;
    int especialidade;
    int falta; //0 para se o paciente não faltar, 1 para se faltar
    int alocado;// 0  para se o paciente for alocado , 1 para não
} Paciente;

typedef struct {
    int id;
    char nome[50];
    int especialidadeId;
    int horasTrabalhadas;
} Medico;

typedef struct {
    int id;
    char nome[50];
} Sala;

typedef struct {
    int pacienteId;
    int medicoId;
    int salaId;
    int horario;
    int retorno;
} Consulta;

void lerDados(Paciente *pacientes, int *numPacientes, Medico *medicos, int *numMedicos, Sala *salas, int *numSalas) {
    FILE *arquivo = fopen("entrada.txt", "r"); // recebe arquivo de entrada
    if (!arquivo) {//caso não seja possível abrir, mostra código de erro
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }
    char linha[256];
    // lendo pacientes do arquivo
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Salas:", 6) == 0) break;//lê até o cabeçalho de salas
        if (*numPacientes >= MAX_PACIENTES) {
            printf("Número máximo de pacientes excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d %f %f %s %d %d",&pacientes[*numPacientes].id,pacientes[*numPacientes].nome,&pacientes[*numPacientes].idade,&pacientes[*numPacientes].altura,&pacientes[*numPacientes].peso,pacientes[*numPacientes].sintomas,&pacientes[*numPacientes].prioridade,&pacientes[*numPacientes].especialidade);
        (*numPacientes)++;// lê os dados dos pacientes e aumenta o número de pacientes
    }
    // lendo salas
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Especialidades:", 15) == 0) break; // próxima seção
        if (*numSalas >= MAX_SALAS) {// se o numero de salas atingir o máximo, mostra aviso
            printf("Número máximo de salas excedido.\n");
            break;
        }
        sscanf(linha, "%d %s",&salas[*numSalas].id,salas[*numSalas].nome);//escaneia os dados das salas
        (*numSalas)++;//aumenta o numero de salas
    }
    // lendo especialidades
    int numEspecialidades = 0;
    char especialidades[MAX_ESPECIALIDADES][50];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Médicos:", 8) == 0) break; // próxima seção
        if (numEspecialidades >= MAX_ESPECIALIDADES) {
            printf("Número máximo de especialidades excedido.\n");
            break;
        }
        sscanf(linha, "%d %s",&numEspecialidades,especialidades[numEspecialidades]);//escaneia os dados das especialidades
        numEspecialidades++;//aumenta o numero de especialidades
    }
    // lendo médicos
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (*numMedicos >= MAX_MEDICOS) {
            printf("Número máximo de médicos excedido.\n");
            break;
        }
        char especialidade[50];
        sscanf(linha, "%d %s %s %d", &medicos[(*numMedicos)].id, medicos[(*numMedicos)].nome, especialidade, &medicos[(*numMedicos)].especialidadeId);//escaneia os dados dos médicos
        medicos[*numMedicos].horasTrabalhadas = 0; // inicializa horas trabalhadas
        (*numMedicos)++;//aumenta o número de médicos
    }
    fclose(arquivo);
}
// função para alocar consultas
void alocarConsultas(Paciente *pacientes, int numPacientes, Medico *medicos, int numMedicos,Sala *salas, int numSalas, Consulta *consultas, int *numConsultas) {
    int horariosMedicos[MAX_MEDICOS][5][8] = {0};
    int horariosSalas[MAX_SALAS][5][8] = {0};
    srand(time(NULL));
    // percorre cada paciente
    for (int i = 0; i < numPacientes; i++) {
        if (*numConsultas >= MAX_CONSULTAS) break; // limita o número de consultas.
        // verifica se o paciente vai faltar (5% de chance).
        if (rand() % 100 < 5) {
            pacientes[i].falta=1;//altera o controlador de falta para 1
            continue; // pule para o próximo paciente.
        }
        int consultaAlocada = 0;//inicializa variavel
        // aloca consulta para o paciente
        for (int dia = 0; dia < 5 && !consultaAlocada; dia++) {
            for (int h = 0; h < 8 && !consultaAlocada; h++) {
                for (int m = 0; m < numMedicos && !consultaAlocada; m++) {
                    if (medicos[m].especialidadeId != pacientes[i].especialidade) {
                        continue; // verifica a especialidade, se for compatível aloca a consulta
                    }
                    for (int s = 0; s < numSalas && !consultaAlocada; s++) {//preenche a consulta com os dados
                        if (horariosMedicos[m][dia][h] == 0 && horariosSalas[s][dia][h] == 0) {
                            consultas[*numConsultas] = (Consulta){
                                .pacienteId = pacientes[i].id,
                                .medicoId = medicos[m].id,
                                .salaId = salas[s].id,
                                .horario = (dia * 8 + h + 8),
                                .retorno = 1
                            };
                            horariosMedicos[m][dia][h] = 1;
                            horariosSalas[s][dia][h] = 1;
                            // incrementa as horas trabalhadas do médico.
                            medicos[m].horasTrabalhadas++;
                            // incrementa o número de consultas alocadas.
                            (*numConsultas)++;
                            consultaAlocada = 1; // Consulta foi alocada.
                        }
                    }
                }
            }
        }
        // Caso não seja possível alocar a consulta.
        if (!consultaAlocada) {//caso não seja alocada mostra mensagem no relatorio
            pacientes[i].alocado = 1;
            printf("Erro: Não foi possível alocar consulta para o paciente %d.\n", pacientes[i].id);
        }
    }
}

void gerenciarRetornos(Consulta *consultas, int *numConsultas, int maxConsultas) {// função para marcar os retornos das consultas dos pacientes
    srand(time(NULL));
    int horasPorDia = 8;
    for (int i = 0; i < *numConsultas; i++) {
        if (consultas[i].retorno == 1 && *numConsultas < maxConsultas) {
            Consulta novaConsulta = consultas[i];
            int diasExtras = 1+(rand() % 29 );
            novaConsulta.horario += diasExtras * horasPorDia;
            novaConsulta.retorno = 0;
            consultas[*numConsultas] = novaConsulta;
            (*numConsultas)++;
        }
    }
}
// função para gerar relatório
void gerarRelatorio(Consulta *consultas, int numConsultas, Medico *medicos, int numMedicos, Paciente *pacientes, int numPacientes, Sala *salas, int numSalas) {
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo para salvar o relatório.\n");
        return;
    }
    // ordena as consultas por horário
    for (int i = 0; i < numConsultas - 1; i++) {
        for (int j = i + 1; j < numConsultas; j++) {
            if (consultas[i].horario > consultas[j].horario) {
                Consulta temp = consultas[i];
                consultas[i] = consultas[j];
                consultas[j] = temp;
            }
        }
    }
    // inicializa contadores
    int atendimentosPorSalaDia[MAX_SALAS][5] = {0};
    int totalHorasTrabalhadas = 0;
    int horasTrabalhadasPorMedico[MAX_MEDICOS] = {0};
    int ultimoDia = 0;
    for (int i = 0; i < numConsultas; i++) {
        // Recupera os dados associados à consulta
        char nomePaciente[50] = "Desconhecido";
        char nomeMedico[50] = "Desconhecido";
        char nomeSala[50] = "Desconhecida";
        for (int p = 0; p < numPacientes; p++) {
            if (pacientes[p].id == consultas[i].pacienteId) {
                strcpy(nomePaciente, pacientes[p].nome);
                break;
            }
        }
        for (int m = 0; m < numMedicos; m++) {
            if (medicos[m].id == consultas[i].medicoId) {
                strcpy(nomeMedico, medicos[m].nome);
                horasTrabalhadasPorMedico[m]++;
                break;
            }
        }
        for (int s = 0; s < numSalas; s++) {
            if (salas[s].id == consultas[i].salaId) {
                strcpy(nomeSala, salas[s].nome);
                break;
            }
        }
        // determina o horário e dia da consulta
        int dia = (consultas[i].horario - 8) / 8;
        int hora = (consultas[i].horario - 8) % 8 + 8;
        if (dia + 1 > ultimoDia) {
            ultimoDia = dia + 1;
        }

        // identifica se é retorno ou primeira consulta
        const char *tipoConsulta = consultas[i].retorno == 0 ? "Retorno" : "Primeira Consulta";
        totalHorasTrabalhadas++;
        for (int s = 0; s < numSalas; s++) {
            if (consultas[i].salaId == salas[s].id) {
                atendimentosPorSalaDia[s][dia]++;
                break;
            }
        }
        // escreve detalhes no arquivo
        fprintf(arquivo, "Consulta %d: Paciente: %s (ID %d), Médico: %s (ID %d), Sala: %s (ID %d), Horário: Dia %d, %02dh, Tipo: %s\n",i + 1,nomePaciente, consultas[i].pacienteId,nomeMedico, consultas[i].medicoId,nomeSala, consultas[i].salaId,dia + 1, hora,tipoConsulta);
        fprintf(arquivo,"\n");
    }
    // resumo de horas trabalhadas por médico
    fprintf(arquivo, "Horas trabalhadas:\n");
    for (int m = 0; m < numMedicos; m++) {
        fprintf(arquivo, "- Médico %s (ID %d): %d \n",medicos[m].nome, medicos[m].id, horasTrabalhadasPorMedico[m]);
    }
    // resumo das faltas
    fprintf(arquivo,"Faltas \n");
    int countfault = 0;
    for (int i = 0; i < numPacientes; i++){
        if (pacientes[i].falta == 1){
            countfault = countfault+1;
            fprintf(arquivo,"Paciente %s \n",pacientes[i].nome);
    }
    }
    if (countfault == 0){
        fprintf(arquivo,"Não Houve Faltas \n");
    }
    //resumo das não alocações
    for (int i = 0; i < numPacientes; i++){
            if (pacientes[i].alocado == 1){
                fprintf(arquivo,"Paciente %s não alocado por não ter especialidade. \n", pacientes[i].nome);
            }
    }
    fclose(arquivo);
}

int main() {
    Paciente pacientes[MAX_PACIENTES];
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];
    Consulta consultas[MAX_CONSULTAS];
    int numPacientes = 0, numMedicos = 0, numSalas = 0, numConsultas = 0;
    lerDados(pacientes, &numPacientes, medicos, &numMedicos, salas, &numSalas);
    alocarConsultas(pacientes, numPacientes, medicos, numMedicos, salas, numSalas, consultas, &numConsultas);
    gerenciarRetornos(consultas, &numConsultas, MAX_CONSULTAS);
    gerarRelatorio(consultas, numConsultas, medicos, numMedicos, pacientes, numPacientes, salas, numSalas);
    return 0;
}

