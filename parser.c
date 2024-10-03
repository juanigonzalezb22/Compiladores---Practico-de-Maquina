#include "parser.h"

int main(int argc, char *argv[])
{
	init_parser(argc, argv);

	inic_tablas();

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
	pushTB();  // Inicia un nuevo bloque léxico ???
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaraciones(CVOID | CCHAR | CINT | CFLOAT | folset);
	}
	pop_nivel(); // Terminar el bloque léxico ???
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
		inf_id->ptr_tipo = en_tabla("void"); 
		inf_id->cant_byte = 0;
		scanner();
		break;

	case CCHAR:
		inf_id->ptr_tipo = en_tabla("char"); 
		inf_id->cant_byte = 1;
		scanner();
		break;

	case CINT:
		inf_id->ptr_tipo = en_tabla("int"); 
		inf_id->cant_byte = sizeof(int);
		scanner();
		break;

	case CFLOAT:
		inf_id->ptr_tipo = en_tabla("float"); 
		inf_id->cant_byte = sizeof(float);
		scanner();
		break;

	default:
		inf_id->ptr_tipo = en_tabla("TIPOERROR"); // va aca?
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
		inf_id->clase = CLASFUNC;
		inf_id->desc.nivel = get_nivel();
		insertarTS();
		definicion_funcion(folset);
		break;
	case CASIGNAC:
	case CCOR_ABR:
	case CCOMA:
	case CPYCOMA:
		inf_id->clase = CLASVAR;
		declaracion_variable(folset);
		break;
	default:
		error_handler(19);
	}
}


void definicion_funcion(set folset)
{	
	pushTB();  // INICIO BLOQUE LEXICO

	match(CPAR_ABR, 20);

	if (lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param( CPAR_CIE | CLLA_ABR| folset);

	match(CPAR_CIE, 21);

	proposicion_compuesta(folset);

	pop_nivel(); // TERMINA BLOQUE LEXICO
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
		inf_id->desc.part_var.arr.ptero_tipo_base= inf_id->ptr_tipo;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO"); 
		//COMPLETAR
	}

	inf_id->clase = CLASPAR;
	inf_id->desc.nivel = get_nivel();
	insertarTS();
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

	test(folset, NADA , 51); 
}


void declarador_init(set folset)
{	
test(CASIGNAC | CCOR_ABR | folset, CCOR_CIE| CLLA_ABR | CLLA_CIE, 47);
	switch (lookahead())
	{
	case CASIGNAC:
		match(CASIGNAC,66);
		constante(folset);
		break;
	case CCOR_ABR:
	case CCOR_CIE:
	case CLLA_ABR:
	case CLLA_CIE:
		match(CCOR_ABR, 35);
		if (lookahead_in(CCONS_ENT)){
			inf_id->desc.part_var.arr.cant_elem = atoi(lookahead_lexema());
			if( inf_id->desc.part_var.arr.cant_elem == 0 ) {
				error_handler(75);
			}
				
			constante(CCOR_CIE | CASIGNAC | CLLA_ABR | CCONS_ENT | CCONS_FLO | CCONS_CAR | CLLA_CIE| folset );
		} else
			error_handler(74);	// ESTE ERROR DEBERIA SER MARCADO ACA? FALTA ALGUN CONTROL?

		match(CCOR_CIE, 22);
		// if(inf_id->ptr_tipo == en_tabla("char") || inf_id->ptr_tipo == en_tabla("int") || inf_id->ptr_tipo == en_tabla("float"))
		// 	inf_id->desc.part_var.arr.ptero_tipo_base= inf_id->ptr_tipo;
		// else {
		// 	error_handler(73);
		// }

		//SI CAMBIAMOS DESPUES EL ptr_tipo PERDEMOS LA INFORMACION? POR LAS DUDAS HICE EL QUE SIGUE xd
		if(inf_id->ptr_tipo == en_tabla("char")) {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("char");
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem;
		} else if (inf_id->ptr_tipo == en_tabla("int")) {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("int");
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem * sizeof(float);
		} else if (inf_id->ptr_tipo == en_tabla("float")) {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("float");
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem * sizeof(float);
		} else {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("TIPOERROR");
			error_handler(73);
		}

		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		//Lo damos de alta igual??? Se, no deberia haber problema, no? xd

		if (lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE))
		{
			match(CASIGNAC, 66);
			match(CLLA_ABR, 24);
			lista_inicializadores( CLLA_CIE | folset );
			match(CLLA_CIE, 25);
		}
		break;
	}
	inf_id->desc.nivel = get_nivel();
	insertarTS();
	test(folset, NADA , 48);
}


void lista_inicializadores(set folset)
{
	int error = 0;		// 0 = los inicializadores son del tipo correccto
										// 1 = los inicializadores no son del tipo correccto
	int cant_aux = 0;	// cantidad de elementos inicializados

	switch (lookahead()) {
		case CCONS_ENT:
			if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("int") )
				error = 1; // PARA NO MOSTRAR EL CARTEL CADA VEZ QUE NO COINCIDE
			cant_aux++;
			break;
		case CCONS_FLO:
			if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("float") )
				error = 1; // PARA NO MOSTRAR EL CARTEL CADA VEZ QUE NO COINCIDE
			cant_aux++;
			break;
		case CCONS_CAR:
			if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("char") )
				error = 1; // PARA NO MOSTRAR EL CARTEL CADA VEZ QUE NO COINCIDE
			cant_aux++;
			break;
		default:break; // LA RECUPERACION DE ERRORES SE ENCARGA DE ESTO? 
	}

	constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	while (lookahead_in(CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR))
	{
		match(CCOMA, 64);

		switch (lookahead()) {
			case CCONS_ENT:
				if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("int") )
					error = 1; // PARA NO MOSTRAR EL CARTEL CADA VEZ QUE NO COINCIDE
				cant_aux++;
				break;
			case CCONS_FLO:
				if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("float") )
					error = 1; // PARA NO MOSTRAR EL CARTEL CADA VEZ QUE NO COINCIDE
				cant_aux++;
				break;
			case CCONS_CAR:
				if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("char") )
					error = 1; // PARA NO MOSTRAR EL CARTEL CADA VEZ QUE NO COINCIDE
				cant_aux++;
				break;
			default:break; // LA RECUPERACION DE ERRORES SE ENCARGA DE ESTO? 
		}

		constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	}

	if( inf_id->desc.part_var.arr.cant_elem < cant_aux )
		error_handler(76);	
		// HAY QUE HACER ALGUN CAMBIO EN LA TABLA DE SIMBOLOS O ALGO? PUES, HAY MAS VALORES QUE ESPACIO EN EL ARREGLO.
	if (error == 1)
		error_handler(77);
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

	test(folset, NADA , 51);
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
	pushTB();
	proposicion(folset);
	pop_nivel();
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | 
			CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	match(CPAR_CIE, 21);
	pushTB();
	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | 
											CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);
	pop_nivel();
	if (lookahead_in(CELSE))
	{
		scanner();
		pushTB();
		proposicion(folset);
		pop_nivel();
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
		variable(folset | CSHR | CIDENT | CPYCOMA, 0);
		while (lookahead_in(CSHR))
		{
			scanner();
			variable(folset | CSHR | CIDENT | CPYCOMA, 0);
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
		if( Clase_Ident(sbol->lexema) == NIL ){
			error_handler(71);
			strcpy(inf_id->nbre,sbol->lexema);
			inf_id->ptr_tipo = en_tabla("TIPOERROR"); 
			inf_id->clase = CLASVAR;
		  inf_id->desc.nivel = get_nivel();
			insertarTS();
			//mostrar_tabla();
			scanner();
			if( lookahead_in(CPAR_ABR) ){
				llamada_funcion(folset, 1);
			} else {
				variable(folset, 1);
			}
		} else {
			if ( Clase_Ident(sbol->lexema) == 3) {
				llamada_funcion(folset, 0); 	// TIPO FUNCION = 3
			} else {
				variable(folset, 0); 				// TIPO VARIABLE = 2
			}
		}
		// CONSULTAR SI ESTA BIEN
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


void variable(set folset, int falta_ident )
{
	int aux = 0;

	// A LOS PUNTOS DE RECONFIGURACION DE LOS TEST, NO LE FALTAN LOS FIRST1 DE EXPRESION????

	if( falta_ident == 0 ){
		test(CIDENT, folset | CCOR_ABR | CCOR_CIE, 59);

		if( strcmp( ts[Tipo_Ident( sbol->lexema )].ets->nbre, "TIPOARREGLO" ) == 0 )
			aux = 1;

		match(CIDENT, 17);
	} else {
		test(folset | CCOR_ABR, CCOR_CIE, 59); // CHEQUEAR TEST
		aux = 1; 	// SI EL IDENT NO ESTA DECLARADO, HACE FALTA MOSTRAR QUE 
							// NO ES TIPO ARREGLO EN CASO DE QUE TENGA CORCHETES???
	}

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if (lookahead_in(CCOR_ABR))
	{
		if( aux == 0 )
			error_handler(78);
		scanner();
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
	}
	test(folset, NADA , 60);
}


void llamada_funcion(set folset, int falta_ident )
{
	if( falta_ident == 0 )
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
