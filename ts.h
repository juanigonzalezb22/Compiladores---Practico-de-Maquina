/*
======================================================================
	DISE�O Y CONSTRUCCI�N DE COMPILADORES
	PROGRAMAS FUENTES ENTREGADOS POR LA C�TEDRA
        HEADER PARA EL ADMINISTRADOR DE TABLA DE S�MBOLOS
======================================================================
*/

/*
=====================================================================
	 Definiciones de constantes usadas por la tabla de s�mbolos
=====================================================================
*/

# define TAM_ID     15
# define TAM_TS    200
# define TAM_HASH  101
# define TAM_BLOQ   30
# define BASE_TS    -1
# define BASE_TB    -1
# define NIL        -1         // indica que no apunto a ning�n tipo


// ========== Definici�n de las clases de identificadores ==========

			// CLASES CORRESPONDIENTES A:

#define CLASTYPE   1    // identificador de tipo
#define CLASVAR    2    // identificador de variable
#define CLASFUNC   3    // identificador de funci�n
#define CLASPAR    4    // identificador de par�metro formal

/*

=====================================================================
 Definiciones de tipos del administrador de tabla de s�mbolos
=====================================================================
*/

/*   LAS SIGUIENTES ESTRUCTURAS FORMAN PARTE DEL DESCRIPTOR
     DE UNA ENTRADA EN LA TABLA DE S�MBOLOS         */

// ------------------------- TIPO_INF_RES --------------------------
// Estructura que forma la lista con informaci�n resumida de par�metros

 typedef struct inf_res {
	int ptero_tipo;					// tipo del par�metro
	char tipo_pje;						// tipo de pasaje: valor o referencia
	int ptero_tipo_base;				// descriptor para cuando el par�metro es de tipo arreglo
	struct inf_res  *ptr_sig;		// siguiente par�metro
} tipo_inf_res;

// ---------------------------- STC_TS -----------------------------
// Estructura propia de: variables (an�nimas), subrutinas y par�metros

typedef struct {
	int nivel;  						// nivel lexicogr�fico del objeto computacional
	int despl; 							// desplazamiento dentro del registro de activaci�n usado solamente
											// para identificadores de clase "variable" y  "par�metro"
	union {
		struct {							// si es un identificador de par�metro
		char tipo_pje;					// tipo de pasaje de este par�metro
		int ptero_tipo_base;			// descriptor para cuando el par�metro es de tipo arreglo
		} param;
		
		struct { 								// si es un identificador de funci�n
			int dir_cod;  						// direcci�n del c�digo
			int cant_par;  					// cantidad de par�metros
			tipo_inf_res *ptr_inf_res;		// puntero a la lista que mantiene informaci�n de los par�metros
		} sub;
	
		struct {									// si es una variable de tipo arreglo
			int ptero_tipo_base;				// tipo de los elementos
			int cant_elem;						// cantidad de elementos
		} arr;					

	} part_var;  						// fin de la uni�n de la parte variante

} stc_TS;


/* LA SIGUIENTE ESTRUCTURA FORMA UNA ENTRADA EN LA TABLA DE S�MBOLOS*/

// -------------------------- ENTRADA_TS ---------------------------
//          Estructura de una entrada en la tabla de s�mbolos

typedef struct {

	char nbre[TAM_ID];	// nombre del identificador

	int clase;				// clase de objeto computacional: vble., funci�n, par�m., tipo.

	int ptr_tipo;      	// puntero al tipo del identificador, apunta a la tabla de s�mbolos
								// tambi�n sirve para indicar el tipo del valor retornado por una funci�n

	int cant_byte;     	// cantidad de bytes del objeto computacional

	stc_TS desc;      	// descriptor de variable, funci�n o par�metro

} entrada_TS;


// ---------------------------- TIPO_TS ----------------------------
//          Estructura que define el tipo de la tabla de s�mbolos

typedef struct {

	int ptr_sinon;			// puntero que encadena los sin�nimos
	entrada_TS  *ets;		// entrada_TS particular

} tipo_TS;


/*
=====================================================================
						Definici�n de variables globales
=====================================================================
*/

//extern entrada_TS  *inf_id;
//extern tipo_TS  ts[TAM_TS];		// Tabla de S�mbolos


entrada_TS  *inf_id;		// Puntero a la estructura que contiene la
								// informaci�n de un identificador, la cual es
								// completada previamente a una inserci�n en TS

int       th[TAM_HASH];   // Tabla de Hash
tipo_TS   ts[TAM_TS];     // Tabla de S�mbolos
int       tb[TAM_BLOQ];   // Tabla de Bloques
int  topeTS;
int  topeTB;

/* 
=====================================================================
		Funciones para la administraci�n de la Tabla de S�mbolos 
=====================================================================
*/

/* PROTOTIPOS */

void inic_tablas();

int insertarTS();
int en_tabla(char *);
int en_nivel_actual(char *);
int Tipo_Ident(char *);
int Clase_Ident(char *);
 
void popTS();
int pushTS(int, entrada_TS *);

void popTB();
void pushTB();
void pop_nivel();
int get_nivel();


void mostrar_tabla();