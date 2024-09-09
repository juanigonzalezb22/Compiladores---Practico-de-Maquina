#include "parser.h"


int main(int argc, char *argv[])
{
	init_parser(argc, argv);

	unidad_traduccion(CEOF);

	match(CEOF, 10);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}


/********* funciones del parser ***********/

void unidad_traduccion( set folset )
{
	test(CVOID|CCHAR|CINT|CFLOAT|folset, 0 ,40);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones(folset | CVOID | CCHAR | CINT | CFLOAT);
}


void declaraciones( set folset )
{	
	especificador_tipo(folset | CIDENT | CPAR_ABR | CASIGNAC |CCOR_ABR);
	
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
	test(folset,0,42);
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
	test(CCOMA | folset, CVOID | CCHAR | CINT | CFLOAT ,45); //Ver nro de error xdxd
	while(lookahead_in(CCOMA|CVOID|CCHAR|CINT|CFLOAT))
	{
		//scanner();
		match(CCOMA,64);
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
		test(CCOMA | folset, CVOID | CCHAR | CINT | CFLOAT ,45);
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
	//test(folset,0,45); Ver como decisión de diseño XD
}


void lista_declaraciones_init( set folset )
{
	test(CIDENT, folset | CCOMA | CASIGNAC | CCOR_ABR, 46); 
	match(CIDENT, 10);

	declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR); 
	test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR,48);
	while(lookahead_in(CCOMA | CIDENT | CASIGNAC | CCOR_ABR ))
	{
		//scanner();
		match(CCOMA,64);
		match(CIDENT, 10);
		declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR);
		test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR,48);
	}
}


void declaracion_variable( set folset )
{
	declarador_init(folset | CCOMA | CIDENT | CPYCOMA);

	if(lookahead_in(CCOMA))
	{
		scanner();
		lista_declaraciones_init(folset | CPYCOMA);
	}

	match(CPYCOMA, 10);
}


void declarador_init( set folset )
{
	test(CASIGNAC|CCOR_ABR|folset,CCOR_CIE | CCONS_ENT | CLLA_ABR | CLLA_CIE | CCONS_FLO | CCONS_CAR, 47);
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
	test(folset,0,48);
}


void lista_inicializadores( set folset )
{
	constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	test(CCOMA | folset, CCONS_ENT | CCONS_FLO | CCONS_CAR, 63);
	while(lookahead_in(CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR))
	{
		//scanner();
		match(CCOMA,64);
		constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
		test(CCOMA | folset, CCONS_ENT | CCONS_FLO | CCONS_CAR, 63);
	}
}


void proposicion_compuesta( set folset )
{
	test(CLLA_ABR, folset|CLLA_CIE|CVOID | CCHAR | CINT | CFLOAT | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN,49);
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
	test(folset,0,50);
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
	test(folset,0,51);
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

	//DUDA TEST DESPUES DE PROPOSICION
}


void proposicion( set folset )
{
	test(CLLA_ABR|CWHILE|CIF|CIN|COUT|CMAS|CMENOS|CIDENT| 
			CPAR_ABR|CNEG|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPYCOMA|CRETURN|folset,
			CELSE | CSHL | CSHR ,52);
	switch(lookahead())
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

	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
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
		case CSHR:
			//scanner();
			match(CIN, 10);
			
			match(CSHR, 10);
			
			variable(folset | CSHR | CIDENT | CPYCOMA);
			//ACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			test(CSHR | folset, CIDENT, 30);
			while(lookahead_in(CSHR|CIDENT)) //Ver si es correcto
			{
				//scanner();
				match(CSHR, 10);
				variable(folset | CSHR | CIDENT | CPYCOMA);
				//ACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
				test(CSHR | folset, CIDENT, 30);
			}

			match(CPYCOMA, 10);
			
			break;
		
		case COUT:
		case CSHL:
			//scanner();
			match(COUT, 10);

			match(CSHL, 10);
			
			expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);


			test(CSHL | folset, CIDENT | CCONS_ENT |CCONS_FLO|CCONS_CAR|CCONS_STR|CMAS|CMENOS|CPAR_ABR|CNEG, 31);
			while(lookahead_in(CSHL|CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CMAS|CMENOS|CPAR_ABR|CNEG)) //Ver si es correcto
			{
				//scanner();
				match(CSHL, 10);
				expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);

				test(CSHL | folset, CIDENT | CCONS_ENT |CCONS_FLO|CCONS_CAR|CCONS_STR|CMAS|CMENOS|CPAR_ABR|CNEG, 31);
			}

			match(CPYCOMA, 10);
			
			break;
		
		default:
			error_handler(29);
	}
	test(folset,0,53);
}


void proposicion_retorno( set folset )
{
	scanner();
	
	expresion(folset | CPYCOMA);
	
	match(CPYCOMA, 10);
	test(folset,0,54);
}


void proposicion_expresion( set folset )
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);

	match(CPYCOMA, 10);
	test(folset,0,55);
}


void expresion( set folset )
{	
	expresion_simple(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | 
									CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG |
									CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG);

	test(CASIGNAC|CDISTINTO|CIGUAL|CMENOR|CMEIG|CMAYOR|CMAIG|folset,0,65);
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
		test(CASIGNAC|CDISTINTO|CIGUAL|CMENOR|CMEIG|CMAYOR|CMAIG|folset,0,65);
	}
}


void expresion_simple( set folset )
{
	test(CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG|CMAS|CMENOS,folset,56); //Punto de reconfiguración
	
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | COR);

	test(CMAS|CMENOS|COR|folset,CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CNEG|CPAR_ABR,69);

	while(lookahead_in(CMAS | CMENOS | COR  |CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CNEG|CPAR_ABR)) 
	{
		if(lookahead_in(CMAS | CMENOS | COR))
			scanner();
		termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | COR);
		test(CMAS|CMENOS|COR|folset,CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CNEG|CPAR_ABR,69); 
	}
}


void termino( set folset )
{	
	factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 	CPAR_ABR | CNEG | CMULT | CDIV | CAND );

	test(CMULT|CDIV|CAND|folset, CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG,58); //ACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	while(lookahead_in(CMULT | CDIV | CAND | CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG))
	{
		if (lookahead_in(CMULT | CDIV | CAND))
			scanner();
		factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 	CPAR_ABR | CNEG | CMULT | CDIV | CAND );
		test(CMULT|CDIV|CAND|folset, CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG,58); //ACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	}
}


void factor( set folset )
{
	test(CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CPAR_ABR|CNEG,folset,57);

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
			expresion(folset|CPAR_CIE);
			match(CPAR_CIE, 10);
			break;
			
		case CNEG:
			scanner();
			expresion(folset);
			break;
			
		default:
			error_handler(32);
	}
	//test(folset,0,58);
}


void variable( set folset )
{
	test(CIDENT, folset | CCOR_ABR | CCOR_CIE, 59);
	match(CIDENT, 10);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		expresion(folset|CCOR_CIE);
		match(CCOR_CIE, 10);
	}
	test(folset,0,60);
}


void llamada_funcion( set folset )
{
	match(CIDENT, 10);
	
	match(CPAR_ABR, 10);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 10);
	test(folset,0,61);
}


void lista_expresiones( set folset )
{
	expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);

	test(CCOMA|folset,CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CMAS|CMENOS|CPAR_ABR|CNEG,64);
	while(lookahead_in(CCOMA|CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CMAS|CMENOS|CPAR_ABR|CNEG))
	{
		//scanner();
		match(CCOMA,64);
		expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			 					CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
		test(CCOMA|folset,CIDENT|CCONS_ENT|CCONS_FLO|CCONS_CAR|CCONS_STR|CMAS|CMENOS|CPAR_ABR|CNEG,64);
	}
}


void constante( set folset )
{
	test(CCONS_ENT|CCONS_FLO|CCONS_CAR, folset, 62);
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
	test(folset,0,63);
}
