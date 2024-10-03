#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigos.h"
#include "util.h"
#include "error.h"
#include "ts.h"

int last_call=0;

/*********** prototipos *************/
void unidad_traduccion(set);
void declaraciones(set);
void especificador_tipo(set);
void especificador_declaracion(set);
void definicion_funcion(set, int);
void declaracion_variable(set);
void lista_declaraciones_param(set);
void declaracion_parametro(set);
void declarador_init(set);
void lista_declaraciones_init(set);
void lista_inicializadores(set, int, int*);
void lista_proposiciones(set);
void lista_declaraciones(set);
void declaracion(set);
void proposicion(set);
void proposicion_expresion(set);
void proposicion_compuesta(set);
void proposicion_seleccion(set);
void proposicion_iteracion(set);
void proposicion_e_s(set);
void proposicion_retorno(set);
void expresion(set);
void expresion_simple(set);
void termino(set);
void factor(set);
void variable(set,int);
void llamada_funcion(set,int);
void lista_expresiones(set);
void constante(set);
void imprimir_set(set);
