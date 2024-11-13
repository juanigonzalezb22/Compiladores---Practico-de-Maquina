#include "parser.h"



int main(int argc, char *argv[])
{
	CODE[0] = INPP;
	GEN = 1;
	init_parser(argc, argv);
	inic_tablas();

	init_arreglo_errores();

	unidad_traduccion(CEOF);

	match(CEOF, 9);
	if (GEN ==1){
		//Debería ir el parar XD
	}
	last_call = 1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}

/********* funciones del parser ***********/

void unidad_traduccion(set folset)
{	
	test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA , 40);
	pushTB();  
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaraciones(CVOID | CCHAR | CINT | CFLOAT | folset);
	}
	if(en_tabla("main") == NIL){
		error_handler(84); 
	}
	pop_nivel(); 
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
		inf_id->ptr_tipo = en_tabla("TIPOERROR");
		error_handler(18);
	}
	test(folset, NADA , 42);
}


void especificador_declaracion(set folset)
{
	test(CPAR_ABR|CASIGNAC|CCOR_ABR|CCOMA|CPYCOMA,folset,43);
	errores_semanticos[16] = 0;
	char aux[strlen(inf_id->nbre)];
	strcpy(aux, inf_id->nbre);
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
		} else {
			errores_semanticos[ERROR_86] = 0;
		}
		if (inf_id->ptr_tipo == en_tabla("char") || inf_id->ptr_tipo== en_tabla("int") || inf_id->ptr_tipo == en_tabla("float")){
			errores_semanticos[ERROR_88] = 1;
		}
		inf_id->cant_byte = 0;
		insertarTS();
		errores_semanticos[ POSICION_FUNCION ] = en_tabla( aux );
		definicion_funcion(folset);
		errores_semanticos[ERROR_88] = 0;
		errores_semanticos[ERROR_89] = 0;
		break;
	case CASIGNAC:
	case CCOR_ABR:
	case CCOMA:
	case CPYCOMA:
		inf_id->clase = CLASVAR;
		//errores_semanticos[ ES_CLASE_VARIABLE ] = 1;
		declaracion_variable(folset);
		//errores_semanticos[ ES_CLASE_VARIABLE ] = 0;
		break;
	default:
		error_handler(19);
	}
}


void definicion_funcion(set folset)
{	
	pushTB();  
	match(CPAR_ABR, 20);
	if (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		if (errores_semanticos[ERROR_86] == 1){ 
			error_handler(86); 
		}

		lista_declaraciones_param( CPAR_CIE | CLLA_ABR| folset); 
	}
	match(CPAR_CIE, 21);
	
	proposicion_compuesta(folset);
	pop_nivel(); 
}


void lista_declaraciones_param(set folset)
{
	int cant_parametros_formales = 1;
	declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);

	while (lookahead_in(CCOMA | CVOID | CCHAR | CINT | CFLOAT))
	{
		match(CCOMA, 64);
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
		cant_parametros_formales++;
	}

	ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par = cant_parametros_formales;

}


void declaracion_parametro(set folset)
{	
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);
	
	if(inf_id->ptr_tipo == en_tabla("void")){
		error_handler(73);
	}

	if (lookahead_in(CAMPER)){
		scanner();
		errores_semanticos[ERROR_92] = 1;
	}
	match(CIDENT, 17);
	if (lookahead_in(CCOR_ABR | CCOR_CIE))
	{
		if (errores_semanticos[ERROR_92] == 1){
			error_handler(92);
		}
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);

		inf_id->desc.part_var.arr.ptero_tipo_base= inf_id->ptr_tipo;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO"); 
	}
	errores_semanticos[ERROR_92] = 0;
	inf_id->clase = CLASPAR;
	inf_id->desc.nivel = get_nivel();
	insertarTS();
	test(folset, NADA ,45);
}


void lista_declaraciones_init(set folset)
{	
	int aux = inf_id->ptr_tipo;
	if(inf_id->ptr_tipo == en_tabla("char")) {
		inf_id->cant_byte = 1;
	} else if (inf_id->ptr_tipo == en_tabla("int")) {
		inf_id->cant_byte = sizeof(int);
	} else if (inf_id->ptr_tipo == en_tabla("float")) {
		inf_id->cant_byte = sizeof(float);
	} else {
		inf_id->cant_byte = 0;
		//error_handler(73);
	}

	test(CIDENT, folset | CCOMA | CASIGNAC | CCOR_ABR, 46);
	match(CIDENT, 17);
	declarador_init( CCOMA | CIDENT | CASIGNAC | CCOR_ABR | folset);
	test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	while (lookahead_in(CCOMA | CIDENT | CASIGNAC | CCOR_ABR))
	{
		inf_id->ptr_tipo = aux;
		if(inf_id->ptr_tipo == en_tabla("char")) {
			inf_id->cant_byte = 1;
		} else if (inf_id->ptr_tipo == en_tabla("int")) {
			inf_id->cant_byte = sizeof(int);
		} else if (inf_id->ptr_tipo == en_tabla("float")) {
			inf_id->cant_byte = sizeof(float);
		} else {
			inf_id->cant_byte = 0;
			//error_handler(73);
		}
		match(CCOMA, 64);
		match(CIDENT, 17);
		declarador_init( CCOMA | CIDENT | CASIGNAC | CCOR_ABR | folset);
		test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	}
}


void declaracion_variable(set folset)
{
	int aux = inf_id->ptr_tipo;
	declarador_init( CCOMA | CIDENT | CPYCOMA | folset );
	if (lookahead_in(CCOMA | CIDENT))
	{
		match(CCOMA, 64);
		inf_id->ptr_tipo = aux;
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
			error_handler(ERROR_82);
		
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
		if( inf_id->desc.part_var.arr.ptero_tipo_base == en_tabla("void") ) {
			inf_id->ptr_tipo = en_tabla("TIPOERROR");
		}
	}

	inf_id->desc.nivel = get_nivel();
	inf_id->clase = CLASVAR;

	if(inf_id->ptr_tipo == en_tabla("void")){
		inf_id->ptr_tipo = en_tabla("TIPOERROR");
		error_handler(73);
	}
	insertarTS();
	
	test(folset, NADA , 48);
}


void lista_inicializadores(set folset)
{
	int cant_iniciadores = 0;	

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
		default: break; 
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
			default: break; 
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
	if (errores_semanticos[ERROR_88] == 1){
		if (errores_semanticos[ERROR_89] == 0 && get_nivel() == 1){
				error_handler(88);
		}
	}
	match(CLLA_CIE, 25);
	test(folset, NADA , 50);
}


void lista_declaraciones(set folset)
{
	//errores_semanticos[ ES_CLASE_VARIABLE ] = 1;
	declaracion(folset | CVOID | CCHAR | CINT | CFLOAT);
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset | CVOID | CCHAR | CINT | CFLOAT);
	//errores_semanticos[ ES_CLASE_VARIABLE ] = 0;
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
		if (errores_semanticos[ERROR_88] == 0){
			error_handler(89);
		}
		errores_semanticos[ERROR_89] = 1;
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
		errores_semanticos[ERROR_94] = 1;
		match(COUT, 29);
		match(CSHL, 31);
		errores_semanticos[DESDE_WHILE_IF] = 1;
		expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);
		if (errores_semanticos[ERROR_96] != 1){
			error_handler(96);
		}
		while (lookahead_in(CSHL))
		{
			scanner();
			expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);
			if (errores_semanticos[ERROR_96] != 1){
				error_handler(96);
			}
		}
		errores_semanticos[DESDE_WHILE_IF] = 0;
		match(CPYCOMA, 23);
		break;

	default:
		error_handler(29);
	}

	test(folset, NADA , 53);
}


void proposicion_retorno(set folset)
{
	errores_semanticos[DESDE_WHILE_IF] = 1;
	scanner();
	expresion(folset | CPYCOMA);
	if (errores_semanticos[ERROR_96] != 1){
		error_handler(96);
	}
	errores_semanticos[DESDE_WHILE_IF] = 0;
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
			if( errores_semanticos[ ES_CLASE_VARIABLE ] == 0 ){
				error_handler(82);
			}	
			else{
				if (!lookahead_in(CCONS_ENT | CCONS_FLO | CCONS_CAR)){
				if (Tipo_Ident(sbol->lexema)!= en_tabla("void") && en_tabla(sbol->lexema)!= NIL && Tipo_Ident(sbol->lexema)!= en_tabla("TIPOERROR")){
					if(Tipo_Ident(sbol->lexema) == en_tabla("TIPOARREGLO")){
						if ((errores_semanticos [ES_CHAR] == 1 || errores_semanticos [ES_ARRCHAR] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base != 1){
							error_handler(83);
						}
						else if ((errores_semanticos [ES_INT] == 1 || errores_semanticos [ES_ARRINT] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 3){
							error_handler(83);
						}
					}
					else{
						if ((errores_semanticos [ES_CHAR] == 1 || errores_semanticos [ES_ARRCHAR] == 1) && Tipo_Ident(sbol->lexema) != en_tabla("char") ){
							error_handler(83);	
						}
						else if ((errores_semanticos [ES_INT] == 1 || errores_semanticos [ES_ARRINT] == 1) && Tipo_Ident(sbol->lexema) == en_tabla("float")){
							error_handler(83);
						}
					}
				}
				else{
					error_handler(83);
				}
				}
			}
			expresion_simple(folset);
			errores_semanticos[ES_ARRFLOAT]=0;
			errores_semanticos[ES_ARRINT]=0;
			errores_semanticos[ES_ARRCHAR]=0;
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
		if (lookahead_in(CMAS | CMENOS | COR)) {
			if (errores_semanticos[ERROR_96] != 1){
				if(lookahead_in(COR))
					error_handler(96);
			}
			scanner();
		} else {
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
	errores_semanticos[ES_FLOAT]=0;
	errores_semanticos[ES_INT]=0;
	errores_semanticos[ES_CHAR]=0;
	errores_semanticos[ ES_CLASE_VARIABLE ] = 0;
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, folset, 57);
	errores_semanticos[ES_VAR_1] = 0;
	switch (lookahead())
	{
	case CIDENT:
		//mostrar_tabla();
		errores_semanticos[ERROR_96] = 0;
		if( Clase_Ident(sbol->lexema) == NIL ){	
			error_handler(71);
			strcpy(inf_id->nbre,sbol->lexema);
			inf_id->ptr_tipo = en_tabla("TIPOERROR"); 
		  inf_id->desc.nivel = get_nivel();
			match(CIDENT, 17);

			errores_semanticos[ FALTA_IDENT ] = 1;
			if( lookahead_in(CPAR_ABR) ){
				llamada_funcion(folset);
			} else {
				inf_id->clase = CLASVAR;
				insertarTS();
				variable(folset);
			}
		} else {
			if ( Clase_Ident(sbol->lexema) == 3) {
				llamada_funcion(folset);
			} else {
				if (errores_semanticos[DESDE_WHILE_IF] == 1 && ( Tipo_Ident(sbol->lexema)!= en_tabla("void") && Tipo_Ident(sbol->lexema)!= en_tabla("TIPOERROR") )){
					errores_semanticos[ERROR_96] = 1;
				}	
				if ( Tipo_Ident(sbol->lexema)!= en_tabla("void") && Tipo_Ident(sbol->lexema)!= en_tabla("TIPOERROR")){
					if(Tipo_Ident(sbol->lexema) == en_tabla("TIPOARREGLO")){
					//	if(en_nivel_actual(sbol->lexema) != NIL){
							if (ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 2){
								errores_semanticos[ES_ARRINT]=1;
							}
							else if (ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 3){
								errores_semanticos[ES_ARRFLOAT]=1;
							}
							else{
								errores_semanticos[ES_ARRCHAR]=1;
							}
					//	}
					}
					else{
					if(Tipo_Ident(sbol->lexema) == en_tabla("float")){
						errores_semanticos[ES_FLOAT]=1;
					}
					else if (Tipo_Ident(sbol->lexema) == en_tabla("int")){ 
						errores_semanticos[ES_INT]=1;
					}
					else{
						errores_semanticos[ES_CHAR]=1;
					}
					}
				}
				match(CIDENT, 17);
				if( lookahead_in(CPAR_ABR) ){
					error_handler(99);
				}
				errores_semanticos[ FALTA_IDENT ] = 1;
				variable(folset);
				errores_semanticos[ ES_CLASE_VARIABLE ] = 1;
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
		if (Tipo_Ident(sbol->lexema) == en_tabla("void") || Clase_Ident(sbol->lexema) == NIL ){
			error_handler(96);
		}
		expresion(folset);
		break;

	default:
		error_handler(32);
	}
	test(folset, NADA , 58);
}


void variable(set folset )
{
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
	char aux[strlen(inf_id->nbre)];
	if( errores_semanticos[ FALTA_IDENT ] == 0 ){
		match(CIDENT, 17);
		if( Clase_Ident(inf_id->nbre) == 3 && (strcmp( ts[Tipo_Ident( inf_id->nbre )].ets->nbre, "TIPOERROR" ) != 0) ){
			errores_semanticos[ POSICION_FUNCION ] = en_tabla( inf_id->nbre );
		} else {
			error_handler(99);
		}
	} else {
		strcpy( aux, inf_id->nbre );
		inf_id->clase = CLASFUNC;
		insertarTS();

		if( Clase_Ident( aux ) == 3 && (strcmp( ts[Tipo_Ident( aux )].ets->nbre, "TIPOERROR" ) != 0) ){
			errores_semanticos[ POSICION_FUNCION ] = en_tabla( aux );
		} else {
			error_handler(99);	
		}
	}

	match(CPAR_ABR, 20);
	if (lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);
	match(CPAR_CIE, 21);
	if( ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par != errores_semanticos[ CANT_PARAMETROS ] ){
		error_handler(90);
	}
	errores_semanticos[ CANT_PARAMETROS ] = 0;
	errores_semanticos[ POSICION_FUNCION ] = 0;
	test(folset, NADA , 61);
}


void lista_expresiones(set folset)
{
	errores_semanticos[ CANT_PARAMETROS ] = 1;
	expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
	test(CCOMA | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG, 64);
	while (lookahead_in(CCOMA | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG))
	{
		match(CCOMA, 64);
		expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
		errores_semanticos[ CANT_PARAMETROS ]++;
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