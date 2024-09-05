#include "parser.h"


int main(int argc, char *argv[])
{
	//DEFINIMOS FOLSET
	set folset = 0;
	//folset = folset | CVOID | CCHAR | CINT | CFLOAT | CEOF;
	//printf("%lld\n", folset);

	init_parser(argc, argv);

	unidad_traduccion( (folset | CEOF) );

	match(CEOF, 10);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}


/********* funciones del parser ***********/

void unidad_traduccion( set folset )
{
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones(folset);
}


void declaraciones( set folset )
{	
	especificador_tipo(folset);
	
	match(CIDENT, 10);
	
	especificador_declaracion(folset);
}


void especificador_tipo( set folset )
{	
	switch(lookahead())
	{
		case CVOID:
			scanner();
			break;
			
		case CCHAR:
			scanner();
			break;
			
		case CINT:
			scanner();
			break;
			
		case CFLOAT:
			scanner();
			break;
			
		default:
			error_handler(10);
	}
}


void especificador_declaracion( set folset )
{	
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion(folset);
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			declaracion_variable(folset);
			break;
		
		default:
			error_handler(10);
	}
}


void definicion_funcion( set folset )
{
	match(CPAR_ABR, 10);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param(folset);

	match(CPAR_CIE, 10);

	proposicion_compuesta(folset);
}


void lista_declaraciones_param( set folset )
{
	declaracion_parametro(folset);

	while(lookahead_in(CCOMA))
	{
		scanner();
		declaracion_parametro(folset);
	}
}


void declaracion_parametro( set folset )
{
	especificador_tipo(folset);

	if(lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 10);

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		match(CCOR_CIE, 10);
	}
}


void lista_declaraciones_init( set folset )
{
	match(CIDENT, 10);

	declarador_init(folset);

	while(lookahead_in(CCOMA))
	{
		scanner();
		match(CIDENT, 10);
		declarador_init(folset);
	}
}


void declaracion_variable( set folset )
{
	declarador_init(folset);

	if(lookahead_in(CCOMA))
	{
		scanner();
		lista_declaraciones_init(folset);
	}

	match(CPYCOMA, 10);
}


void declarador_init( set folset )
{
	switch(lookahead())
	{
		case CASIGNAC:
			scanner();
			constante(folset);
			break;

		case CCOR_ABR:
			scanner();
			
			if(lookahead_in(CCONS_ENT))
				constante(folset);

			match(CCOR_CIE, 10);

			if(lookahead_in(CASIGNAC))
			{
				scanner();
				match(CLLA_ABR, 10);
				lista_inicializadores(folset);
				match(CLLA_CIE, 10);
			}
			break;
	}
}


void lista_inicializadores( set folset )
{
	constante(folset);

	while(lookahead_in(CCOMA))
	{
		scanner();
		constante(folset);
	}
}


void proposicion_compuesta( set folset )
{
	match(CLLA_ABR, 10);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset);

	match(CLLA_CIE, 10);
}


void lista_declaraciones( set folset )
{
	declaracion(folset); 

	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset); 
}


void declaracion( set folset )
{
	especificador_tipo(folset);

	lista_declaraciones_init(folset);

	match(CPYCOMA, 10);
}


void lista_proposiciones( set folset )
{
	proposicion(folset);

	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion(folset);
}


void proposicion( set folset )
{
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta(folset);
			break;
		
		case CWHILE:
			proposicion_iteracion(folset);
			break;
		
		case CIF:
			proposicion_seleccion(folset);
			break;
		
		case CIN:
		case COUT:
			proposicion_e_s(folset);
			break;
		
		case CMAS:
		case CMENOS:
		case CIDENT:
		case CPAR_ABR:
		case CNEG:
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
		case CCONS_STR:
		case CPYCOMA:
			proposicion_expresion(folset);
			break;
		
		case CRETURN:
			proposicion_retorno(folset);
			break;
		
		default:
			error_handler(10);
	}
}


void proposicion_iteracion( set folset )
{
	match(CWHILE, 10);

	match(CPAR_ABR, 10);

	expresion(folset);

	match(CPAR_CIE, 10);

	proposicion(folset);
}


void proposicion_seleccion( set folset )
{
	match(CIF, 10);

	match(CPAR_ABR, 10);

	expresion(folset);

	match(CPAR_CIE, 10);

	proposicion(folset);

	if(lookahead_in(CELSE))
	{
		scanner();
		proposicion(folset);
	}
}


void proposicion_e_s( set folset )
{
	switch(lookahead())
	{
		case CIN:
			scanner();
			
			match(CSHR, 10);
			
			variable(folset);
			
			while(lookahead_in(CSHR))
			{
				scanner();
				variable(folset);
			}

			match(CPYCOMA, 10);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 10);
			
			expresion(folset);

			while(lookahead_in(CSHL))
			{
				scanner();
				expresion(folset);
			}

			match(CPYCOMA, 10);
			
			break;
		
		default:
			error_handler(10);
	}
}


void proposicion_retorno( set folset )
{
	scanner();
	
	expresion(folset);
	
	match(CPYCOMA, 10);
}


void proposicion_expresion( set folset )
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset);

	match(CPYCOMA, 10);
}


void expresion( set folset )
{	
	expresion_simple(folset);

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				expresion_simple(folset);
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple(folset);
				break;
		}
	}
}


void expresion_simple( set folset )
{
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino(folset);

	while(lookahead_in(CMAS | CMENOS | COR))
	{
		scanner();
		termino(folset);
	}
}


void termino( set folset )
{	
	factor(folset);

	while(lookahead_in(CMULT | CDIV | CAND))
	{
		scanner();
		factor(folset);
	}
}


void factor( set folset )
{
	switch(lookahead())
	{
		case CIDENT:
			/***************** Re-hacer *****************/
			if(sbol->lexema[0] == 'f')
				llamada_funcion(folset);
			else
				variable(folset);
			/********************************************/
			/* El alumno debera evaluar con consulta a TS
			si bifurca a variable o llamada a funcion */
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			constante(folset);
			break;
		
		case CCONS_STR:
			scanner();
			break;
		
		case CPAR_ABR:
			scanner();
			expresion(folset);
			match(CPAR_CIE, 10);
			break;
			
		case CNEG:
			scanner();
			expresion(folset);
			break;
			
		default:
			error_handler(10);
	}
}


void variable( set folset )
{
	match(CIDENT, 10);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		expresion(folset);
		match(CCOR_CIE, 10);
	}
}


void llamada_funcion( set folset )
{
	match(CIDENT, 10);
	
	match(CPAR_ABR, 10);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset);

	match(CPAR_CIE, 10);
}


void lista_expresiones( set folset )
{
	expresion(folset);

	while(lookahead_in(CCOMA))
	{
		scanner();
		expresion(folset);
	}
}


void constante( set folset )
{
	switch(lookahead())
	{
		case CCONS_ENT:
			scanner();
			break;
		
		case CCONS_FLO:
			scanner();
			break;
		
		case CCONS_CAR:
			scanner();
			break;
		
		default:
			error_handler(10);
	}
}
