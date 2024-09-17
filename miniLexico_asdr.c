/*
Implementacao do mini analisador lexico
Compile com:
gcc -g -Og -Wall  .\miniLexico_asdr.c -o .\miniLexico_asdr
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // exit(0);
//#########################
// INICIO: COMUM PARA LEXICO E SINTATICO

// definicao de tipo
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    OP_SOMA,
    OP_MULT,
    AND,
    BEGIN,
    BOOLEAN,
    ELIF,
    END,
    FALSE,
    FOR,
    IF,
    INTEGER,
    NOT,
    OF,
    OR,
    PROGRAM,
    READ,
    SET,
    TO,
    TRUE,
    WRITE,
    PONTO_VIRGULA,
    VIRGULA,
    COMENTARIO,
    DOIS_PONTOS,
    ABRE_PARENTESES,
    FECHA_PARENTESES,
    MENOR,
    MAIOR,
    MENOR_IGUAL,
    MAIOR_IGUAL,
    IGUAL,
    DIFERENTE,
    PONTO,
    OP_SUB,
    BARRA,
    EOS
}TAtomo;

typedef struct{
    TAtomo atomo;
    int linha;
    char atributo_numero[20];
    char atributo_ID[16];
}TInfoAtomo;

char *msgAtomo[] = {
    "ERRO LEXICO",
    "IDENTIFICADOR",
    "NUMERO",
    "+",
    "*",
    "AND",
    "BEGIN",
    "BOOLEAN",
    "ELIF",
    "END",
    "FALSE",
    "FOR",
    "IF",
    "INTEGER",
    "NOT",
    "OF",
    "OR",
    "PROGRAM",
    "READ",
    "SET",
    "TO",
    "TRUE",
    "WRITE",
    "PONTO_VIRGULA",
    "VIRGULA",
    "COMENTARIO",
    "DOIS_PONTOS",
    "ABRE_PARENTESES",
    "FECHA_PARENTESES",
    "MENOR",
    "MAIOR",
    "MENOR_IGUAL",
    "MAIOR_IGUAL",
    "IGUAL",
    "DIFERENTE",
    "PONTO",
    "OP_SUB",
    "BARRA",
    "EOS"
};
//#########################
// FIM: COMUM PARA LEXICO E SINTATICO

//#########################
// INICIO: LEXICO
// variavel global para o analisador lexico
// variavel bufer devera ser inicializada a partir de um arquivo texto
char *buffer;         
int contaLinha = 1;

// declaracao da funcao
TInfoAtomo obter_atomo(); // irah integrar com a Analisador Sintatico
TInfoAtomo reconhece_id();
TInfoAtomo reconhece_num();


//#########################
// FIM: LEXICO

//#########################
// INICIO: SINTATICO

TAtomo lookahead;//posteriormente sera do tipo TAtomo, declarado no ASDR
TInfoAtomo info_atomo;
void consome(TAtomo atomo);
int converteBinario(char atributo_numero[]);

// declaracao da funcao
void initPrograma();
void bloco();
void declaracaoVariaveis();
void tipo();
void listaVariavel();
void comandoComposto();
void comando();
void comandoAtribuicao();
void comandoCondicional();
void comandoRepeticao();
void comandoEntrada();
void comandoSaida();
void expressao();
void expressaoLogica();
void expressaoRelacional();
void opRelacional();
void expressaoSimples();
void termo();
void fator();

//#########################
// FIM: SINTATICO
int main (int argc, char** argv) {
    if(argc != 2) {
        printf("\nInsira o path do arquivo a ser lido!");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    
    if(file == NULL) {
      printf("\nArquivo inválido!");
      return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    buffer = malloc(fileSize);
    size_t bytes = fread(buffer, 1, fileSize, file);

    if(bytes != fileSize) {
        printf("\nFalha ao ler o arquivo!");
        return 1;
    }

    info_atomo = obter_atomo();
    lookahead = info_atomo.atomo;

    initPrograma(); // simbolo inicial da gramatica

    consome(EOS); // se lookahead chegou ao final

    printf("%d linhas analisadas, programa sintaticamente correto\n", info_atomo.linha);

    return 0;
}

//#########################
// INICIO: LEXICO
TInfoAtomo obter_atomo(){
    TInfoAtomo info_atomo;

    // consome espaços em branco quebra de linhas tabulação e retorno de carro
    while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r'){
        if(*buffer =='\n') {
            contaLinha++;
        }
        
        buffer++;
    }
    // reconhece identificador
    if(islower(*buffer)){ // ser for letra mininuscula
        info_atomo = reconhece_id();
    }
    // reconhece numero
    else if(isdigit(*buffer)){ // ser for digito
        info_atomo = reconhece_num();
    }

    else if(*buffer == '#') {
        while(*buffer != '\n')
            buffer++;
        
        info_atomo.atomo = COMENTARIO;
    }

    else if(*buffer == '{') {
        buffer++;
      
        if(*buffer == '-') {
q1:
            while(*buffer != '-') {
                if(*buffer == '\n') {contaLinha++;}
                buffer++;
            }

            buffer++;
            if(*buffer == '}') {
                info_atomo.atomo = COMENTARIO;
                buffer++;
            }
          
            else {goto q1;}
        }
        
        else{info_atomo.atomo = ERRO;}
    }
    
    else if(*buffer == '+'){
        info_atomo.atomo = OP_SOMA;
        buffer++;
    }

    else if(*buffer == '-'){
        info_atomo.atomo = OP_SUB;
        buffer++;
    } 

    else if(*buffer == '*'){
        info_atomo.atomo = OP_MULT;
        buffer++;
    }

    else if(*buffer == ':'){
        info_atomo.atomo = DOIS_PONTOS;
        buffer++;
    }

    else if(*buffer == ';'){
        info_atomo.atomo = PONTO_VIRGULA;
        buffer++;
    }

    else if(*buffer == ','){
        info_atomo.atomo = VIRGULA;
        buffer++;
    }

    else if(*buffer == '.'){
        info_atomo.atomo = PONTO;
        buffer++;
    }

    else if(*buffer == '='){
        info_atomo.atomo = IGUAL;
        buffer++;
    }

    else if(*buffer == '('){
        info_atomo.atomo = ABRE_PARENTESES;
        buffer++;
    }

    else if(*buffer == ')'){
        info_atomo.atomo = FECHA_PARENTESES;
        buffer++;
    }

    else if(*buffer == '<'){
        buffer++;

        if(*buffer == '=') {
            info_atomo.atomo = MENOR_IGUAL;
            buffer++;
        }
        
        else {
            info_atomo.atomo = MENOR;
        }
    }
    
    else if(*buffer == '>'){
        buffer++;

        if(*buffer == '=') {
            info_atomo.atomo = MAIOR_IGUAL;
            buffer++;
        }
        
        else {
            info_atomo.atomo = MAIOR;
        }
    }

    else if(*buffer == '/'){
        buffer++;

        if(*buffer == '=') {
            info_atomo.atomo = DIFERENTE;
            buffer++;
        }
        
        else {
            info_atomo.atomo = BARRA;
        }
    }

    else if(*buffer == 0){
        info_atomo.atomo = EOS;
    }
    
    else{
        info_atomo.atomo = ERRO;
    }
    
    info_atomo.linha = contaLinha;

    if(info_atomo.atomo == IDENTIFICADOR) 
        printf("%03d# %s | %s\n", info_atomo.linha, msgAtomo[info_atomo.atomo], info_atomo.atributo_ID);

    else if(info_atomo.atomo == NUMERO)
        printf("%03d# %s | %d\n", info_atomo.linha, msgAtomo[info_atomo.atomo], converteBinario(info_atomo.atributo_numero));
    
    else
        printf("%03d# %s\n", info_atomo.linha, msgAtomo[info_atomo.atomo]);

    return info_atomo;
}
// IDENTIFICADOR -> LETRA_MINUSCULA (LETRA_MINUSCULA | DIGITO )*
TInfoAtomo reconhece_id(){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;
    char *iniID = buffer;
    // ja temos uma letra minuscula
    buffer++;
q1:
    if( islower(*buffer) || isdigit(*buffer) || *buffer == '_'){
        buffer++;
        goto q1;
    }
    else if( isupper(*buffer))
        return info_atomo;

    //"  var1@"
    // falta testar o tamanho do identificador que dever ser menor que 15,
    // se for maior eh erro lexico
    if(buffer - iniID > 15)
        return info_atomo;

    strncpy(info_atomo.atributo_ID, iniID, buffer - iniID);
    info_atomo.atributo_ID[buffer - iniID] = 0; // finaliza a string
    info_atomo.atomo = IDENTIFICADOR;

    if(strcmp(info_atomo.atributo_ID, "and") == 0)
        info_atomo.atomo = AND;      

    else if(strcmp(info_atomo.atributo_ID, "begin") == 0)
        info_atomo.atomo = BEGIN;
   
    else if(strcmp(info_atomo.atributo_ID, "boolean") == 0)
        info_atomo.atomo = BOOLEAN;

    else if(strcmp(info_atomo.atributo_ID, "elif") == 0)
        info_atomo.atomo = ELIF;
    
    else if(strcmp(info_atomo.atributo_ID, "end") == 0)
        info_atomo.atomo = END;

    else if(strcmp(info_atomo.atributo_ID, "false") == 0)
        info_atomo.atomo = FALSE;

    else if(strcmp(info_atomo.atributo_ID, "for") == 0)
        info_atomo.atomo = FOR;

    else if(strcmp(info_atomo.atributo_ID, "if") == 0)
        info_atomo.atomo = IF;

    else if(strcmp(info_atomo.atributo_ID, "integer") == 0)
        info_atomo.atomo = INTEGER;

    else if(strcmp(info_atomo.atributo_ID, "not") == 0)
        info_atomo.atomo = NOT;

    else if(strcmp(info_atomo.atributo_ID, "of") == 0)
        info_atomo.atomo = OF;

    else if(strcmp(info_atomo.atributo_ID, "or") == 0)
        info_atomo.atomo = OR;

    else if(strcmp(info_atomo.atributo_ID, "program") == 0)
        info_atomo.atomo = PROGRAM;

    else if(strcmp(info_atomo.atributo_ID, "read") == 0)
        info_atomo.atomo = READ;
    
    else if(strcmp(info_atomo.atributo_ID, "set") == 0)
        info_atomo.atomo = SET;

    else if(strcmp(info_atomo.atributo_ID, "to") == 0)
        info_atomo.atomo = TO;

    else if(strcmp(info_atomo.atributo_ID, "true") == 0)
        info_atomo.atomo = TRUE;

    else if(strcmp(info_atomo.atributo_ID, "write") == 0)
        info_atomo.atomo = WRITE;
    
    return info_atomo;
}
// NUMERO -> DIGITO+.DIGITO+
// NUMERO -> 0b (0|1)+
TInfoAtomo reconhece_num(){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;
    char *iniID = buffer;
    // ja temos um digito
    buffer++;

    if(*iniID != '0' || *buffer != 'b')
        return info_atomo;

    buffer++;

    if(*buffer != '0' && *buffer != '1')
        return info_atomo;
    
q1:
    if(*buffer == '0' || *buffer == '1') {
        buffer++;
        goto q1;
    }

    info_atomo.atomo = NUMERO;
    strncpy(info_atomo.atributo_numero, iniID, buffer - iniID);
    info_atomo.atributo_numero[buffer - iniID] = 0; 
    return info_atomo;
}
//#########################
// FIM: LEXICO

//#########################
// INICIO: SINTATICO
void consome(TAtomo atomo){
    if(lookahead == atomo){
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
    }

    else if(lookahead == COMENTARIO) {
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
        consome(atomo);
    }

    else{
        printf("#%d:Erro sintatico:esperado [%s] encontrado [%s] \n", info_atomo.linha, msgAtomo[atomo], msgAtomo[lookahead]);
        exit(0);
    }
}

int converteBinario(char atributo_numero[]) {
    int decimal = 0;  
    for (int i = 2; atributo_numero[i] != '\0'; i++) {
        decimal = (decimal << 1) | (atributo_numero[i] - '0');
    }

    return decimal;
}

// E::=numero|identificador|+EE|*EE
void initPrograma(){
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

void bloco() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    declaracaoVariaveis();
    comandoComposto();
}

void declaracaoVariaveis() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}
q1:
    if(lookahead == INTEGER || lookahead == BOOLEAN) {
        tipo();
        listaVariavel();
        consome(PONTO_VIRGULA);
        goto q1;
    }
}

void tipo() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    switch (lookahead) {  
        case INTEGER:
            consome(INTEGER);
            break;

        case BOOLEAN:
            consome(BOOLEAN);
            break;
        
        default:
            consome(ERRO);
    }
}

void listaVariavel() { 
    consome(IDENTIFICADOR);
    
q1:
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    if(lookahead == VIRGULA) {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
        goto q1;
    }
    
}

void comandoComposto() {
    consome(BEGIN);
    comando();

q1:
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}
  
    if(lookahead == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        comando();
        goto q1;
    }

    consome(END);
}

void comando() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    switch (lookahead) {  
        case SET:
            comandoAtribuicao();
            break;
        
        case IF:
            comandoCondicional();
            break;

        case FOR:
            comandoRepeticao();
            break;

        case READ:
            comandoEntrada();
            break;

        case WRITE:
            comandoSaida();
            break;

        case BEGIN:
            comandoComposto();
            break;

        default:
            consome(ERRO);
    }

}

void comandoAtribuicao() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    consome(SET);
    consome(IDENTIFICADOR);
    consome(TO);
    expressao();
}

void comandoCondicional() {
    consome(IF);
    expressao();
    consome(DOIS_PONTOS);
    comando();
    
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}
    
    if(lookahead == ELIF) {
        consome(ELIF);
        comando();
    }
}

void comandoRepeticao() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    consome(FOR);
    consome(IDENTIFICADOR);
    consome(OF);
    expressao();
    consome(TO);
    expressao();
    consome(DOIS_PONTOS);
    comando();
}

void comandoEntrada() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    consome(READ);
    consome(ABRE_PARENTESES);
    listaVariavel();
    consome(FECHA_PARENTESES);
}

void comandoSaida() {
    consome(WRITE);
    consome(ABRE_PARENTESES);
    expressao();
    
q1:
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    if(lookahead == VIRGULA) {
        consome(VIRGULA);
        expressao();
        goto q1;
    }

    consome(FECHA_PARENTESES);
}

void expressao() {
    expressaoLogica();
    
q1:
   while(lookahead == COMENTARIO) {consome(COMENTARIO);}
   
   if(lookahead == OR) {
        consome(OR);
        expressaoLogica();
        goto q1;
   } 
}

void expressaoLogica() {
    expressaoRelacional();

q1:
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    if(lookahead == AND) {
        consome(AND);
        expressaoRelacional();
        goto q1;
    }
}

void expressaoRelacional() {
    expressaoSimples();
  
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}
    
    if(lookahead == MENOR || lookahead == MENOR_IGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == MAIOR || lookahead == MAIOR_IGUAL) {
        opRelacional();
        expressaoSimples();
  }
}

void opRelacional() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    switch(lookahead) {
        case MENOR:
            consome(MENOR);
            break;

        case MENOR_IGUAL:
            consome(MENOR_IGUAL);
            break;
      
        case IGUAL:
            consome(IGUAL);
            break;

        case DIFERENTE:
            consome(DIFERENTE);
            break;

        case MAIOR:
            consome(MAIOR);
            break;
          
        case MAIOR_IGUAL:
            consome(MAIOR_IGUAL);
            break;

        default:
            consome(ERRO);
    }
}

void expressaoSimples() {
    termo();

q1:
    while(lookahead == COMENTARIO) {consome(COMENTARIO);} 
    switch (lookahead) {
        case OP_SOMA:
            consome(OP_SOMA);
            termo();
            goto q1;
            break;

        case OP_SUB:
            consome(OP_SUB);
            termo();
            goto q1;
            break;
        
        default:
            break;
    }
}

void termo() {
    fator();

q1:
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}
    switch (lookahead) {
        case OP_MULT:
            consome(OP_MULT);
            fator();
            goto q1;
            break;

        case BARRA:
            consome(BARRA);
            fator();
            goto q1;
            break;
        
        default:
            break;
    } 
}

void fator() {
    while(lookahead == COMENTARIO) {consome(COMENTARIO);}

    switch(lookahead) {
        case IDENTIFICADOR:
            consome(IDENTIFICADOR);
            break;

        case NUMERO:
            consome(NUMERO);
            break;

        case TRUE:
            consome(TRUE);
            break;

        case FALSE:
            consome(FALSE);
            break;

        case NOT:
            consome(NOT);
            fator();
            break;

        case ABRE_PARENTESES:
            consome(ABRE_PARENTESES);
            expressao();
            consome(FECHA_PARENTESES);
            break;

        default:
            consome(ERRO);
    }

}

//#########################
// INICIO: SINTATICO
