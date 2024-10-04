#include "parser.h"

//int llamado_factor = 0;

int main(int argc, char *argv[])
{
	init_parser(argc, argv);

	inic_tablas();

	init_arreglo_errores();

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
	if(en_tabla("main") == NIL){
		error_handler(84); //Algo más para hacer?
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
	errores_semanticos[16] = 0;
	switch (lookahead())
	{
	case CPAR_ABR:
		inf_id->clase = CLASFUNC;
		inf_id->desc.nivel = get_nivel();
		if (strcmp(inf_id->nbre,"main") == 0){
			errores_semanticos[ERROR_86] = 1;
			if (inf_id->ptr_tipo != en_tabla("void")){
				error_handler(85);
			}
		}
		insertarTS();
		definicion_funcion(folset);
		break;
	case CASIGNAC:
	case CCOR_ABR:
	case CCOMA:
	case CPYCOMA:
		inf_id->clase = CLASVAR;
		errores_semanticos[ ES_CLASE_VARIABLE ] = 1;
		declaracion_variable(folset);
		errores_semanticos[ ES_CLASE_VARIABLE ] = 0;
		break;
	default:
		error_handler(19);
	}
}


void definicion_funcion(set folset)
{	
	pushTB();  // INICIO BLOQUE LEXICO
	//errores_semanticos [30] = 1;
	match(CPAR_ABR, 20);
	if (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		if (errores_semanticos[ERROR_86] == 1){ 
			error_handler(86); //Damos error e igualmente cargamos los parametros para evitar el error de identificador no declarado
		}
		lista_declaraciones_param( CPAR_CIE | CLLA_ABR| folset); 
	}
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
		if( errores_semanticos[ ES_CLASE_VARIABLE ] == 0 )
			error_handler(82);
		
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
			errores_semanticos[ CANT_ELEM_ARREGLO ] = atoi(lookahead_lexema());
			if( inf_id->desc.part_var.arr.cant_elem == 0 ) {
				error_handler(75);
			}
			constante(CCOR_CIE | CASIGNAC | CLLA_ABR | CCONS_ENT | CCONS_FLO | CCONS_CAR | CLLA_CIE| folset );
			errores_semanticos[ EXISTE_CONS_ENT ] = 1;
		}
		match(CCOR_CIE, 22);

		if(inf_id->ptr_tipo == en_tabla("char")) {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("char");
		} else if (inf_id->ptr_tipo == en_tabla("int")) {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("int");
		} else if (inf_id->ptr_tipo == en_tabla("float")) {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("float");
		} else {
			inf_id->desc.part_var.arr.ptero_tipo_base = en_tabla("TIPOERROR");
			error_handler(73);
		}

		if (lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE))
		{
			match(CASIGNAC, 66);
			match(CLLA_ABR, 24);
			lista_inicializadores( CLLA_CIE | folset);
			match(CLLA_CIE, 25);
		} else {
			if( errores_semanticos[ EXISTE_CONS_ENT ] == 0 ){
				error_handler(74);
			}
		}
		
		inf_id->desc.part_var.arr.cant_elem = errores_semanticos[ CANT_ELEM_ARREGLO ];
		if(inf_id->ptr_tipo == en_tabla("char")) {
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem;
		} else if (inf_id->ptr_tipo == en_tabla("int")) {
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem * sizeof(float);
		} else if (inf_id->ptr_tipo == en_tabla("float")) {
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem * sizeof(float);
		}
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		inf_id->desc.nivel = get_nivel();
		errores_semanticos[ CANT_ELEM_ARREGLO ] = 0;
		errores_semanticos[ EXISTE_CONS_ENT ] = 0;
	}
	if(inf_id->ptr_tipo == en_tabla("void")) {
		error_handler(73);
	}
	insertarTS();
	test(folset, NADA , 48);
}


void lista_inicializadores(set folset)
{
	int cant_iniciadores = 0;	// cantidad de elementos inicializados

	switch (lookahead()) {
		case CCONS_ENT:
			if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("int") )
				errores_semanticos[ ERROR_77 ] = 1;
			cant_iniciadores++;
			break;
		case CCONS_FLO:
			if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("float") )
				errores_semanticos[ ERROR_77 ] = 1; 
			cant_iniciadores++;
			break;
		case CCONS_CAR:
			if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("char") )
				errores_semanticos[ ERROR_77 ] = 1; 
			cant_iniciadores++;
			break;
		default: break; // LA RECUPERACION DE ERRORES SE ENCARGA DE ESTO? 
	}

	constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	while (lookahead_in(CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR))
	{
		match(CCOMA, 64);

		switch (lookahead()) {
			case CCONS_ENT:
				if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("int") )
					errores_semanticos[ ERROR_77 ] = 1;
				cant_iniciadores++;
				break;
			case CCONS_FLO:
				if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("float") )
					errores_semanticos[ ERROR_77 ] = 1;
				cant_iniciadores++;
				break;
			case CCONS_CAR:
				if( inf_id->desc.part_var.arr.ptero_tipo_base != en_tabla("char") )
					errores_semanticos[ ERROR_77 ] = 1;
				cant_iniciadores++;
				break;
			default: break; // LA RECUPERACION DE ERRORES SE ENCARGA DE ESTO? 
		}
		constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	}

	if( errores_semanticos[ EXISTE_CONS_ENT ] == 0 ){
		errores_semanticos[ CANT_ELEM_ARREGLO ] = cant_iniciadores;
	} else if( errores_semanticos[ CANT_ELEM_ARREGLO ] < cant_iniciadores ){
		error_handler(76);
	}
	if (errores_semanticos[ ERROR_77 ] == 1){
		error_handler(77);
		errores_semanticos[ ERROR_77 ] = 0;
	}
}


void proposicion_compuesta(set folset)
{
	test(CLLA_ABR, folset | CLLA_CIE | CVOID | CCHAR | CINT | CFLOAT | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
	 		CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, 49);
	match(CLLA_ABR, 24);
	//if (errores_semanticos[30] == 0){
	//	pushTB();
	//}
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
	//mostrar_tabla();
	//pop_nivel();
	test(folset, NADA , 50);
}


void lista_declaraciones(set folset)
{
	errores_semanticos[ ES_CLASE_VARIABLE ] = 1;
	declaracion(folset | CVOID | CCHAR | CINT | CFLOAT);
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset | CVOID | CCHAR | CINT | CFLOAT);
	errores_semanticos[ ES_CLASE_VARIABLE ] = 0;
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
	errores_semanticos[DESDE_WHILE_IF] = 1;
	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT |
	 						CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);

	match(CPAR_CIE, 21);
	if (errores_semanticos[ERROR_96] != 1){
		error_handler(96);
	}
	errores_semanticos[DESDE_WHILE_IF] = 0;
	pushTB();
	//errores_semanticos[30] = 0;
	proposicion(folset);
	pop_nivel();
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);
	errores_semanticos[DESDE_WHILE_IF] = 1;

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | 
			CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	match(CPAR_CIE, 21);
	if (errores_semanticos[ERROR_96] != 1){
		error_handler(96);
	}
	errores_semanticos[DESDE_WHILE_IF] = 0;
	pushTB();
	//errores_semanticos[30] = 0;
	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | 
											CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);
	pop_nivel();
	if (lookahead_in(CELSE))
	{
		scanner();
		pushTB();
	//	errores_semanticos[30] = 0;
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
		//llamado_factor = 1;
		errores_semanticos[ERROR_94] = 1;
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
		//llamado_factor = 1;
		errores_semanticos[ERROR_94] = 1;
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
			if (errores_semanticos[ERROR_96] != 1){
				if (lookahead_in(CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
					error_handler(96);
			}
			scanner();
			errores_semanticos[ERROR_96] = 0;
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
			if (errores_semanticos[ERROR_96] != 1){
				if(lookahead_in(COR))
					error_handler(96);
			}
			scanner();
		}
		else
		{
			error_handler(65);
		}
		errores_semanticos[ERROR_96] = 0;
		termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | COR);
		test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR, 69);
	}
}


void termino(set folset)
{
	factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMULT | CDIV | CAND);
	while (lookahead_in(CMULT | CDIV | CAND))
	{
		if (lookahead_in(CMULT | CDIV | CAND)){
			if (errores_semanticos[ERROR_96] != 1){
				if(lookahead_in(CAND))
					error_handler(96);
			}
			scanner();
		}
		errores_semanticos[ERROR_96] = 0;
		factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMULT | CDIV | CAND);
	}
}


void factor(set folset)
{
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, folset, 57);

	switch (lookahead())
	{
	case CIDENT:
		errores_semanticos[ERROR_96] = 0;
		if( Clase_Ident(sbol->lexema) == NIL ){
			error_handler(71);
			strcpy(inf_id->nbre,sbol->lexema);
			inf_id->ptr_tipo = en_tabla("TIPOERROR"); 
			inf_id->clase = CLASVAR;
		    inf_id->desc.nivel = get_nivel();
			errores_semanticos[ERROR_96] = 0;
			insertarTS();
			//mostrar_tabla();
			scanner();
			errores_semanticos[ FALTA_IDENT ] = 1;
			if( lookahead_in(CPAR_ABR) ){
				llamada_funcion(folset);
			} else {
				variable(folset);
			}
		} else {
			if ( Clase_Ident(sbol->lexema) == 3) {
				llamada_funcion(folset);
			} else {
				if (errores_semanticos[DESDE_WHILE_IF] == 1 && ( Tipo_Ident(sbol->lexema)!= en_tabla("void") && Tipo_Ident(sbol->lexema)!= en_tabla("TIPOERROR") )){
					errores_semanticos[ERROR_96] = 1;
				}
				variable(folset);
			}
		}
		errores_semanticos[ FALTA_IDENT ] = 0;
		break;
	case CCONS_ENT:
	case CCONS_FLO:
	case CCONS_CAR:
		if (errores_semanticos[DESDE_WHILE_IF] == 1){
			errores_semanticos[ERROR_96] = 1;
		}
		constante(folset);
		break;

	case CCONS_STR:
		if (errores_semanticos[DESDE_WHILE_IF] == 1){
			errores_semanticos[ERROR_96] = 0;
		}
		if (errores_semanticos[ERROR_94] == 0){
			error_handler(94);
		}
		errores_semanticos[ERROR_94] = 0;
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


void variable(set folset )
{
	// A LOS PUNTOS DE RECONFIGURACION DE LOS TEST, NO LE FALTAN LOS FIRST1 DE EXPRESION????
	if( errores_semanticos[ FALTA_IDENT ] == 0 ){
		test(CIDENT, folset | CCOR_ABR | CCOR_CIE | CMAS | CMENOS | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR, 59);
		if (Tipo_Ident( sbol->lexema ) == NIL ) {
			error_handler(71);
		} else {
			if( strcmp( ts[Tipo_Ident( sbol->lexema )].ets->nbre, "TIPOARREGLO" ) != 0 )
		 		errores_semanticos[ ERROR_78 ] = 1;
		}
		match(CIDENT, 17);
	} else {
		test(folset | CCOR_ABR, CCOR_CIE | CMAS | CMENOS | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR, 59); // CHEQUEAR TEST
	}

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if (lookahead_in(CCOR_ABR))
	{
		if( errores_semanticos[ ERROR_78 ] == 1 ){
			error_handler(78);
			errores_semanticos[ ERROR_78 ] = 0;
		}
			
		scanner();
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
	}
	test(folset, NADA , 60);
}


void llamada_funcion(set folset)
{
	if( errores_semanticos[ FALTA_IDENT ] == 0 )
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
