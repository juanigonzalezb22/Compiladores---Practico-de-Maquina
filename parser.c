#include "parser.h"

int contador = 0;
int contador_string = 0;
int desp = 0;
int posvar=0;
int arr_auxi[4];
int lo_anterior = -1;
int laotraauxi = 0;
int vienepar= 0;
int yaHizoIMPRCS = 0;
int AGREGAR_CAST_A = 0;	// 0 = no hacer cast, 1 = int , 2 = float	

int memoriaADevolver = 0;

int main(int argc, char *argv[])
{
	CODE[contador++] = INPP;
	GEN = 1;
	init_parser(argc, argv);
	inic_tablas();

	init_arreglo_errores();

	// HAY QUE INICIALIZAR EL ARREGLO DE LA ETAPA3
	etapa3[CANT_NEGACIONES] = 0;
	etapa3[CANT_RELACIONES1] = 0;
	etapa3[CANT_RELACIONES2] = 0;
	etapa3[PASAJE_POR_REF] = 0;
	etapa3[FLAG_LLAMADA_FUN] = 0;
	//

	unidad_traduccion(CEOF);
	match(CEOF, 9);
	last_call = 1;
	error_handler(COD_IMP_ERRORES);
	if (GEN == 1 ){
		CODE[contador++] = PARAR;
		impr_codgen2();
		//guardar_codgen(CODE,CONSTANTES_STRING,"prueba.txt");
        // Recorre la estructura hasta encontrar /0
        int i;
		for (i = 0; argv[2][i] != '\0'; ++i)
        {
        }
        argv[2][i] = '\0';    // Deja el /0 al final
        argv[2][i - 1] = 'o'; // Cambia la c por la o

        // Se guarda la memoria de programa, constantes string y el .o
        if (guardar_codgen(CODE, CONSTANTES_STRING, argv[2]) == 0)
            printf("No se pudo guardar el archivo.\n");
	}
	return 0;
}

/********* funciones del parser ***********/

void unidad_traduccion(set folset)
{	
	test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA , 40);
	pushTB();
	CODE[contador++] = ENBL;
	CODE[contador++] = get_nivel();
	while (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaraciones(CVOID | CCHAR | CINT | CFLOAT | folset);
	}
	if(en_tabla("main") == NIL){

		error_handler(84); 
		GEN = 0;
	}
	
	if(GEN){
		CODE[contador++] = FINB;
		CODE[contador++] = get_nivel();
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
		GEN = 0;
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
				GEN = 0;
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
		GEN = 0;
	}
}


void definicion_funcion(set folset)
{	
	int desp_aux = desp;
	// int memoriaAux = memoriaADevolver;
	// memoriaADevolver = 0;
	pushTB();  
	if(GEN){
		CODE[contador++] = ENBL;
		CODE[contador++] = get_nivel();
	}
	match(CPAR_ABR, 20);
	if (lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		if (errores_semanticos[ERROR_86] == 1){ 
			error_handler(86);
			GEN = 0;
		}

		lista_declaraciones_param( CPAR_CIE | CLLA_ABR| folset); 
	}
	match(CPAR_CIE, 21);
	
	ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.dir_cod = contador;

	proposicion_compuesta(folset);
	
	
	if(GEN){
		// if( memoriaADevolver > 0 ){
		// 	CODE[contador++] = DMEM;
		// 	CODE[contador++] = memoriaADevolver;
		// }
		CODE[contador++] = FINB;
		CODE[contador++] = get_nivel();
	}
	pop_nivel();
	desp = desp_aux;
	//memoriaADevolver = memoriaAux;
}


void lista_declaraciones_param(set folset)
{
	ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par = 1;
	declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
	while (lookahead_in(CCOMA | CVOID | CCHAR | CINT | CFLOAT))
	{
		match(CCOMA, 64);
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
		ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par++;
	}
}


void declaracion_parametro(set folset)
{	
	tipo_inf_res *nuevo_parametro = (tipo_inf_res *)malloc(sizeof(tipo_inf_res));

	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);
	
	if(inf_id->ptr_tipo == en_tabla("void")){
		error_handler(73);
		GEN = 0;
	}
	
	inf_id->desc.part_var.param.tipo_pje = 'v';		//PASAJE POR VALOR
	if (lookahead_in(CAMPER)){
		scanner();
		errores_semanticos[ERROR_92] = 1;
		inf_id->desc.part_var.param.tipo_pje = 'r'; //PASAJE POR REFERENCIA
		nuevo_parametro->tipo_pje = inf_id->desc.part_var.param.tipo_pje;
	}

	match(CIDENT, 17);
	if (lookahead_in(CCOR_ABR | CCOR_CIE))
	{
		if (errores_semanticos[ERROR_92] == 1){
			error_handler(92);
			GEN = 0;
		}
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);

		inf_id->desc.part_var.param.ptero_tipo_base = inf_id->ptr_tipo;
		nuevo_parametro->ptero_tipo_base = inf_id->ptr_tipo;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO"); 
	}
	nuevo_parametro->ptero_tipo = inf_id->ptr_tipo;
	nuevo_parametro->ptr_sig = NULL;

	errores_semanticos[ERROR_92] = 0;
	inf_id->clase = CLASPAR;
	inf_id->desc.nivel = get_nivel();
	inf_id->desc.despl = desp;
	desp += inf_id->cant_byte;

	//AGREGAMOS EL PARAMETRO A LA FUNCION
	tipo_inf_res *punteroAux = ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.ptr_inf_res;
	if ( punteroAux == NULL) {
		ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.ptr_inf_res = nuevo_parametro;
	} else {
		while ( punteroAux != NULL )
		{
			punteroAux = punteroAux->ptr_sig;
		}
		ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.ptr_inf_res->ptr_sig = nuevo_parametro;
	}

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
	int entro_asignacion = 0;
	switch (lookahead())
	{
	case CASIGNAC:
		match(CASIGNAC,66);
		constante(folset);
		entro_asignacion = 1;
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
				GEN = 0;
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
			GEN = 0; // Detenemos la Generacion de Codigo Intermedio
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
				GEN = 0;
			}
		}
		inf_id->desc.part_var.arr.cant_elem = errores_semanticos[ CANT_ELEM_ARREGLO ];
		if(inf_id->ptr_tipo == en_tabla("char")) {
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem;
		} else if (inf_id->ptr_tipo == en_tabla("int")) {
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem * sizeof(int);
		} else if (inf_id->ptr_tipo == en_tabla("float")) {
			inf_id->cant_byte = inf_id->desc.part_var.arr.cant_elem * sizeof(float);
		}
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		//inf_id->desc.nivel = get_nivel();
		errores_semanticos[ CANT_ELEM_ARREGLO ] = 0;
		errores_semanticos[ EXISTE_CONS_ENT ] = 0;
		if( inf_id->desc.part_var.arr.ptero_tipo_base == en_tabla("void") ) {
			inf_id->ptr_tipo = en_tabla("TIPOERROR");
			GEN = 0; // Detenemos la Generacion de Codigo Intermedio
		}
	}
	inf_id->desc.nivel = get_nivel();
	inf_id->desc.despl = desp;
	desp += inf_id->cant_byte;
	inf_id->clase = CLASVAR;
	if(inf_id->ptr_tipo == en_tabla("void")){
		inf_id->ptr_tipo = en_tabla("TIPOERROR");
		error_handler(73);
		GEN = 0; // Detenemos la Generacion de Codigo Intermedio
	}

	// Para este punto la cant_byte de la variable a cargar ya esta calculada.
	if( GEN == 1 ){
		CODE[contador++] = ALOC ;
		CODE[contador++] = inf_id->cant_byte;
		
		memoriaADevolver += inf_id->cant_byte;
		
		if(entro_asignacion) {
			CODE[contador++] = CRCT ;
			CODE[contador++] = constante_aux;
			
			switch ( etapa3[TIPO_CONSTANTE] )
			{
			case 0:		//CHAR
				CODE[contador++] = 0 ;
				CODE[contador++] = ALM ;
				CODE[contador++] = inf_id->desc.nivel;
				CODE[contador++] = inf_id->desc.despl;
				CODE[contador++] = 0 ;
				break;
			case 1:		//INT
				CODE[contador++] = 1 ;
				CODE[contador++] = ALM ;
				CODE[contador++] = inf_id->desc.nivel;
				CODE[contador++] = inf_id->desc.despl;
				CODE[contador++] = 1 ;
				break;
			case 2:		//FLOAT
				CODE[contador++] = 2 ;
				CODE[contador++] = ALM ;
				CODE[contador++] = inf_id->desc.nivel;
				CODE[contador++] = inf_id->desc.despl;
				CODE[contador++] = 2 ;
				break;
			default: break;
			}
		}
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
		GEN = 0;
	}
	if (errores_semanticos[ ERROR_77 ] == 1){
		error_handler(77);
		GEN = 0;
		errores_semanticos[ ERROR_77 ] = 0;
	}
}


void proposicion_compuesta(set folset)
{
	int memoriaAux = memoriaADevolver;
	memoriaADevolver = 0;

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
			GEN = 0;
		}
	}
	if(GEN){
		if( memoriaADevolver > 0 ){
			CODE[contador++] = DMEM;
			CODE[contador++] = memoriaADevolver;
		}
	}

	match(CLLA_CIE, 25);
	memoriaADevolver = memoriaAux;
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
	int desp_aux = desp;
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
			GEN = 0;
		}
		errores_semanticos[ERROR_89] = 1;
		proposicion_retorno(folset);
		break;

	default:
		error_handler(26);
		GEN = 0;
	}
	desp = desp_aux;
}


void proposicion_iteracion(set folset)
{
	int desplazamientoAux = contador;
	int salto;
	match(CWHILE, 27);

	match(CPAR_ABR, 20);
	etapa3[DESDE_WHILE] = 1;
	errores_semanticos[DESDE_WHILE_IF] = 1;
	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT |
	 						CCONS_FLO | CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);

	match(CPAR_CIE, 21);
	if (errores_semanticos[ERROR_96] != 1){
		error_handler(96);
		GEN = 0;
	}
	
	if(GEN){
		CODE[contador++] = BIFF;
		CODE[contador++] = CODE[contador-2];
		salto = contador;
		contador++;	
	}

	errores_semanticos[DESDE_WHILE_IF] = 0;
	etapa3[DESDE_WHILE] = 0;
	pushTB();
	if(GEN){
		CODE[contador++] = ENBL;
		CODE[contador++] = get_nivel();
	}
	proposicion(folset);

	if(GEN){
		CODE[contador++] = FINB;
		CODE[contador++] = get_nivel();

		CODE[contador++] = BIFS ;
		CODE[contador++] = desplazamientoAux ;
		CODE[salto] = contador;
	}
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
		GEN = 0;
	}
	errores_semanticos[DESDE_WHILE_IF] = 0;
	pushTB();
	if(GEN){
		CODE[contador++] = ENBL;
		CODE[contador++] = get_nivel();
	}
	
	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | 
											CCONS_CAR | CCONS_STR | CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);
	
	if(GEN){
		CODE[contador++] = FINB;
		CODE[contador++] = get_nivel();
	}
	pop_nivel();
	
	if (lookahead_in(CELSE))
	{
		scanner();
		pushTB();
		if(GEN){
			CODE[contador++] = ENBL;
			CODE[contador++] = get_nivel();
		}
		proposicion(folset);
		if(GEN){
			CODE[contador++] = FINB;
			CODE[contador++] = get_nivel();
		}
		pop_nivel();
	}
}


void proposicion_e_s(set folset)
{
	int aux=0;
	switch (lookahead())
	{
	case CIN:
	case CSHR:
		errores_semanticos[ERROR_94] = 1;
		match(CIN, 29);
		match(CSHR, 30);
		if (GEN){
			CODE[contador++]= LEER;
			//printf("%s", ts[en_tabla(lookahead_lexema())].ets->nbre);
			if (ts[en_tabla(lookahead_lexema())].ets->ptr_tipo != en_tabla("TIPOARREGLO")) {
				if (ts[en_tabla(lookahead_lexema())].ets->ptr_tipo == en_tabla("char")){
					CODE[contador++] = 0;
				}
				else if (ts[en_tabla(lookahead_lexema())].ets->ptr_tipo == en_tabla("int")){
					CODE[contador++]= 1;
					aux=1;
				}
				else{
					CODE[contador++]= 2;
					aux=2;
				}
			}
			else{
				if (ts[en_tabla(lookahead_lexema())].ets->desc.part_var.arr.ptero_tipo_base == en_tabla("char")){
					CODE[contador++]= 0;
					aux=0;
				}
				else if (ts[en_tabla(lookahead_lexema())].ets->desc.part_var.arr.ptero_tipo_base == en_tabla("int")){
					CODE[contador++]= 1;
					aux=1;
				}
				else{
					CODE[contador++]= 2;
					aux=2;
				}
			}
			CODE[contador++] = ALM;
			CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.nivel;
			CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.despl; //???
			CODE[contador++] = aux;
		}
		variable(folset | CSHR | CIDENT | CPYCOMA);
		while (lookahead_in(CSHR))
		{
			scanner();
			if (GEN){
				CODE[contador++]= LEER;
				if (ts[en_tabla(lookahead_lexema())].ets->ptr_tipo != en_tabla("TIPOARREGLO")) {
					if (ts[en_tabla(lookahead_lexema())].ets->ptr_tipo == en_tabla("char")){
						CODE[contador++] = 0;
					}
					else if (ts[en_tabla(lookahead_lexema())].ets->ptr_tipo == en_tabla("int")){
						CODE[contador++]= 1;
						aux=1;
					}
					else{
						CODE[contador++]= 2;
						aux=2;
					}
				}
				else{
					if (ts[en_tabla(lookahead_lexema())].ets->desc.part_var.arr.ptero_tipo_base == en_tabla("char")){
						CODE[contador++]= 0;
						aux=0;
					}
					else if (ts[en_tabla(lookahead_lexema())].ets->desc.part_var.arr.ptero_tipo_base == en_tabla("int")){
						CODE[contador++]= 1;
						aux=1;
					}
					else{
						CODE[contador++]= 2;
						aux=2;
					}
				}
				CODE[contador++] = ALM;
				CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.nivel;
				CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.despl; //???
				CODE[contador++] = aux;
			}
			variable(folset | CSHR | CIDENT | CPYCOMA);
		}
		match(CPYCOMA, 23);
		errores_semanticos[ERROR_94] = 0;
		break;

	case COUT:
	case CSHL:
		errores_semanticos[ERROR_94] = 1;
		match(COUT, 29);
		match(CSHL, 31);
		errores_semanticos[DESDE_WHILE_IF] = 1;

		expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);

		if(GEN){
			if(yaHizoIMPRCS == 0){
				CODE[contador++] = IMPR;
				if(errores_semanticos[ES_CHAR]==1 || errores_semanticos[ES_ARRCHAR]==1 || (etapa3[TIPO_CONSTANTE] == 0)){
					CODE[contador++] = 0;
				} else if (errores_semanticos[ES_INT]==1 || errores_semanticos[ES_ARRINT]==1 || (etapa3[TIPO_CONSTANTE] == 1)) {
					CODE[contador++] = 1;
				} else {
					CODE[contador++] = 2;
				}
			} else {
				yaHizoIMPRCS = 0;
			}
		}

		if (errores_semanticos[ERROR_96] != 1){
			error_handler(96);
			GEN = 0;
		}
		while (lookahead_in(CSHL))
		{
			scanner();
			expresion(folset | CSHL | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CPYCOMA);

			if(GEN){
				if(yaHizoIMPRCS == 0){
					CODE[contador++] = IMPR;
					if(errores_semanticos[ES_CHAR]==1 || errores_semanticos[ES_ARRCHAR]==1 || etapa3[TIPO_CONSTANTE] == 0){
						CODE[contador++] = 0;
					} else if (errores_semanticos[ES_INT]==1 || errores_semanticos[ES_ARRINT]==1 || etapa3[TIPO_CONSTANTE] == 1) {
						CODE[contador++] = 1;
					} else {
						CODE[contador++] = 2;
					}
				} else {
					yaHizoIMPRCS = 0;
				}
			}

			if (errores_semanticos[ERROR_96] != 1){
				error_handler(96);
				GEN = 0;
			}
		}
		errores_semanticos[DESDE_WHILE_IF] = 0;
		match(CPYCOMA, 23);
		errores_semanticos[ERROR_94] = 0;
		break;

	default:
		error_handler(29);
		GEN = 0;
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
		GEN = 0;
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
	int valorRelacionAux1,valorRelacionAux2, aux;

	if (etapa3[CANT_RELACIONES1] != 0) {
		valorRelacionAux1 = etapa3[VALOR_RELACION1];
	}
	etapa3[VALOR_RELACION1] = -1;

	expresion_simple(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR |CCONS_STR | CMAS | CMENOS | 
									CPAR_ABR | CNEG | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG);
	
	if( etapa3[CANT_RELACIONES1] > 1 )
		etapa3[VALOR_RELACION1] = valorRelacionAux1;
	etapa3[CANT_RELACIONES1]--;

	test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 65);
	while (lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch (lookahead())
		{
		case CASIGNAC:
			scanner();
			if (GEN){
				arr_auxi[0] = ALM;
				arr_auxi[1] = ts[posvar].ets->desc.nivel;
				arr_auxi[2] = ts[posvar].ets->desc.despl;
				if (ts[posvar].ets->ptr_tipo != en_tabla("TIPOARREGLO")) {
					if (ts[posvar].ets->ptr_tipo == en_tabla("char")){
						arr_auxi[3] = 0;
					}
					else if (ts[posvar].ets->ptr_tipo == en_tabla("int")){
						arr_auxi[3] = 1;
					}
					else{
						arr_auxi[3] = 2;
					}
				}
				else{
					if (ts[posvar].ets->desc.part_var.arr.ptero_tipo_base == en_tabla("char")){
						arr_auxi[3]= 0;
					}
					else if (ts[posvar].ets->desc.part_var.arr.ptero_tipo_base == en_tabla("int")){
						arr_auxi[3]= 1;
					}
					else{
						arr_auxi[3]= 2;
					}
				}
			}
			if( errores_semanticos[ ES_CLASE_VARIABLE ] == 0 ){
				error_handler(82);
				GEN = 0;
			} 
			else 
			{
				if (!lookahead_in(CCONS_ENT | CCONS_FLO | CCONS_CAR)){
					if (Tipo_Ident(sbol->lexema)!= en_tabla("void") && en_tabla(sbol->lexema)!= NIL && Tipo_Ident(sbol->lexema)!= en_tabla("TIPOERROR")){
						if(Tipo_Ident(sbol->lexema) == en_tabla("TIPOARREGLO")){
							if ((errores_semanticos [ES_CHAR] == 1 || errores_semanticos [ES_ARRCHAR] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base != 1){
								error_handler(83);
								GEN = 0;
							} 
							else if ((errores_semanticos [ES_INT] == 1 || errores_semanticos [ES_ARRINT] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 3){
								error_handler(83);
								GEN = 0;
							}
							if	(( errores_semanticos [ES_INT] == 1 || errores_semanticos [ES_ARRINT] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 1)
							{
								AGREGAR_CAST_A = 1;
							}
							if	(( errores_semanticos [ES_FLOAT] == 1 || errores_semanticos [ES_ARRFLOAT] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 1)
							{
								AGREGAR_CAST_A = 2;
							}
							if  (( errores_semanticos [ES_FLOAT] == 1 || errores_semanticos [ES_ARRFLOAT] == 1) && ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 2 )
							{
								AGREGAR_CAST_A = 2;
							}
						}
						else
						{
							if ((errores_semanticos [ES_CHAR] == 1 || errores_semanticos [ES_ARRCHAR] == 1) && Tipo_Ident(sbol->lexema) != en_tabla("char") ){
								error_handler(83);
								GEN = 0;
							}
							else if ((errores_semanticos [ES_INT] == 1 || errores_semanticos [ES_ARRINT] == 1) && Tipo_Ident(sbol->lexema) == en_tabla("float")){
								error_handler(83);
								GEN = 0;
							}
							if	( (errores_semanticos [ES_INT] == 1 || errores_semanticos [ES_ARRINT] == 1) && Tipo_Ident(sbol->lexema) == en_tabla("char"))
							{
								AGREGAR_CAST_A = 1;
							}
							if	( (errores_semanticos [ES_FLOAT] == 1 || errores_semanticos [ES_ARRFLOAT] == 1) && Tipo_Ident(sbol->lexema) == en_tabla("char"))
							{
								AGREGAR_CAST_A = 2;
							}
							if	( (errores_semanticos [ES_FLOAT] == 1 || errores_semanticos [ES_ARRFLOAT] == 1) && Tipo_Ident(sbol->lexema) == en_tabla("int"))
							{
								AGREGAR_CAST_A = 2;
							}
						}
					}
					else{
						error_handler(83);
						GEN = 0;
					}
				}
			}
			expresion_simple(folset);
			AGREGAR_CAST_A = 0;
			CODE[contador++] = arr_auxi[0];
			CODE[contador++] = arr_auxi[1];
			CODE[contador++] = arr_auxi[2];
			CODE[contador++] = arr_auxi[3];
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
			if(GEN) {
				switch (lookahead()) 
				{
					case CDISTINTO: aux = 0; 	break;
					case CIGUAL: 	aux = 1; 	break;
					case CMENOR: 	aux = 2; 	break;
					case CMEIG: 	aux = 3; 	break;
					case CMAYOR:	aux = 4; 	break;
					case CMAIG:		aux = 5; 	break;
					default:break;
				}
			}
			if (errores_semanticos[ERROR_96] != 1) {
				if (lookahead_in(CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG)) {
					error_handler(96);
					GEN = 0;
				}
			}
			scanner();
			errores_semanticos[ERROR_96] = 0;

			if (!etapa3[CANT_RELACIONES2] == 0) {
				valorRelacionAux2 = etapa3[VALOR_RELACION2];
			}
			etapa3[VALOR_RELACION2] = -1;
			etapa3[CANT_RELACIONES2]++;
			
			expresion_simple(folset);

			if(GEN) {
				switch (aux)
				{
					case 0: 	CODE[contador++] = CMNI; 	break;
					case 1: 	CODE[contador++] = CMIG; 	break;
					case 2: 	CODE[contador++] = CMME; 	break;
					case 3: 	CODE[contador++] = CMEI; 	break;
					case 4:		CODE[contador++] = CMMA; 	break;
					case 5:		CODE[contador++] = CMAI; 	break;
					default:break;
				}
				if( etapa3[VALOR_RELACION1] > etapa3[VALOR_RELACION2]){
					CODE[contador++] = etapa3[VALOR_RELACION1];
				} else {
					CODE[contador++] = etapa3[VALOR_RELACION2];
				}
			}
			if( etapa3[CANT_RELACIONES2] > 1 )
				etapa3[VALOR_RELACION2] = valorRelacionAux2;
			etapa3[CANT_RELACIONES2]--;
			break;
		}
		test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 65);
	}
}


void expresion_simple(set folset)
{
	int sum_resta = 0;
	int tipo_op, tipo_opauxi;
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMAS | CMENOS, folset, 56);
	if (lookahead_in(CMAS | CMENOS)){
		if (lookahead_in(CMENOS)){
			laotraauxi = 1;
		}
		//if (lookahead_in(CMAS)){
		//	sum_resta = 1;
		//}
		//else{
		//	sum_resta = 2;
		//}
		scanner();
	}
	termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | COR);
	if (GEN){
	//	if (sum_resta == 1){
	//		CODE[contador++] = SUM;
	//			CODE[contador++] = 2;
	//	}
	//	else if (sum_resta == 2){
	//		CODE[contador++] = SUB;
	//		CODE[contador++] = 0;
	//	}
		if (lo_anterior == 2){
			tipo_opauxi = 2;
		}
		else if (lo_anterior == 1){
			tipo_opauxi = 1;
		}
		else{
			tipo_opauxi = 0;
		}
	}
	test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR, 69);
	while (lookahead_in(CMAS | CMENOS | COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR))
	{
		if (lookahead_in(CMAS | CMENOS | COR)) {
			if (errores_semanticos[ERROR_96] != 1){
				if(lookahead_in(COR)){
					error_handler(96);
					GEN = 0;
				}
			}
			if (lookahead_in(CMAS)){
				sum_resta = 1;
			}
			else{
				sum_resta = 2;
			}
			scanner();
		} else {
			error_handler(65);
			GEN = 0;
		}
		errores_semanticos[ERROR_96] = 0;
		termino(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | COR);
		if (GEN){
			if (sum_resta == 1){
				CODE[contador++] = SUM;
			}
			else if (sum_resta == 2){
				CODE[contador++] = SUB;
			}
			if (lo_anterior == 2){
				tipo_op = 2;
			}
			else if (lo_anterior == 1){
				tipo_op = 1;
			}
			else{
				tipo_op = 0;
			}
			if (tipo_opauxi < tipo_op){
				CODE[contador++] = tipo_op;
			}
			else{
				CODE[contador++] = tipo_opauxi;
			}
		}
		test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CNEG | CPAR_ABR, 69);
	}
}

void termino(set folset)
{
	int mul_div = 0;
	int tipo_op, tipo_opauxi;
	factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMULT | CDIV | CAND);
	if (GEN){
	//	if (sum_resta == 1){
	//		CODE[contador++] = SUM;
	//			CODE[contador++] = 2;
	//	}
	//	else if (sum_resta == 2){
	//		CODE[contador++] = SUB;
	//		CODE[contador++] = 0;
	//	}
		if (lo_anterior == 2){
			tipo_opauxi = 2;
		}
		else if (lo_anterior == 1){
			tipo_opauxi = 1;
		}
		else{
			tipo_opauxi = 0;
		}
		if (laotraauxi ==1 && vienepar ==0) {
			CODE[contador++] = INV;
			CODE[contador++] = tipo_opauxi;
			laotraauxi = 0;
		}
	}
	while (lookahead_in(CMULT | CDIV | CAND))
	{
		if (errores_semanticos[ERROR_96] != 1){
			if(lookahead_in(CAND)){
				error_handler(96);
				GEN = 0;
			}
		}
		if (lookahead_in(CMULT)){
			mul_div = 1;
		}
		else{
			mul_div = 2;
		}
		scanner();
		errores_semanticos[ERROR_96] = 0;
		factor(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG | CMULT | CDIV | CAND);
		if(GEN){
			if (mul_div == 1){
				CODE[contador++] = MUL;
			}
			else if (mul_div == 2){
				CODE[contador++] = DIV;
			}	
			if (lo_anterior == 2){
				tipo_op = 2;
			}
			else if (lo_anterior == 1){
				tipo_op = 1;
			}
			else{
				tipo_op = 0;
			}
			if (tipo_opauxi < tipo_op){
				CODE[contador++] = tipo_op;
			}
			else{
				CODE[contador++] = tipo_opauxi;
			}
		}
	}
}


void factor(set folset)
{
	int valorNegAux;
	errores_semanticos[ES_FLOAT]=0;
	errores_semanticos[ES_INT]=0;
	errores_semanticos[ES_CHAR]=0;
	errores_semanticos[ ES_CLASE_VARIABLE ] = 0;
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, folset, 57);
	errores_semanticos[ES_VAR_1] = 0;
	lo_anterior = -1;
	switch (lookahead())
	{
	case CIDENT:
		errores_semanticos[ERROR_96] = 0;
		if( Clase_Ident(sbol->lexema) == NIL ){	
			error_handler(71);
			GEN = 0;
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
						if (ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 2){
							lo_anterior = 1;
							errores_semanticos[ES_ARRINT]=1;
							if(etapa3[VALOR_NEG] < 1)
								etapa3[VALOR_NEG] = 1;
							if(etapa3[VALOR_RELACION1] < 1)
								etapa3[VALOR_RELACION1] = 1;
							if(etapa3[VALOR_RELACION2] < 1)
								etapa3[VALOR_RELACION2] = 1;
						}
						else if (ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base == 3){
							lo_anterior = 2;
							errores_semanticos[ES_ARRFLOAT]=1;
							if(etapa3[VALOR_NEG] < 2)
								etapa3[VALOR_NEG] = 2;
							if(etapa3[VALOR_RELACION1] < 2)
								etapa3[VALOR_RELACION1] = 2;
							if(etapa3[VALOR_RELACION2] < 2)
								etapa3[VALOR_RELACION2] = 2;
						}
						else{
							lo_anterior = 0;
							errores_semanticos[ES_ARRCHAR]=1;
							if(etapa3[VALOR_NEG] < 0)
								etapa3[VALOR_NEG] = 0;
							if(etapa3[VALOR_RELACION1] < 0)
								etapa3[VALOR_RELACION1] = 0;
							if(etapa3[VALOR_RELACION2] < 0)
								etapa3[VALOR_RELACION2] = 0;
						}
					}
					else
					{
						if(Tipo_Ident(sbol->lexema) == en_tabla("float")){
							lo_anterior = 2;
							errores_semanticos[ES_FLOAT]=1;
							if(etapa3[VALOR_NEG] < 2)
								etapa3[VALOR_NEG] = 2;
							if(etapa3[VALOR_RELACION1] < 2)
								etapa3[VALOR_RELACION1] = 2;
							if(etapa3[VALOR_RELACION2] < 2)
								etapa3[VALOR_RELACION2] = 2;
						}
						else if (Tipo_Ident(sbol->lexema) == en_tabla("int")){ 
							lo_anterior = 1;
							errores_semanticos[ES_INT]=1;
							if(etapa3[VALOR_NEG] < 1)
								etapa3[VALOR_NEG] = 1;
							if(etapa3[VALOR_RELACION1] < 1)
								etapa3[VALOR_RELACION1] = 1;
							if(etapa3[VALOR_RELACION2] < 1)
								etapa3[VALOR_RELACION2] = 1;
						}
						else{
							lo_anterior = 0;
							errores_semanticos[ES_CHAR]=1;
							if(etapa3[VALOR_NEG] < 0)
								etapa3[VALOR_NEG] = 0;
							if(etapa3[VALOR_RELACION1] < 0)
								etapa3[VALOR_RELACION1] = 0;
							if(etapa3[VALOR_RELACION2] < 0)
								etapa3[VALOR_RELACION2] = 0;
						}
					}
				}
				if(GEN){
					// if( etapa3[FLAG_LLAMADA_FUN] == 0 || (etapa3[FLAG_LLAMADA_FUN] == 1 && etapa3[PASAJE_POR_REF] == 0))
					// {
						CODE[contador++] = CRVL;
						CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.nivel;
						CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.despl;
						posvar = en_tabla(lookahead_lexema());
						if(errores_semanticos[ES_CHAR]==1 || errores_semanticos[ES_ARRCHAR]==1){
							CODE[contador++] = 0;
							if(AGREGAR_CAST_A != 0){
								CODE[contador++] = CAST;
								CODE[contador++] = 0;
								CODE[contador++] = AGREGAR_CAST_A;
							}
						} else if (errores_semanticos[ES_INT]==1 || errores_semanticos[ES_ARRINT]==1)
						{
							CODE[contador++] = 1;
							if(AGREGAR_CAST_A != 0){
								CODE[contador++] = CAST;
								CODE[contador++] = 1;
								CODE[contador++] = AGREGAR_CAST_A;
							}
						} else {
							CODE[contador++] = 2;
						}
					//} 
					// else {
					// 	CODE[contador++] = CRDI;
					// 	CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.nivel;
					// 	CODE[contador++] = ts[en_tabla(lookahead_lexema())].ets->desc.despl;
					// }
				}

				match(CIDENT, 17);
				if( lookahead_in(CPAR_ABR) ){
					error_handler(99);
					GEN = 0;
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
		if (errores_semanticos[DESDE_WHILE_IF] == 1)
			errores_semanticos[ERROR_96] = 1;
		constante(folset);
		switch ( etapa3[TIPO_CONSTANTE] )
		{
			case 0: if(etapa3[VALOR_NEG] < 0) etapa3[VALOR_NEG] = 0;
					if(etapa3[VALOR_RELACION1] < 0) etapa3[VALOR_RELACION1] = 0; 
					if(etapa3[VALOR_RELACION2] < 0) etapa3[VALOR_RELACION2] = 0; 
					break;
			case 1: if(etapa3[VALOR_NEG] < 1) etapa3[VALOR_NEG] = 1;
					if(etapa3[VALOR_RELACION1] < 1) etapa3[VALOR_RELACION1] = 1;  
					if(etapa3[VALOR_RELACION2] < 1) etapa3[VALOR_RELACION2] = 1;  
					break;
			case 2:	if(etapa3[VALOR_NEG] < 2) etapa3[VALOR_NEG] = 2; 
					if(etapa3[VALOR_RELACION1] < 2) etapa3[VALOR_RELACION1] = 2;
					if(etapa3[VALOR_RELACION2] < 2) etapa3[VALOR_RELACION2] = 2;  
					break;
			default: break;
		}
		if(GEN){
			CODE[contador++] = CRCT ;
			CODE[contador++] = constante_aux;
			switch ( etapa3[TIPO_CONSTANTE] )
			{
				case 0: CODE[contador++] = 0; break;
				case 1: CODE[contador++] = 1; break;
				case 2: CODE[contador++] = 2; break;
				default: break;
			}
		}
		break;

	case CCONS_STR:
		if (errores_semanticos[DESDE_WHILE_IF] == 1){
			errores_semanticos[ERROR_96] = 1;
		}
		if (errores_semanticos[ERROR_94] == 0){
			error_handler(94);
			GEN = 0;
		}

		if(GEN){
			yaHizoIMPRCS = 1;
			CODE[contador++] = CRCTS;
			CODE[contador++] = contador_string;
			CODE[contador++] = IMPRCS;
			for (int i = 0; i < strlen(sbol->lexema); i++){
				CONSTANTES_STRING[contador_string++] = sbol->lexema[i]; //CARGAMOS EL STRING EN LA ZONA DE CONSTANTES STRING
			}
			CONSTANTES_STRING[contador_string++] = '\0'; 				// AGREGAMOS MARCA DE FIN A LA CONSTANTE STRING
		}
		scanner();
		break;

	case CPAR_ABR:
		vienepar = 1;
		scanner();
		expresion(folset | CPAR_CIE);
		match(CPAR_CIE, 21);
		vienepar = 0;
		break;

	case CNEG:
		scanner();
		// if (Tipo_Ident(sbol->lexema) == en_tabla("void") || Clase_Ident(sbol->lexema) == NIL ){
		// 	error_handler(96);
		// 	GEN = 0;
		// }
		if( !etapa3[CANT_NEGACIONES] == 0 )
			valorNegAux = etapa3[VALOR_NEG];
		etapa3[VALOR_NEG] = -1;
		etapa3[CANT_NEGACIONES]++;
		expresion(folset);
		if(GEN)
		{
			CODE[contador++] = NEG;
			CODE[contador++] = etapa3[VALOR_NEG];
		}
		if( etapa3[CANT_NEGACIONES] > 1 )
			etapa3[VALOR_NEG] = valorNegAux;
		etapa3[CANT_NEGACIONES]--;
		break;

	default:
		error_handler(32);
		GEN = 0;
	}
	test(folset, NADA , 58);
}


void variable(set folset )
{
	if( errores_semanticos[ FALTA_IDENT ] == 0 ){
		test(CIDENT, folset | CCOR_ABR | CCOR_CIE | CMAS | CMENOS | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR, 59);
		if (Tipo_Ident( sbol->lexema ) == NIL ) {
			error_handler(71);
			GEN = 0;
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
			GEN = 0;
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
			GEN = 0;
		}
		
	} else {
		strcpy( aux, inf_id->nbre );
		inf_id->clase = CLASFUNC;
		insertarTS();

		if( Clase_Ident( aux ) == 3 && (strcmp( ts[Tipo_Ident( aux )].ets->nbre, "TIPOERROR" ) != 0) ){
			errores_semanticos[ POSICION_FUNCION ] = en_tabla( aux );
		} else {
			error_handler(99);	
			GEN = 0;
		}
	}

	match(CPAR_ABR, 20);
	if (lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);
	match(CPAR_CIE, 21);
	if( ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par != errores_semanticos[ CANT_PARAMETROS ] ){
		error_handler(90);
		GEN = 0;
	}
	errores_semanticos[ CANT_PARAMETROS ] = 0;
	errores_semanticos[ POSICION_FUNCION ] = 0;
	test(folset, NADA , 61);
}


void lista_expresiones(set folset)
{
	etapa3[FLAG_LLAMADA_FUN] = 1;
	//tipo_inf_res *punteroAux;
	errores_semanticos[ CANT_PARAMETROS ] = 1;
	// if( ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par >= errores_semanticos[ CANT_PARAMETROS ] ){
	// 	punteroAux = ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.ptr_inf_res;
	// 	if( punteroAux->tipo_pje == 'r' ){
	// 		etapa3[PASAJE_POR_REF] = 1;
	// 	} else {
	// 		etapa3[PASAJE_POR_REF] = 0;
	// 	}
	// }
	expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
	test(CCOMA | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG, 64);
	while (lookahead_in(CCOMA | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG))
	{
		errores_semanticos[ CANT_PARAMETROS ]++;
		// if( ts[errores_semanticos[ POSICION_FUNCION ]].ets->desc.part_var.sub.cant_par >= errores_semanticos[ CANT_PARAMETROS ] ){
		// 	punteroAux = punteroAux->ptr_sig;
		// 	if( punteroAux->tipo_pje == 'r' ){
		// 		etapa3[PASAJE_POR_REF] = 1;
		// 	} else {
		// 		etapa3[PASAJE_POR_REF] = 0;
		// 	}
		// }
		match(CCOMA, 64);
		expresion(folset | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG | CCOMA);
		test(CCOMA | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CMAS | CMENOS | CPAR_ABR | CNEG, 64);
	}
	etapa3[FLAG_LLAMADA_FUN] = 0;
}


void constante(set folset)
{
	test(CCONS_ENT | CCONS_FLO | CCONS_CAR, folset, 62);
	
	char* ptr = lookahead_lexema();

    char primerCaracter = *(ptr+1);

	switch (lookahead())
	{
	case CCONS_ENT:
		constante_aux = (float) atoi(lookahead_lexema());
		etapa3[TIPO_CONSTANTE] = 1;
		lo_anterior = 1;
		scanner();
		break;

	case CCONS_FLO:
		constante_aux = atof(lookahead_lexema());
		etapa3[TIPO_CONSTANTE] = 2;
		lo_anterior = 2;
		scanner();
		break;

	case CCONS_CAR:
		constante_aux = (float) primerCaracter;
		etapa3[TIPO_CONSTANTE] = 0;
		lo_anterior = 0;
		scanner();
		break;

	default:
		error_handler(33);
		GEN = 0;
	}
	test(folset, NADA , 63);
}