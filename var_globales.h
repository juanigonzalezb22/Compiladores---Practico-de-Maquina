#define TAM_LEXEMA 200

typedef long long set;

typedef struct { set codigo;
                 char lexema[TAM_LEXEMA];
               }token;

#define COD_IMP_ERRORES -1


char * linea;
char *liberar;
int nro_linea;

token token1;

extern int yylex(void);


#define TAM_ETAPA3 8

int etapa3[TAM_ETAPA3];

#define TIPO_CONSTANTE 0    //etapa3[TIPO_CONSTANTE]      0 = CHAR / 1 = INT / 2 = FLOAT 
#define DESDE_WHILE 1
#define VALOR_NEG 2
#define CANT_NEGACIONES 3
#define VALOR_RELACION1 4
#define VALOR_RELACION2 5
#define CANT_RELACIONES1 6
#define CANT_RELACIONES2 7

float constante_aux;
