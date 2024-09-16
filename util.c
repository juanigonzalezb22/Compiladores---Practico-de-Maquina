#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "codigos.h"
#include "error.h"


void scanner()
{	
	int i;
	for(; (i = yylex()) != NADA && lookahead() == SEGUIR;);

	if(i == NADA)
		sbol->codigo = CEOF;
	/* yylex retorna 0 si llego a fin de archivo */
	printf("Scanner: %s\n", sbol->lexema);					//DESCOMENTAR EN CASO DE EMERGENCIA.
	liberar = linea;
	linea = (char *) malloc (strlen(linea) + strlen (token1.lexema) + 3);
	strcpy(linea, liberar);
	strcat(linea, token1.lexema);
	free((void *) liberar);
}


void init_parser(int argc, char *argv[])
{
	linea = (char *) malloc (2);
	strcpy(linea, "");
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
			error_handler(7);
			error_handler(COD_IMP_ERRORES);
			exit(1);
		}
	}

	sbol = &token1;

	scanner();
}


void match(set codigo, int ne)
{
    if(lookahead() & codigo){
			printf("Match: %s\n", lookahead_lexema());					//DESCOMENTAR EN CASO DE EMERGENCIA.
			scanner();
		} else
        error_handler(ne);
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
		error_print(nro_error);
		conjunto1 |= puntos_de_reconfiguracion;
		while(!lookahead_in(conjunto1))
			scanner();
	}
}

