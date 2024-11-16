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


#define TAM_ETAPA3 2

int etapa3[TAM_ETAPA3];

#define TIPO_CONSTANTE 0   // 0 = CHAR / 1 = INT / 2 = FLOAT 
#define DESDE_WHILE 1
float constante_aux;
