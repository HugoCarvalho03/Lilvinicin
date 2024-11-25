# Formato do Arquivo de Entrada

Este projeto requer um arquivo de entrada contendo informações sobre **Pacientes**, **Salas**, **Especialidades** e **Médicos**. O arquivo deve seguir o formato especificado abaixo para garantir que os dados sejam lidos corretamente pelo programa.

## Estrutura do Arquivo de Entrada

O arquivo de entrada deve ser composto por 4 seções, listadas na ordem a seguir:

1. **Pacientes**
2. **Salas**
3. **Especialidades**
4. **Médicos**

Cada seção contém dados separados por espaços e segue uma estrutura específica, conforme detalhado abaixo.

### 1. *Pacientes*

Cada paciente é representado por uma linha no arquivo com os seguintes dados:

- **numero_de_paciente**: Um número único que identifica o paciente.
- **nome_do_paciente**: O nome do paciente.
- **idade**: A idade do paciente.
- **altura**: A altura do paciente em metros.
- **peso**: O peso do paciente em quilos.
- **sintoma**: O sintoma apresentado pelo paciente.
- **prioridade**: Um número que representa a prioridade do atendimento (quanto menor o número, maior a prioridade).
- **especialidadeId**: O ID da especialidade que o paciente precisa.

**Exemplo:**

```txt
1 Francio 25 1.80 75 Dor_no_peito 2 1
2 Zé_do_Caixão 30 1.60 65 Febre_alta 3 1
3 Clebilso 20 1.50 55 Dor_no_corpo 1 1
4 Andre_Pinga 20 1.50 55 Dor_no_corpo 7 2
7 Russin 20 1.50 55 Dor_no_corpo 4 1
8 PV 20 1.50 55 Dor_no_corpo 8 2
9 Cicin 20 1.50 55 Dor_no_corpo 9 1
10 Bahias 20 1.50 55 Dor_no_corpo 13 1
11 Paulão 20 1.50 55 Dor_no_corpo 11 3
12 Paçoca 21 1.70 65 Dor_na_coluna 15 2
```
### 2. *Salas*

A seção de **Salas** descreve as salas onde as consultas podem ocorrer. Cada linha da seção deve conter as seguintes informações:

- **numero_de_sala**: Um número único que identifica a sala.
- **nome_da_sala**: O nome da sala.

Cada sala é descrita por uma linha de dados. O número da sala deve ser único, e o nome da sala deve ser representado como uma string de texto.

#### Exemplo:

```txt
1 Sala_A
2 Sala_B
````

### 3. *Especialidades*

No arquivo de entrada, as especialidades devem ser listadas da seguinte forma:

Cada linha representará uma especialidade, seguindo o formato:

- `numero_de_especialidade`: Um identificador único para a especialidade (um número inteiro).
- `especialidade`: O nome da especialidade médica.

### Exemplo de entrada de especialidades:

```txt
Especialidades:
1 Cardiologia
2 Neurologia
3 Pediatria
```

### 4. *Médicos*

No arquivo de entrada, os médicos devem ser listados da seguinte forma:

- `numero_de_medico`: Um identificador único para o médico (um número inteiro).
- `nome_do_medico`: O nome do médico.
- `especialidade`: O nome da especialidade à qual o médico pertence.
- `id_do_medico`: O identificador único do médico (um número inteiro).
- `especialidadeId`: O identificador da especialidade que o médico atende.

### Exemplo de entrada de médicos:

```txt
Médicos:
1 Dr_Vinicin Cardiologia 1 1
2 Dr_Silva Neurologia 2 2
3 Dr_Marta Pediatria 3 3
4 Dr_Alves Ortopedia 4 4
```








