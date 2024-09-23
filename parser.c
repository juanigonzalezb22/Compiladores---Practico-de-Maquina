#include "parser.h"

int main(int argc, char *argv[])
{
	init_parser(argc, argv);

	unidad_traduccion(CEOF);

	match(CEOF, 9);

	last_call = 1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}

/********* funciones del parser ***********/

void unidad_traduccion(set folset)
{	
	test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA , 40);
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaraciones(CVOID | CCHAR | CINT | CFLOAT | folset);
	}
}


void declaraciones(set folset)
{
	especificador_tipo(CIDENT | CPAR_ABR | CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA| folset);
	match(CIDENT, 17);
	especificador_declaracion(folset);
}


void especificador_tipo(set folset)
{
	test(CVOID | CCHAR | CINT | CFLOAT, folset, 41);
	switch (lookahead())
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
	test(folset, NADA , 42);
}


void especificador_declaracion(set folset)
{
	test(CPAR_ABR|CASIGNAC|CCOR_ABR|CCOMA|CPYCOMA,folset,43);
	switch (lookahead())
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
		error_handler(19);
	}
}


void definicion_funcion(set folset)
{	
	match(CPAR_ABR, 20);

	if (lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param( CPAR_CIE | CLLA_ABR| folset);

	match(CPAR_CIE, 21);

	proposicion_compuesta(folset);
}


void lista_declaraciones_param(set folset)
{
	declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
	while (lookahead_in(CCOMA | CVOID | CCHAR | CINT | CFLOAT))
	{
		match(CCOMA, 64);
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
	}
}


void declaracion_parametro(set folset)
{
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

	if (lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 17);

	if (lookahead_in(CCOR_ABR | CCOR_CIE))
	{
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
	}
	test(folset, NADA ,45);
}


void lista_declaraciones_init(set folset)
{	
	test(CIDENT, folset | CCOMA | CASIGNAC | CCOR_ABR, 46);
	match(CIDENT, 17);
	declarador_init( CCOMA | CIDENT | CASIGNAC | CCOR_ABR | folset);
	test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	while (lookahead_in(CCOMA | CIDENT | CASIGNAC | CCOR_ABR))
	{
		match(CCOMA, 64);
		match(CIDENT, 17);
		declarador_init( CCOMA | CIDENT | CASIGNAC | CCOR_ABR | folset);
		test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	}
}


void declaracion_variable(set folset)
{
	declarador_init( CCOMA | CIDENT | CPYCOMA | folset );
	if (lookahead_in(CCOMA | CIDENT))
	{
		match(CCOMA, 64);
		lista_declaraciones_init(CPYCOMA| folset );
	}
	match(CPYCOMA, 23);

	test(folset, NADA , 51); //ver codigo de error y preguntar al edgardo quizás xdxd
}


void declarador_init(set folset)
{	
	test(CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA | folset, CCOR_CIE | CLLA_ABR | CLLA_CIE | CCONS_FLO | CCONS_CAR , 47);
	switch(lookahead())
	{	
		case CCONS_FLO:
		case CCONS_CAR:
		case CASIGNAC:
			match(CASIGNAC, 66);
			constante(folset);
			break;

		case CCOR_ABR:
		case CCOR_CIE:
		case CLLA_ABR:
		case CLLA_CIE:
			 match(CCOR_ABR, 35);			
			if(lookahead_in(CCONS_ENT))
				constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CCONS_ENT | CCONS_FLO | CCONS_CAR | CLLA_CIE);

			match(CCOR_CIE, 22);

			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | CCONS_ENT | CCONS_FLO | CCONS_CAR))
			{
				match(CASIGNAC, 66);
				match(CLLA_ABR, 24);
				lista_inicializadores(folset | CLLA_CIE);
				match(CLLA_CIE, 25);
			}
			break;
	}
	test(folset, NADA, 48);
}


void lista_inicializadores(set folset)
{
	constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	while (lookahead_in(CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR))
	{
		match(CCOMA, 64);
		constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	}
}


void proposicion_compuesta(set folset)
{
	test(CLLA_ABR, folset | CLLA_CIE | CVOID | CCHAR | CINT | CFLOAT | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
	 		CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, 49);
	match(CLLA_ABR, 24);
	if (lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
	{
		lista_declaraciones(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
		 										CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CLLA_CIE);
	}
	if (lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
	 								CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
	{
		lista_proposiciones(folset | CLLA_CIE);
	}
	match(CLLA_CIE, 25);
	test(folset, NADA , 50);
}


void lista_declaraciones(set folset)
{
	declaracion(folset | CVOID | CCHAR | CINT | CFLOAT);
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset | CVOID | CCHAR | CINT | CFLOAT);
}


void declaracion(set folset)
{
	especificador_tipo(folset | CIDENT | CPYCOMA);

	lista_declaraciones_init(folset | CPYCOMA);

	match(CPYCOMA, 23);

	test(folset, NADA , 51); //chusmear
}


void lista_proposiciones(set folset)
{
	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
	 						CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);
	test(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
	 						CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, NADA, 68);
	while (lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
	 										CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
		 						CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);
		test(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO |
	 						CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, NADA, 68);
}


void proposicion(set folset)
{
	test(CLLA_ABR | CWHILE | CIF | CIN | COUT | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT |
	 		CCONS_FLO | CCONS_CAR | CCONS_STR | CPYCOMA | CRETURN | folset, CELSE | CSHL | CSHR, 52);
	switch (lookahead())
	{
	case CLLA_ABR:
		proposicion_compuesta(folset);
		break;

	case CWHILE:
		proposicion_iteracion(folset);
		break;

	case CIF:
	case CELSE:
		proposicion_seleccion(folset);
		break;

	case CIN:
	case COUT:
	case CSHL:
	case CSHR:
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


void proposicion_iteracion(set folset)
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT |
	 						CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);

	match(CPAR_CIE, 21);

	proposicion(folset);
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | 
			CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	match(CPAR_CIE, 21);

	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | 
											CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	// Se decidio no agregar un test para evitar errores en caso que falte CELSE ya que esta
	// implementacion genera varios problemas y consideramos que CELSE no es un simbolo facil de omitir.
	if (lookahead_in(CELSE))
	{
		scanner();
		proposicion(folset);
	}
}


void proposicion_e_s(set folset)
{
	switch (lookahead())
	{
	case CIN:
	case CSHR:
		match(CIN, 29);
		match(CSHR, 30);
		variable(folset | CSHR | CIDENT | CPYCOMA);
		while (lookahead_in(CSHR))
		{
			scanner();
			variable(folset | CSHR | CIDENT | CPYCOMA);
		}
		match(CPYCOMA, 23);
		break;

	case COUT:
	case CSHL:
		match(COUT, 29);
		match(CSHL, 31);
		expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);
		while (lookahead_in(CSHL))
		{
			scanner();
			expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);
		}
		match(CPYCOMA, 23);
		break;

	default:
		error_handler(29);
	}

	test(folset, NADA , 53);
}


void proposicion_retorno(set folset)
{
	scanner();
	expresion(folset | CPYCOMA);
	match(CPYCOMA, 23);
	
	test(folset, NADA , 54);
}


void proposicion_expresion(set folset)
{
	if (lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);
	match(CPYCOMA, 23);
	
	test(folset, NADA , 55);
}


void expresion(set folset)
{
	expresion_simple(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR |CCONS_STR | CMAS | CMENOS | 
									CPAR_ABR | CNEG | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG);
	test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 65);
	while (lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch (lookahead())
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
		test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 65);
	}
}


void expresion_simple(set folset)
{
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMAS | CMENOS, folset, 56);

	if (lookahead_in(CMAS | CMENOS))
		scanner();
	termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | COR);
	test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR, 69);
	while (lookahead_in(CMAS | CMENOS | COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR))
	{
		if (lookahead_in(CMAS | CMENOS | COR))
		{
			scanner();
		}
		else
		{
			error_handler(65);
		}
		termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | COR);
		test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR, 69);
	}
}


void termino(set folset)
{
	factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMULT | CDIV | CAND);
	while (lookahead_in(CMULT | CDIV | CAND))
	{
		if (lookahead_in(CMULT | CDIV | CAND))
			scanner();
		factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMULT | CDIV | CAND);
	}
}


void factor(set folset)
{
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, folset, 57);

	switch (lookahead())
	{
	case CIDENT:
		/***************** Re-hacer *****************/
		if (sbol->lexema[0] == 'f')
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
		expresion(folset | CPAR_CIE);
		match(CPAR_CIE, 21);
		break;

	case CNEG:
		scanner();
		expresion(folset);
		break;

	default:
		error_handler(32);
	}
	test(folset, NADA , 58);
}


void variable(set folset)
{
	test(CIDENT, folset | CCOR_ABR | CCOR_CIE, 59);
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if (lookahead_in(CCOR_ABR))
	{
		scanner();
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
	}
	test(folset, NADA , 60);
}


void llamada_funcion(set folset)
{
	match(CIDENT, 17);
	match(CPAR_ABR, 20);
	if (lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);
	match(CPAR_CIE, 21);

	test(folset, NADA , 61);
}


void lista_expresiones(set folset)
{
	expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
	test(CCOMA | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG, 64);
	while (lookahead_in(CCOMA | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG))
	{
		match(CCOMA, 64);
		expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
		test(CCOMA | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG, 64);
	}
}


void constante(set folset)
{
	test(CCONS_ENT | CCONS_FLO | CCONS_CAR, folset, 62);
	switch (lookahead())
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
	test(folset, NADA , 63);
}
