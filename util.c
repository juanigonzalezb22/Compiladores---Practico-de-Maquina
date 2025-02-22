#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "codigos.h"
#include "error.h"
#include "ts.h"
#include "sistejec.h"


void scanner()
{	
	int i;
	for(; (i = yylex()) != NADA && lookahead() == SEGUIR;);

	if(i == NADA)
		sbol->codigo = CEOF;
	/* yylex retorna 0 si llego a fin de archivo */
	//printf("Scanner: %s\n", sbol->lexema);					//DESCOMENTAR EN CASO DE EMERGENCIA.
	liberar = linea;
	linea = (char *) malloc (strlen(linea) + strlen (token1.lexema) + 3);
	strcpy(linea, liberar);
	strcat(linea, token1.lexema);
	free((void *) liberar);
}


void init_parser(int argc, char *argv[])
{
	linea = (char *) malloc (2);
	strcat(linea, "");
	nro_linea = 0;

	if(argc != 3)
	{
		error_handler(6);
		error_handler(COD_IMP_ERRORES);
		exit(1);
	}
	else
	{
		if(strcmp(argv[1], "-c") == 0)
		{
			if((yyin = fopen(argv[2], "r")) == NULL)
			{
				error_handler(8);
				error_handler(COD_IMP_ERRORES);
				exit(1);
			}
		}
		else
		{
			if(strcmp(argv[1], "-o") == 0){
				cargar_codgen(argv[2]);
				interprete();
				impr_codgen();
				exit(1);
			}
			else{
				error_handler(7);
				error_handler(COD_IMP_ERRORES);
				exit(1);
			}
		}
	}

	sbol = &token1;

	scanner();
}


void match(set codigo, int ne)
{
    if(lookahead() & codigo){
			//printf("Match: %s\n", lookahead_lexema());					//DESCOMENTAR EN CASO DE EMERGENCIA.
			if( codigo == CIDENT ){
				strcpy(inf_id->nbre, sbol->lexema);
				//printf("IDENTIFICADOR: %s\n", inf_id->nbre);
			}
			scanner();
	} else{
        error_handler(ne);
		GEN = 0;
	}
}


set lookahead()
{
	return sbol->codigo;
}

char* lookahead_lexema()
{
return sbol->lexema;
}


set lookahead_in(set conjunto)
{
	return lookahead() & conjunto;
}


void test( set conjunto1, set puntos_de_reconfiguracion, int nro_error ){
	if( !lookahead_in(conjunto1) ){
		error_handler(nro_error);
		GEN = 0;
		conjunto1 |= puntos_de_reconfiguracion;
		while(!lookahead_in(conjunto1)){
			scanner();
		}	
	}
}

void imprimir_set(set conjunto) {
    printf("Elementos en el conjunto:\n");
    for (int i = 0; i < sizeof(set) * 8; ++i) { // Itera sobre cada bit del conjunto
        if (conjunto & (1LL << i)) {            // Si el bit i-ésimo está encendido
            printf("Elemento %d\n", i);         // Imprime el índice del elemento
        }
    }
	system("pause");
}