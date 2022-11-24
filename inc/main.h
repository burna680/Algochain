#ifndef _MAIN_H_
#define _MAIN_H_


#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "cmdline.h"

#include "Lista.h"
#include "block.h"
#include "Array.h"
#include "user.h"

using namespace std;


/**************** Elementos globales ******************/
static void opt_input(string const &);
static void opt_output(string const &);
static void opt_help(string const &);

bool setAlgochainFromFile( istream *iss);
bool refreshUsersFromBlock(block );
block mempool;
list <block> algochain;
list <user> users;

// Tabla de opciones de línea de comando. El formato de la tabla
// consta de un elemento por cada opción a definir. A su vez, en
// cada entrada de la tabla tendremos:
//
// o La primera columna indica si la opción lleva (1) o no (0) un
//   argumento adicional.
//
// o La segunda columna representa el nombre corto de la opción.
//
// o Similarmente, la tercera columna determina el nombre largo.
//
// o La cuarta columna contiene el valor por defecto a asignarle
//   a esta opción en caso que no está explícitamente presente
//   en la línea de comandos del programa. Si la opción no tiene
//   argumento (primera columna nula), todo esto no tiene efecto.
//
// o La quinta columna apunta al método de parseo de la opción,
//   cuyo prototipo debe ser siempre void (*m)(string const &arg);
//
// o La última columna sirve para especificar el comportamiento a
//   adoptar en el momento de procesar esta opción: cuando la
//   opción es obligatoria, deberá activarse OPT_MANDATORY.
//
// Además, la última entrada de la tabla debe contener todos sus
// elementos nulos, para indicar el final de la misma.
//

static option_t options[] = {
	{1, "i", "input", "-", opt_input, OPT_DEFAULT},
	{1, "o", "output", "-", opt_output, OPT_DEFAULT},
	{0, "h", "help", NULL, opt_help, OPT_DEFAULT},
	{0, },
};


static istream *iss = 0;	// Input Stream (clase para manejo de los flujos de entrada)
static ostream *oss = 0;	// Output Stream (clase para manejo de los flujos de salida)
static fstream ifs; 		// Input File Stream (derivada de la clase ifstream que deriva de istream para el manejo de archivos)
static fstream ofs;		// Output File Stream (derivada de la clase ofstream que deriva de ostream para el manejo de archivos)

static void
opt_input(string const &arg)
{
	// Si el nombre del archivos es "-", usaremos la entrada
	// estándar. De lo contrario, abrimos un archivo en modo
	// de lectura.
	//
	if (arg == "-") {
		iss = &cin;		// Establezco la entrada estandar cin como flujo de entrada
	}
	else {
		ifs.open(arg.c_str(), ios::in); // c_str(): Returns a pointer to an array that contains a null-terminated
										// sequence of characters (i.e., a C-string) representing
										// the current value of the string object.
		iss = &ifs;
	}

	// Verificamos que el stream este OK.
	//
	if (!iss->good()) {
		cerr << "cannot open "
		     << arg
		     << "."
		     << endl;
		exit(1);
	}
}

static void
opt_output(string const &arg)
{
	// Si el nombre del archivos es "-", usaremos la salida
	// estándar. De lo contrario, abrimos un archivo en modo
	// de escritura.
	//
	if (arg == "-") {
		oss = &cout;	// Establezco la salida estandar cout como flujo de salida
	} else {
		ofs.open(arg.c_str(), ios::out);
		oss = &ofs;
	}

	// Verificamos que el stream este OK.
	//
	if (!oss->good()) {
		cerr << "cannot open "
		     << arg
		     << "."
		     << endl;
		exit(1);		// EXIT: Terminación del programa en su totalidad
	}
}

static void
opt_help(string const &arg)
{
	cout << "Comands: \n"
		 << "init <user> <value> <bits> \n"
		 << "transfer <src> <dst1> <value1> ... <dstN> <valueN> \n"
		 << "mine <bits> \n"
		 << "balance <user> \n"
		 << "block <id> \n"
		 << "txn <id> \n"
		 << "load <filename> \n"
		 << "save <filename> \n"
		 << "exit program \n"
	     << endl;
	exit(0);
}

bool setAlgochainFromFile( istream *iss_load)
{
	block block_aux, block_empty;
	string str,str_aux;
	size_t i = -1;//, aux = 0;
	hdr header_aux;
	size_t diff, nonce;
	bdy body_aux;
	getline(*iss_load, str, '\n');
	if(str!=NULL_HASH)
	{
		cerr << "ERROR: No comienza con el genesis block" << endl;
		exit (1); 
	}
	while (str!="")
	{
		//seteo el header
		i++;
		
		if(isHash(str)==false)
		{
			cerr << "ERROR: no es un hash para prev block" << endl;
			exit(1);
		}
		header_aux.setPrevBlock(str);
		getline(*iss_load, str, '\n');
		if(isHash(str)==false)
		{
			cerr << "ERROR: no es un hash para txns hash" << endl;
			return false;
		}
		header_aux.setTxnsHash(str);
		getline(*iss_load, str, '\n');
		if(isNumber<size_t>(str)==0)
		{
			cerr<<"ERROR: no es un numero"<< endl;
			return false;
		}
		diff = stoi(str);
		header_aux.setBits(diff);
		getline(*iss_load, str, '\n');
		if(isNumber<size_t>(str)==0)
		{
			cerr<<"ERROR: no es un numero"<< endl;
			return false;
		}
		nonce = stoi(str);
		header_aux.setNonce(nonce);
		
		block_aux.setHeader(header_aux); //guarda el header
		//seteo el body
		str_aux=block_aux.setBody(iss_load);
		// chequeo que sea genesis
		if(i==0)
		{
			body_aux = block_aux.getBody();
			if (body_aux.getTxnCount()!=1)
			{
				cerr << "ERROR: No comienza con el genesis block" << endl;
				exit (1); 
			}
			Array <txn> txns_aux = body_aux.getTxns();
			if(txns_aux[0].getNTxIn()!=1  || txns_aux[0].getNTxOut()!=1 )
			{
				cerr << "ERROR: No comienza con el genesis block" << endl;
				exit (1); 
			}
			Array <inpt> tx_in_aux = txns_aux[0].getInputs();
			outpnt outpoint = tx_in_aux[0].getOutPoint();
			if(outpoint.tx_id!=NULL_HASH && outpoint.idx!=0)
			{
				cerr << "ERROR: No comienza con el genesis block" << endl;
				exit (1); 
			}
		}
		

		if(isHash(str_aux)==true || str_aux=="")//volver a los if separadaros y ver si es break o continue se rompio mi crerbo
		{
			str=str_aux;
			algochain.append(block_aux);
			if(refreshUsersFromBlock(block_aux)==false)
			{
				cerr<< "ERROR: no se pueden cargar los users"<< endl;
				exit(1);
			}
			break;
		}
		else if (str_aux=="OK")
		{
			algochain.append(block_aux);
			if(refreshUsersFromBlock(block_aux)==false)
			{
				cerr<< "ERROR: no se pueden cargar los users"<< endl;
				exit(1);
			}
			getline(*iss_load, str, '\n');			
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool refreshUsersFromBlock(block blck)
{
	bdy body = blck.getBody();
	size_t txn_count = body.getTxnCount(), n_tx_in, n_tx_out;
	Array <txn> txns = body.getTxns();
	Array <inpt> inpts;
	Array <outpt> outpts;
	string addr;
	list <string> address, empty_list;
	for(size_t i =0 ; i < txn_count ; i++)
	{
		n_tx_in = txns[i].getNTxIn();
		inpts = txns[i].getInputs();
		
		for (size_t j = 0; j < n_tx_in; j++)
		{
			if(j==0)
			{
				addr = inpts[j].getAddr();
				continue;
			}
			else
			{
				if(inpts[j].getAddr()!=addr)
				{
					cerr << "ERROR: Addr en inputs es distinto" << endl;
					return false;
				}
			}
		}

		if(users.find("checkUser",addr)!=FINDNT)
		{
			string str_user=users.find("user",addr);
			user aux_user(str_user);
			users.removeElement(aux_user);
			aux_user.loadTxn( txns[i]);
			users.append(aux_user);
		}
		else
		{
			if (addr != NULL_HASH )
			{
				user aux_user;
				aux_user.setName(addr);
				aux_user.loadTxn(txns[i]);
				users.append(aux_user);
			}
		}
		//con los outputs
		n_tx_out = txns[i].getNTxOut();
		outpts = txns[i].getOutputs();

		for (size_t j = 0; j < n_tx_out; j++)
		{
			addr = outpts[j].getAddr();
			if (addr ==inpts[0].getAddr() )
			{
			}
			else if(address.contains(addr)==true)
			{
				cerr << "ERROR: Addr en outputs repetidas" << endl;
				return false;
			}
			else if (address.contains(addr)!=true || users.find("checkUser",addr)==FINDNT)
			{
				addr = outpts[j].getAddr();
				address.append(addr);
				user aux_user;
				aux_user.setName(addr);
				aux_user.loadTxn(txns[i]);
				users.append(aux_user);
			}
			else
			{
				string str_user=users.find("user",addr);
				user aux_user(str_user);
				users.removeElement(aux_user);
				aux_user.loadTxn(txns[i]);
				users.append(aux_user);
			}	
		}
		address = empty_list;
	}
	return true;
}
#endif //MAIN_H