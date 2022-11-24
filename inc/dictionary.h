#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

// dictionary.h tiene elementos que traducen los comandos para la interfaz con la algochain 
// ingresados por linea de comandos.  
#include <iostream>
#include <string.h>
#include <cstring>

#include "Array.h"
#include "tools.h"
#include "Lista.h"
#include "block.h"
#include "sha256.h"
#include "Lista.h"
#include "main.h"
#include "user.h"
//-----------------------------------------------------MACROS----------------------------------------------
//Para definir las referencias de comandos
#define STR_INIT "init"
#define STR_TRANSFER "transfer"
#define STR_MINE "mine"
#define STR_BALANCE "balance"
#define STR_BLOCK "block"
#define STR_TXN "txn"
#define STR_LOAD "load"
#define STR_SAVE "save"

//Para definir hashes en un estado inicial
#define NULL_HASH "0000000000000000000000000000000000000000000000000000000000000000"

//Para contar la cantidad de punteros a funciones se tiene. Se utiliza para encontrar el que se necesita en 
//cada situacion
#define MAXCMD 8

//Para ocurrencias de error
#define MSG_FAIL "FAIL"

//Para aumentar el tamaño de un arreglo dinamico
#define CHOP_SIZE 5

//----------------------------------------------VARIABLES GLOBALES-----------------------------------------
using namespace std;

//-----------------------------------------------PUNTEROS A FUNCION ---------------------------------------
//Los punteros a funcion ejecutan el comando ingresado y devuelven lo especificado por el comando (como un 
// hash que represente cierto objeto o un mensaje de error)

typedef string (* p_func)(Array <string>);


string cmdInit(Array <string> args);
string cmdTransfer( Array <string> args);
string cmdMine(Array <string> args);
string cmdBalance(Array <string> args);
string cmdBlock(Array <string> args);
string cmdTxn(Array <string> args);
string cmdLoad(Array <string> args);
string cmdSave(Array <string> args);


struct cmd_option_t
{
	string name;
	p_func comand;
	int num_param;
};

static cmd_option_t dictionary_cmd[] = {
	{STR_INIT, cmdInit, 3},
	{STR_TRANSFER, cmdTransfer, 0},
	{STR_MINE, cmdMine, 1},
	{STR_BALANCE, cmdBalance, 1},
	{STR_BLOCK, cmdBlock, 1},
	{STR_TXN, cmdTxn, 1},
	{STR_LOAD, cmdLoad, 1},
	{STR_SAVE, cmdSave, 1},
};

p_func dictCmds( string cmd, int &num_param)
{
	string aux;
	int i = 0;

	while(cmd != dictionary_cmd[i].name && i < MAXCMD) i++;
	if(i == MAXCMD)
	{
		cerr << "El comando no es valido" << endl;
		exit(1);
	}
	num_param = dictionary_cmd[i].num_param;
	return dictionary_cmd[i].comand;
}


string cmdInit(Array <string> args)
{
 
	string user_;
	string STR_TXNing;
	size_t bits;
	if(algochain.empty()==false)
	{
		list <block> list_empty;
		algochain = list_empty;
	}
	if(users.empty()==false)
	{
		list <user> list_empty;
		users = list_empty;
	}
	user_ = sha256(args[0]);

	if(isNumber<double>(args[1]) == false || args[1][0] == '-')
	{
		cerr << "El user no es valido" << endl;
		exit(1);
	}

	
	if(isNumber<int>(args[2]) == false || args[2][0] == '-')
	{
		cerr << "El bits no es valido" << endl;
		exit(1);
	}

	bits = stoi(args[2]);
	STR_TXNing.append("1"); // txn count
	STR_TXNing.append("\n");
	STR_TXNing.append("1"); // txn in
	STR_TXNing.append("\n");
	STR_TXNing.append(NULL_HASH);
	STR_TXNing.append(" ");
	STR_TXNing.append("0");
	STR_TXNing.append(" ");
	STR_TXNing.append(NULL_HASH);
	STR_TXNing.append("\n");
	STR_TXNing.append("1");
	STR_TXNing.append("\n");
	STR_TXNing.append(args[1]);
	STR_TXNing.append(" ");
	STR_TXNing.append(user_);
	
	istringstream iss(STR_TXNing);
	block genesis_block(NULL_HASH, bits, &iss);
	if (refreshUsersFromBlock(genesis_block) == false)
	{
		cerr << "ERROR: No se puede cargar user" << endl;
		exit(1);
	}
	algochain.append(genesis_block);
	return sha256(sha256(genesis_block.toString()));
}

string cmdTransfer( Array <string> args)
{
	//El comando se invoca asi:
	// transfer src dst1 value1 dst2 value 2 ... dstN valueN

	//Se debe buscar la ultima aparicion de ese usuario primero en la MEMPOOL y si no se encuentra nada, en la 
	//ALGOCHAIN; conseguir su value y verificar que dicho valor (su dinero disponible) no sea menor a la suma de
	//las cantidades a transferir.

	double src_balance,aux;
	string src=sha256(args[0]); //El primer elemento se condice con el usuario de origen.
	src_balance=stod(users.find("balance",src)); //Se busca el dinero disponible de el usuario src,  que aporta el dinero en la transaccion.
											//Precondicion: la lista global con los balances debe estar actualizada en todo momento.
	aux=src_balance;
	size_t dim_array_aux=(args.getSize()-1)/2;
	Array<string> dst(dim_array_aux); //Arreglo de usuarios destino.

	//Al saber la cantidad de argumentos que se reciben se puede calcular el tamaño de los arreglos auxiliares, pues
	// args.getSize()-1 es la cantidad de argumentos relacionados a los usuarios de destino. Como vienen de a pares
	//(una vez validados) el resultado de (args.getSize()-1)/2 sera siempre entero, el valor del tamaño del arreglo.

	Array<string> dst_value_str(dim_array_aux); //Arreglo de valores(en strings) a transferir a usuarios destino.
	Array <double> dst_value(dim_array_aux);   //Arreglo de valores(en doubles) a transferir a usuarios destino.
	
	size_t n=dim_array_aux;// args.getSize();

	for(size_t i=2,j=0; j < n ;i+=2,j++)
	{
 
		//Se consiguen los hash de los usuarios destino y los valores a transferir
		dst[j]=sha256(args[i-1]); //Puede no ser necesario conseguir los hashes, podria trabajarse directamente con los nombres de los usuarios.

		//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿??????????????????????????????????????????
		args[i-1]=dst[j]; //Necesario para evitar complicaciones a la hora de generar el arreglo de txn. 
		//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿??????????????????????????????????????????

		dst_value_str[j]=args[i]; // Se necesitan como strings? 
		dst_value[j]=stod(dst_value_str[j]);

		if(dst_value[j]<0) //No se puede transferir una cantidad negativa
			return MSG_FAIL;
		src_balance-=dst_value[j];
		if(src_balance<0) //Si en algun momento los fondos del usuario fuente se terminan, se devuelve error.
			return MSG_FAIL;
	}

	//Al salir del for ya se tienen cargadas las estructuras con las addresses y los valores a transferirles
	//por lo que se crea un arreglo con la informacion de la transaccion.
	txn aux_txn;

	//Construccion del arreglo de inputs
	// Se debe buscar en la lista de transacciones del user src todos los outputs necesarios hasta completar la 
	//cantidad que se desea transferir: buscar en los outputs hasta conseguir aux-src_balance.
	
	string str_user=users.find("user",src);
	user aux_user(str_user);

	Array<inpt> aux_arr_inputs; //Implementar este constructor en block.
	aux_arr_inputs = aux_user.trackMoney(aux-src_balance);
	aux_txn.setNTxIn(aux_arr_inputs.getSize());
	aux_txn.setTxIn(aux_arr_inputs);
	string aux_str=to_string(src_balance);	
	//Construccion del arreglo de outputs
	if(src_balance == 0)
	{
		aux_txn.setNTxOut(dim_array_aux);
	}
	else
	{
		aux_txn.setNTxOut(dim_array_aux+1);
		dst.ArrayRedim(dim_array_aux+1);
		dst[dst.getSize()-1]=src;	
		dst_value_str.ArrayRedim(dim_array_aux+1);
		dst_value_str[dst_value_str.getSize()-1]=aux_str;
	}
	//Se agrega a los arreglos auxiliares el output necesario para el UTXO de src.

	
	aux_txn.setTxOut(dst,dst_value_str); //Implementar esta funcion en block.h

	mempool.addTxn(aux_txn); //Implementar esta funcion en block.h

	//Se carga la transaccion a la lista de usuarios.
	for(size_t i=0; i< dim_array_aux;i++)
	{
		str_user=users.find("user",dst[i]);

		if(str_user==FINDNT)
		{
			user new_user;
			new_user.setName(dst[i]);
			new_user.addTxn(aux_txn);//falta ponerle el nombre
			users.append(new_user);
		}
		else
		{	
			user aux_user(str_user);
			users.removeElement(aux_user);
			aux_user.addTxn(aux_txn);
			users.append(aux_user);
		}
	}
	str_user=users.find("user",src);
	if(str_user==FINDNT)
	{
		user new_user;
		new_user.addTxn(aux_txn);
		users.append(new_user);
	}
	else
	{	
		user aux_user(str_user);
		users.removeElement(aux_user);
		aux_user.addTxn(aux_txn);
		users.append(aux_user);
	}
	return  sha256(sha256(aux_txn.toString()));
}

string cmdMine(Array <string> args)
		//Ensambla y agrega a la Algochain un nuevo bloque a partir de todas las transacciones en la mempool. 
		//La dificultad del minado viene dada por args
{
	/*Valor de retorno.
	 	Hash del bloque en caso de exito;  FAIL en caso de falla por invalidez.
	*/
	size_t bits = stoi(args[0]);
	block aux_save;
	if(bits<0)
	{
		cerr << "ERROR: dificultad invalida"<< endl; // que otra falla?
		exit(1);
	}
	block aux = algochain.getLastNode();
	string prev_block = sha256(sha256(aux.toString()));
	if(isHash(prev_block)==false)
	{
		cerr << "ERROR: al convertir prev block"<< endl; // que otra falla?
		exit(1);
	}
	mempool.setHeader(prev_block,bits);
	aux_save = mempool; //lo guargo para despues imprimirlo
	//guardar la mempool en la parte de la lista correspondientes
	algochain.append(mempool);
	//limpiar mempool
	block empty_block;
	
	mempool = empty_block;
	return sha256(sha256(aux_save.toString()));
	//return "hola";
}



string cmdBalance(Array <string> args)
{
	string name = sha256(args[0]), balance_str;
	if((balance_str = users.find(STR_BALANCE, name)) == FINDNT)
		return "0";
	return balance_str;
}

string cmdBlock(Array <string> args)
{
	string id = args[0], block_str;
	if((block_str = algochain.find(STR_BLOCK, id)) == FINDNT)
		return "FAIL";
	return block_str;
}

string cmdTxn(Array <string> args)
{

	string id = args[0], txn_str;
	if((txn_str = algochain.find(STR_TXN_IN_BLOCK_BY_HASH, id)) == FINDNT)
	{
		if((txn_str = findTxnInBlockByHash(id, mempool.toString())) == FINDNT)
			return "FAIL";
		else
			return txn_str;
	}
	return txn_str;
}

string cmdLoad(Array <string> args)
{
	fstream ifs_load;
	istream *iss_load = 0;
	ifs_load.open(args[0].c_str(), ios::in);
	iss_load = &ifs_load;
	if (!iss_load->good()) {
	cerr << "cannot open "
			<< args[0]
			<< "."
			<< endl;
	exit(1);
	}
	if(algochain.empty()==false)//la vacio si es necesario
	{
		list <block> empty_list;
		algochain = empty_list;//
	}
	if(setAlgochainFromFile(iss_load)==false)
	{
		cerr << "ERROR: no se pudo cargar el archivo " << endl;
		exit(1);
	}
	ifs_load.close();
	block aux ;
	aux = algochain.getLastNode();
	return sha256(sha256(aux.toString()));
}

string cmdSave(Array <string> args)
 { 
	fstream ofs_save;
	ostream *oss_save = 0;
	ofs_save.open(args[0].c_str(), ios::out);
	oss_save = &ofs_save;
	if (!oss_save->good()) 
	{
		cerr << "cannot open "
		     << args[0]
		     << "."
		     << endl;
		exit(1);		// EXIT: Terminación del programa en su totalidad
	}
	else if (oss_save->bad()) 
	{
		cerr << "cannot write to output stream."
		     << endl;
		exit(1);
	}
	*oss_save << algochain;//le agrego tostring?
	ofs_save.close();
	return "Carga realizada con exito";
}


Array <string> parseCmdArgs(string str, size_t N)//funcion para verificar la correcta escritura de los argumentos, 
									  // devuelve un array dinamico con los argumentos.
									 //  Le paso N para saber cuantos argumentos debo mirar para que sea correcto
									//   Si es N=0 entonces es argumentos variables impares minimo 3
{
	istringstream iss(str);
	size_t i=0;
	string aux;

	Array <string> args(1); // Lo dimensiono con 3 porque es el valor minimo, despues lo resize
	while(getline(iss, aux, ' ').good() || iss.eof() )//averiguar que pasa con el ultimo
	{
		if(aux == "")
		{
			cerr << "Hay un espacio de mas" << endl; // poner error
			exit(1);
		}
		if (i>=args.getSize())
		{
			args.ArrayRedim(args.getSize()+1);
		}
		if(iss.eof()==true)
		{
			args[i]=aux;
			break;
		}
		args[i]=aux;
		i++;
	}
	if(N!=(i+1) && N!=0)
	{
		cerr << "Numero incorrecto de argumentos"<< endl;
		exit(1);
	}
	return args;
}

#endif //_DICTIONARY_H_