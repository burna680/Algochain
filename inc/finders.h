#ifndef _FINDERS_H_
#define _FINDERS_H_


#define FINDNT "Findnt" 
//Para definir las referencias de busqueda
#define STR_BALANCE "balance" 
#define STR_TRANSACTIONS "transactions"
#define STR_BLOCK "block"
#define STR_CHECK_USER "checkUser"
#define STR_USER "user"
#define STR_TXN_BY_HASH "txnByHash"
#define STR_TXN_IN_BLOCK_BY_HASH "findTxnInBlockByHash"

#include <iostream> 
#include "user.h"	//Necesario para buscar dentro de la lista de usuarios.
#include "block.h" //Necesario para crear la lista de transacciones de cada usuario.

using namespace std;
//-----------------------------------------------------MACROS----------------------------------------------
//Para contar la cantidad de finders que se tiene. Se utiliza para encontrar el que se necesita en 
//cada situacion
#define MAXFINDER 7




//-----------------------------------------------------FINDERS---------------------------------------------
//Los finders buscan la informacion especifica pedida (como un id o un value de cierto
// user) y la devuelven en una string
typedef string (*finder)(string d,string str); //Buscan en un la string str dato d


//Recordar modificar la macro MAXFINDER al agregar nuevas funciones aqui

string findBalance(string d, string str);
string findTransactions(string d, string str);
string checkUser(string d, string str);
string findUser(string d, string str);
string findTxnByHash(string d, string str);

string findTransactions(string d, string str) 
{
	//str debe ser el contenedor de la transaccion. En este caso el user.
	
	//Esta funcion devuelve todas las transacciones del usuario como una string.
    user aux_user(str);
	string result, aux;
	aux = aux_user.getName();
	if(d == aux)
		return aux_user.getTransactions().toString();
		// return (aux_user.getTransactions()).toString();     //AGREGAR ESTA LINEA CUANDO SE HAYA TERMINADO LA FUNCION
	else
		return "Findnt";
}
string findBalance(string d, string str)
{
	//Se recorren todos los outputs de todas las transacciones realizadas buscando
	//la utlima aparicion del usuario especificado para devolver el valor que quedo en output.

	//Seria bueno agregar unos metodos mas en la clase outpt que sean getValueAsString()
	//y getAddr()

	user aux_user(str);
	string result, aux;
	aux = aux_user.getName();
	if(d == aux)
		return to_string(aux_user.getBalance());
	else
		return "Findnt";
}

string findBlock(string d, string str)
{
	string aux;
	aux=sha256(sha256(str));
	if(aux==d)
		return str;
	else
		return "Findnt";
}

string checkUser(string d, string str)
{
	user aux_user(str);
	if(d == aux_user.getName())
		return "TRUE";
	else
		return "Findnt";
}

string findUser(string d, string str)
{
	user aux_user(str);
	if(d == aux_user.getName())
		return aux_user.toString();
	else
		return "Findnt";
}

string findTxnByHash(string d, string str)
{
	txn tran(str);
	if(d == sha256(sha256(tran.toString())))
		return tran.toString();
	else
		return "Findnt";
}

string findTxnInBlockByHash(string d, string str)
{
// cout << "find tx in bloc COMIENZA " << str << endl;
// cout << "find txn in block by hash d:\n" << d << " >>" << endl;
	block blck(str);
// cout << " blco " <<  blck << endl;
	bdy body;
	Array<txn> txns;
	body = blck.getBody();
	txns = body.getTxns();
// cout << "get txn count " << body.getTxnCount() << endl;
	for (size_t i = 0; i < body.getTxnCount(); i++)
	{

// cout << "find txn in block by hash doble hash:\n" <<  sha256(sha256(txns[i].toString())) << " >>" << endl;
		if(d == sha256(sha256(txns[i].toString())))
		{
			return txns[i].toString();
		}
			
	}
	return FINDNT;
}
// string findTransaction(string d, string str) 
// {
// 	//Esta funcion devuelve la transaccion buscada como una string.
//     user aux_user(str);
// 	string result, aux;
// 	aux = aux_user.getName();
// 	if(d == aux)
// 		return aux_user.getTransactions().toString();
// 		// return (aux_user.getTransactions()).toString();     //AGREGAR ESTA LINEA CUANDO SE HAYA TERMINADO LA FUNCION
// 	else
// 		return "Findnt";
// }

//---------------------------------------------DICCIONARIOS-----------------------------------------------
//Definicion de las estructuras que relacionan las referencias (el tipo de dato que se
// quiere buscar) con los finders.


//Completar con las definiciones de todos los finders necesarios
struct finder_option_t
{
	string reference; //Identifica el tipo de dato que debe ser buscado (como value o tx_id)
	finder fndr;
};

static finder_option_t dictionary_finder[] = {
	{STR_BALANCE, findBalance},
    {STR_TRANSACTIONS, findTransactions},
	{STR_BLOCK, findBlock},
	{STR_CHECK_USER, checkUser},
	{STR_USER, findUser},
	{STR_TXN_BY_HASH, findTxnByHash},
	{STR_TXN_IN_BLOCK_BY_HASH,findTxnInBlockByHash}
};

finder finderParse( string ref)
{
	string aux;
	int i = 0;

	while(ref != dictionary_finder[i].reference && i < MAXFINDER) i++;
	
	if(i == MAXFINDER)
	{
		cerr << "El finder no es valido" << endl;
		exit(1);
	}
	return dictionary_finder[i].fndr;
}
#endif //_FINDERS_H_



		
	// string d_hash=sha256(d);
	// outpt aux;
	// for(size_t i=this->getBody().getTxnCount()-1;i>=0;;i--)
	// {
	//     for(size_t j=this->getBody().getTxns()[i].getNTxOut();j>=0;j--)
	//     {
	//         aux=this->getBody().getTxns()[i].getTxOut()[j];
	//         if(aux.addr==d_hash)
	//             return aux.getValueAsString();
	//     }
	// }