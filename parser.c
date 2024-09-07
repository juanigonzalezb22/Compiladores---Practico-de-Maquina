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
	test(CVOID|CCHAR|CINT|CFLOAT|CEOF, folset ,1);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones( (folset | CVOID | CCHAR | CINT | CFLOAT));
}


void declaraciones( set folset )
{	
	especificador_tipo(folset | CIDENT | CPAR_ABR | CASIGNAC |CCOR_ABR );
	
	match(CIDENT, 10);
	
	especificador_declaracion(folset);
}


void especificador_tipo( set folset )
{
	test(CVOID|CCHAR|CINT|CFLOAT,folset,1);
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
			error_handler(18);
	}
	test(folset,0,1);
}


void especificador_declaracion( set folset )
{	
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion( folset );
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			declaracion_variable( folset );
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion( set folset )
{
	match(CPAR_ABR, 10);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param(folset | CPAR_CIE | CLLA_ABR );

	match(CPAR_CIE, 10);

	proposicion_compuesta(folset);
}


void lista_declaraciones_param( set folset )
{
	declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);

	while(lookahead_in(CCOMA))
	{
		scanner();
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
	}
}


void declaracion_parametro( set folset )
{
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

	if(lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 10);

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		match(CCOR_CIE, 10);
	}
	test(folset,0,1);
}


void lista_declaraciones_init( set folset )
{
	test(CIDENT | folset, CCOMA | CASIGNAC | CCOR_ABR, 1); //DUDA
	match(CIDENT, 10);

	declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR);

	while(lookahead_in(CCOMA))
	{
		scanner();
		match(CIDENT, 10);
		declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR);
	}
}


void declaracion_variable( set folset )
{
	declarador_init(folset | CCOMA | CIDENT | CPYCOMA);

	if(lookahead_in(CCOMA))
	{
		scanner();
		lista_declaraciones_init(folset);
	}

	match(CPYCOMA, 10);
}


void declarador_init( set folset )
{
	test(CASIGNAC|CCOR_ABR, folset | CCOR_CIE | CCONS_ENT | CLLA_ABR | CLLA_CIE | CCONS_FLO | CCONS_CAR, 1);
	switch(lookahead())
	{
		case CASIGNAC:
			scanner();
			constante(folset);
			break;

		case CCOR_ABR:
		case CCOR_CIE:
		case CCONS_ENT:  
		case CLLA_ABR:
		case CLLA_CIE:
		case CCONS_FLO:
		case CCONS_CAR : 
			scanner();
			
			if(lookahead_in(CCONS_ENT))
				constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CCONS_ENT | CCONS_FLO | CCONS_CAR | CLLA_CIE);

			match(CCOR_CIE, 10);

			if(lookahead_in(CASIGNAC))
			{
				scanner();
				match(CLLA_ABR, 10);
				lista_inicializadores(folset | CLLA_CIE);
				match(CLLA_CIE, 10);
			}
			break;
	}
	test(folset,0,1);
}


void lista_inicializadores( set folset )
{
	constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);

	while(lookahead_in(CCOMA))
	{
		scanner();
		constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	}
}


void proposicion_compuesta( set folset )
{
	test(CLLA_ABR, folset|CLLA_CIE|CVOID | CCHAR | CINT | CFLOAT | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN,1);
	match(CLLA_ABR, 10);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CLLA_CIE);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset | CLLA_CIE);

	match(CLLA_CIE, 10);
	test(folset,0,1);
}


void lista_declaraciones( set folset )
{
	declaracion(folset | CVOID | CCHAR | CINT | CFLOAT ); 

	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset | CVOID | CCHAR | CINT | CFLOAT); 
}


void declaracion( set folset )
{
	especificador_tipo(folset | CIDENT | CPYCOMA);

	lista_declaraciones_init(folset | CPYCOMA);

	match(CPYCOMA, 10);
	test(folset,0,1);
}


void lista_proposiciones( set folset )
{
	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN );

	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN );
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
			error_handler(26);
	}
}


void proposicion_iteracion( set folset )
{
	match(CWHILE, 10);

	match(CPAR_ABR, 10);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);

	match(CPAR_CIE, 10);

	proposicion(folset);
}


void proposicion_seleccion( set folset )
{
	match(CIF, 10);

	match(CPAR_ABR, 10);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	match(CPAR_CIE, 10);

	proposicion(folset |  CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

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
			
			variable(folset | CSHR | CIDENT | CPYCOMA);
			
			while(lookahead_in(CSHR))
			{
				scanner();
				variable(folset | CSHR | CIDENT | CPYCOMA);
			}

			match(CPYCOMA, 10);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 10);
			
			expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);

			while(lookahead_in(CSHL))
			{
				scanner();
				expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);
			}

			match(CPYCOMA, 10);
			
			break;
		
		default:
			error_handler(29);
	}
	test(folset,0,1);
}


void proposicion_retorno( set folset )
{
	scanner();
	
	expresion(folset | CPYCOMA);
	
	match(CPYCOMA, 10);
	test(folset,0,1);
}


void proposicion_expresion( set folset )
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);

	match(CPYCOMA, 10);
	test(folset,0,1);
}


void expresion( set folset )
{	
	expresion_simple(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | 
									CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG |
									CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG);

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
	test(CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG|CMAS|CMENOS,folset | CPAR_CIE,1); //Punto de reconfiguración
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA | COR);

	while(lookahead_in(CMAS | CMENOS | COR))
	{
		scanner();
		termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA | COR);
	}
}


void termino( set folset )
{	
	factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 	CPAR_ABR | CNEG | CPYCOMA | CMULT | CDIV | CAND );

	while(lookahead_in(CMULT | CDIV | CAND))
	{
		scanner();
		factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 	CPAR_ABR | CNEG | CPYCOMA | CMULT | CDIV | CAND );
	}
}


void factor( set folset )
{
	test(CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG,folset | CPAR_CIE,1);
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
		case CPAR_CIE:
			scanner();
			expresion(folset);
			match(CPAR_CIE, 10);
			break;
			
		case CNEG:
			scanner();
			expresion(folset);
			break;
			
		default:
			error_handler(32);
	}
	test(folset,0,1);
}


void variable( set folset )
{
	test(CIDENT, folset | CCOR_ABR | CCOR_CIE, 1);
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
	test(folset,0,1);
}


void llamada_funcion( set folset )
{
	match(CIDENT, 10);
	
	match(CPAR_ABR, 10);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 10);
	test(folset,0,1);
}


void lista_expresiones( set folset )
{
	expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA | CCOMA);

	while(lookahead_in(CCOMA))
	{
		scanner();
		expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA | CCOMA);
	}
}


void constante( set folset )
{
	test(CCONS_ENT|CCONS_FLO|CCONS_CAR, folset, 1);
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
			error_handler(33);
	}
	test(folset,0,1);
}
