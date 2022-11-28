#ifndef _BLOCK_H_
#define _BLOCK_H_
#include <iostream>
#include <string.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <bitset>
#include <ctype.h>
#include <iomanip>

#include "blockPrototype.h"

#include "math.h"
#include "tools.h"
#include "Array.h"
#include "sha256.h"
#define NULL_HASH "0000000000000000000000000000000000000000000000000000000000000000"

// o Se usaran strings para representar los hash
//
// o Jerarquía de clases: Bloque > Header/Body>Txn > Input/Output > Outpoint (estructura que pertenece a Input).


inpt::inpt(){}//Creador base

inpt::~inpt(){} //Destructor base

inpt::inpt(string & str) //Creador mediante una string
{
	istringstream ss(str);
	string str_tx_id,str_id,str_addr;
	
	getline(ss, str_tx_id, ' '); // Se recorren los campos. Si el formato es erroneo, se detecta como una cadena vacia.
	getline(ss, str_id, ' ');
	getline(ss, str_addr, '\n');

	if((isHash(str_tx_id)==true) && (isNumber<size_t>(str_id)==1) && (isHash(str_addr)==true))
	{
		this->outpoint.tx_id=str_tx_id;
		this->outpoint.idx=stoi(str_id);
		this->addr=str_addr;
	}
	else
	{
		this->addr=ERROR; // Si hay un error pone addr en ERROR, para avisar a un nivel mas alto
	}
}

inpt & inpt::operator=(const inpt & right)
{
	if (&right != this)
	{
		outpoint = right.outpoint;
		addr = right.addr;
		return *this;
	}
	return *this;
}

outpnt inpt::getOutPoint(){return outpoint;}

string inpt::getAddr(){return addr;}

void inpt::setInput(string aux_tx_id, size_t aux_idx, string aux_addr)
{
	outpoint.tx_id=aux_tx_id;
	outpoint.idx=aux_idx;
	addr=aux_addr;
}

void inpt::show(ostream& oss)
{
	if(outpoint.tx_id == "" || addr == "")
	{
		return ;
	}
	oss << outpoint.tx_id << " " << outpoint.idx << " " << addr;
}

bool inpt::operator==(const inpt & right) const
{
	if(outpoint.idx == right.outpoint.idx && outpoint.tx_id == right.outpoint.tx_id && addr == right.addr)
		return true;
	else
		return false;
}

bool inpt::operator!=(const inpt & right){return !(*this == right);}

string inpt::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}
//--------------------------CLASE OUTPUT----------------------------------------------------------------------------------------

outpt::outpt() //Creador base
{    
}

outpt::~outpt() //Destructor base
{    
}

outpt & outpt::operator=(const outpt & right)
{
	if(&right != this)
	{
		value = right.value;
		addr = right.addr;
		return *this;
	}
	return *this;
}

outpt::outpt(string & str) //Creador mediante una string
{
	istringstream ss(str);
	string str_value,str_addr;

	getline(ss, str_value, ' '); // Se recorren los campos. Si el formato es erroneo, se detecta como una cadena vacia.
	getline(ss, str_addr, '\n');

	if((isNumber<double>(str_value)==1) && (isHash(str_addr)==true))
	{
		this->value=str_value;
		this->addr=str_addr;
	}
	else
	{
		this->addr=ERROR;
	}
}
outpt::outpt(string& str_addr, string & str_value)
{
	if((isNumber<double>(str_value)==1) && (isHash(str_addr)==true))
	{
		this->value=str_value;
		this->addr=str_addr;
	}
	else
	{
		this->addr=ERROR;
	}
}

string outpt::getAddr(){return addr;}

string outpt::getValue(){return value;}


void outpt::show(ostream& oss)
{
	if(addr == "")
	{
		return ;
	}
	oss << value << " " << addr;
}


bool outpt::operator==(const outpt & right) const
{
	if(value == right.value && addr == right.addr)
		return true;
	else
		return false;
}

string outpt::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}
//--------------------------CLASE TXN----------------------------------------------------------------------------------------


txn::txn()
{
	n_tx_in=0;
	n_tx_out=0;
	tx_in.ArrayRedim(0);
	tx_out.ArrayRedim(0);
}

txn::txn(Array<string>& txn_str_arr)
{
	size_t i;
	this->setNTxIn(stoi(txn_str_arr[0]));
	for(i=1;i<(this->getNTxIn())+1;i++)
	{
		inpt in(txn_str_arr[i]);
		tx_in[i-1] = in;
	}
	this->setNTxOut(stoi(txn_str_arr[i]));
	for( size_t j=0;j<(this->getNTxOut());j++,i++)
	{
		outpt out(txn_str_arr[i]);
		tx_out[j] = out;
	}
}
txn::txn(string txn_str)
{
	istringstream ss(txn_str);
	string aux;
	getline(ss, aux, '\n');
	size_t i;
	n_tx_in = stoi(aux);
	tx_in.ArrayRedim(n_tx_in);
	for(i=0;i<n_tx_in;i++)
	{
		getline(ss, aux, '\n');
		inpt in(aux);
		tx_in[i] = in;
	}
	getline(ss, aux, '\n');
	n_tx_out = stoi(aux);
	tx_out.ArrayRedim(n_tx_out);
	for(i=0 ;i<n_tx_out;i++)
	{
		getline(ss, aux, '\n');
		outpt out(aux);
		tx_out[i] = out;
	}
}

txn::~txn()
{
}

txn & txn::operator=(const txn &right)
{
	if(&right != this)
	{
		n_tx_in = right.n_tx_in;
		n_tx_out = right.n_tx_out;
		tx_in = right.tx_in;
		tx_out = right.tx_out;
		return *this;
	}
	return *this;
}
void txn::setNTxIn(const  size_t n) 
{
	n_tx_in=n;
	if(tx_in.getSize() == 0)
	{
		tx_in.ArrayRedim(n);
	}
}


void txn::setNTxOut(const size_t n)
{
	n_tx_out=n;
	if(tx_out.getSize() == 0)
	{
		tx_out.ArrayRedim(n);
	}
}



string txn::setTxIn(const size_t n, istream *iss)  //Se modifica el retorno del setter por defecto (void) por
{												// necesidad. Verifica si el setteo pudo realizarse correctamente.
	string aux_s;
	tx_in.ArrayRedim(n);
	for (size_t i = 0; i < n; i++)
	{
		getline(*iss, aux_s, '\n');
// cout << "settxin 1 \n" << aux_s << ">>" << endl;
		if(isHash(aux_s)==true)
		{
			return aux_s;
		}
		inpt in(aux_s);
		if(isError(in.getAddr())==false)
			return "ERROR: addres invalida";
		tx_in[i] = in;
	}
	return "OK";
}

bool txn::setTxIn(const size_t n, Array<string>& tx_in_str_arr)
{
	//Se modifica el retorno del setter por defecto (void) por
	// necesidad. Verifica si el setteo pudo realizarse correctamente. 
	string aux_s;
	tx_in.ArrayRedim(n);
	for (size_t i = 0; i < n; i++)
	{
		inpt in(tx_in_str_arr[i]);
		if(isError(in.getAddr())==false)
			return false;
		tx_in[i] = in;
	}
	return true;
}

bool txn::setTxIn(Array<inpt>& arr)
{
	size_t n = arr.getSize();
	tx_in.ArrayRedim(n);
	for (size_t i = 0; i < n; i++)
	{
		if(isError(arr[i].getAddr())==false) //QUE QUISISTE PONER ACA
			return false;
		tx_in[i]=arr[i];
	}
	return true;
}


string txn::setTxOut(const size_t n, istream *iss) //Se modifica el retorno del setter por defecto (void) por
												// necesidad. Verifica si el setteo pudo realizarse correctamente.
{
	string aux_s;
	tx_out.ArrayRedim(n);
	for (size_t i = 0; i < n; i++)
	{
		getline(*iss, aux_s, '\n');
		if(isHash(aux_s)==true)
		{
			return aux_s;
		}
		outpt out(aux_s);
		if(isError(out.getAddr())==false)
			return "ERROR: addres invalida";
		tx_out[i] = out;
	}
	return "OK";
}

string txn::setTxOut(Array<string> dst,Array<string> dst_value)
{
	size_t n=dst.getSize();
	tx_out.ArrayRedim(n);
	for(size_t i=0; i< n;i ++)
	{
		outpt aux_output(dst[i],dst_value[i]);
		if(isError(aux_output.getAddr())==false)
			return "ERROR: addres invalida";
		tx_out[i]=aux_output;
	}
	return "OK";
	
} 

bool txn::setTxOut(const size_t n, Array<string>& tx_in_str_arr) //Se modifica el retorno del setter por defecto (void) por
												// necesidad. Verifica si el setteo pudo realizarse correctamente.
{
	tx_out.ArrayRedim(n);
	for (size_t i = 0; i < n; i++)
	{
		outpt out(tx_in_str_arr[i]);
		if(isError(out.getAddr())==false)
			return false;
		tx_out[i] = out;
	}
	return true;
}

size_t txn::getNTxIn(){return n_tx_in;}

size_t txn::getNTxOut(){return n_tx_out;}

Array<inpt>& txn::getInputs(){return tx_in;}
Array<outpt>& txn::getOutputs(){return tx_out;}


void txn::show(ostream& oss)
{
	size_t i;
	if(n_tx_in == 0)
		return ;
	oss << n_tx_in << endl;
	for (i = 0; i < n_tx_in; i++)
	{
		oss << tx_in[i] << endl;
	}
	
	oss << n_tx_out << endl;
	for (i = 0; i < n_tx_out-1; i++)
	{
		oss << tx_out[i] << endl;
	}
	oss << tx_out[i];
}

bool txn::operator==(const txn & right) const
{
	if(n_tx_in == right.n_tx_in && tx_in == right.tx_in && n_tx_out == right.n_tx_out && tx_out == right.tx_out )
		return true;
	else
		return false;
}
string txn::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}
//--------------------------CLASE BODY----------------------------------------------------------------------------------------

bdy::bdy()
{
}

bdy::~bdy(){}

bdy & bdy::operator=(const bdy & right)
{
	if(&right != this)
	{
		txn_count = right.txn_count;
		txns = right.txns;
		return *this;
	}
	return *this;
}

void bdy::setTxnCount(const size_t n)
{

	txn_count = n;
	if(this->txns.getSize() == 0)
	{
		txns.ArrayRedim(n);
	}
}

// void bdy::setTxns(Array <txn> n)
// {
// 	txns = n;
// }

string bdy::setTxns(istream *iss)
{
	string str, error_string;
	size_t aux, i = 0;
	bool err=false;
	while(getline(*iss, str, '\n'))
	{
// cout << "settxns 1 \n" << str << ">>" << endl;
		if(isHash(str)==true || str == "")
		{
			txn_count = i;
			return str;
		}
		if(i >=txns.getSize())
		{
			txns.ArrayRedim(txns.getSize()*2); // Dependiendo de cuantos datos haya que analizar se puede modificar
		}									  // la estrategia de crecimiento del arreglo.

		// Se verifica n_tx_in
		if(isNumber<size_t>(str)==0 || (str[0]) == '\0')
		{
	// cout << "error ntxin no es un numero " << " << " << str << " >> " << endl;
			err=true; 
			break;
		}

		aux = stoi(str);
		txns[i].setNTxIn(aux);
		// Se verifican las entradas

		if(txns[i].setTxIn(aux, iss)!="OK")
		{
// cout << "error txin no ok " << endl;	
			
			err=true;
			break;
		}
		// Se verifica n_tx_out

		getline(*iss, str, '\n');

		if(isNumber<size_t>(str)==0 || (str[0]) == '\0')
		{
	// cout << "error ntxout no es un numero " << endl;

			err=true;
			break;
		}
		
		aux = stoi(str);
		txns[i].setNTxOut(aux);

		// Se verifican las salidas

		str=txns[i].setTxOut(aux, iss);
		i++;

		if(isHash(str)==true)
		{
			txn_count = i;
			return str;
		}
		else if(str=="OK")
		{
			if(i < txn_count)
				continue;
			txn_count = i;
			return str;
		}
		else 
		{
	// cout << "error txour " << endl;

			err=true;
			break;
		}
		
	}
	txn_count = i;
	if(err==true)
	{
		error_string.append("Error en la transaccion ");
		error_string.append(to_string(i+1));
		error_string.append("\n");
		error_string.append("Carga de datos interrumpida");
		error_string.append("\n");
		error_string.append("Vuelva a cargar los datos del bloque");
		return error_string;
	}
	
	if(str == "")
	{
		return str;
	}
	return "OK";
}

size_t bdy::getTxnCount(){return txn_count;}
Array<txn> &bdy::getTxns(){return txns;}

bdy bdy::getBody(){return *this;}

void bdy::txnsArrRedim(const size_t n ){txns.ArrayRedim(n);}


void bdy::show(ostream& oss)
{
	size_t i;
	if(txn_count == 1)
		oss << txn_count << endl;
	else
		oss << txn_count << endl;
			
	for (i = 0; i < txn_count - 1; i++)
	{
		oss << txns[i];
		oss << endl;
	}
	if(txn_count !=0)
	{
		oss << txns[i];	
	}
}

string bdy::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}
//--------------------------CLASE HEADER----------------------------------------------------------------------------------------


hdr::hdr()
{
	prev_block = "\0";
	txns_hash = "\0";
	bits = 0; 	//Se podría hacer que los bits y el nonce fueran ints para detectar errores haciendo que estos valgan -1 (por ej)
	nonce = 0;
}

hdr::~hdr(){}

hdr & hdr::operator=(const hdr & right)
{
	if(&right != this)
	{
		prev_block = right.prev_block;
		txns_hash = right.txns_hash;
		bits = right.bits;
		nonce = right.nonce;
		return *this;
	}
	return *this;
}

bool hdr::setPrevBlock(const string & str)//Se modifica el retorno del setter por defecto (void) por
										// necesidad. Verifica si el setteo pudo realizarse correctamente.
{
	if(isHash(str) == false)
		return false;
	else
	{
		prev_block = str;
		return true;
	}
}


void hdr::setTxnsHash(const string & str)
{
	if(isHash(str)==true)
	{
		txns_hash = str;
	}
	else
		txns_hash = sha256(sha256(str));
}


void hdr::setBits(const size_t n){bits = n;}

void hdr::setNonce(const size_t n){nonce = n;}


string hdr::getPrevBlock(){return prev_block;}


string hdr::getTxnHash(){return txns_hash;}


size_t hdr::getBits(){return bits;}


size_t hdr::getNonce(){return nonce;}


void hdr::setNonce(const string prev_block,const  string txns ,const  size_t bits) // Setea el header con el nonce que verifica que el hash del header cumpla con los primeros d bits en cero
{
	size_t out = 0; //inicializo d_auz que contara el nivel de difucultar y out que es un flag para el for
	size_t nonce_aux = 0; //inicializo el nonce, para mi hay que hacerlo double porque se puede hacer muy grande pero hay que cambiar struct

	string header_str; //defino un header_aux auxiliar para hacer la string antes de hashearla
	string hash_header; // para guardar el hash del header_aux
	
	int j, i, aux;

	int cant_char = bits/4;// me da la cantidad de char en 0 que necesito
	int cant_bit = bits%4; //me da la cantidad de bits del ultimo char en 0


	for (nonce_aux = 0; out ==0 ; nonce_aux++)//aumento el nonce hasta que el flag out sea 1, igualmente tambien hay un break, es por las dudas que el break no funcione como espero
	{
		header_str.clear();
		nonce = nonce_aux;
		header_str = toString();
		header_str.append("\n");
		hash_header = sha256(sha256(header_str)); //calculo el hash del header_aux
		i=0;
		aux=0;
		while (i<cant_char)
		{
			if(hash_header[i] != '0')
			{
				aux=1;
				break;
			}
			i++;
		}
		if(aux==1)
		{
			continue;
		}
		else
		{
			hash_header = Hex2Bin(hash_header.substr(i,i+1));
			
			j=0;
			aux=0;
			while (j<cant_bit)
			{
				if(hash_header[j] != '0')
				{
					aux=1;
					break;
				}
				j++;
			}
			if(aux == 0)
			{
				nonce = nonce_aux;//guardo el nonce en el header_aux
		   		out = 1; // Para verificar que termine el for
				break; //como cumple la canditad de bits necesarias y ya esta guardado y hasheado salgo
			}
		}
	}
	return ;
}

void hdr::show(ostream& oss)
{
	oss << prev_block << endl;
	oss << txns_hash << endl;
	oss << bits << endl;
	oss << nonce << endl;
}

string hdr::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}
//--------------------------CLASE BLOCK----------------------------------------------------------------------------------------

hdr block::getHeader()
{
	return header;
}
bdy block::getBody()
{
	return body;
}


void block::setHeader(hdr h){header = h;}

void block::setHeader(const string& prev_block_str,const size_t diffic)
{
	string aux;
	header.setPrevBlock(prev_block_str);
	aux = body.toString();
	header.setTxnsHash(aux);
	header.setBits(diffic);
	header.setNonce(header.getPrevBlock(),header.getTxnHash(),header.getBits());
}

block & block::operator=(const block & right)
{
	if(&right != this)
	{
		header = right.header;
		body = right.body;
		return *this;
	}
	return *this;
}

string block::setBody(istream *iss)
{
	string str;
	getline(*iss, str, '\n');
	
	size_t txn_count = stoi(str);
	//validar que sea numero
	body.setTxnCount(txn_count);
	//body.txnsArrRedim(1); //Se inicializa en uno. Tiene redimensionamiento automatico a
						 // traves de metodos de la clase.
	str=body.setTxns(iss);
	if (isHash(str)==true)
	{
		return str;
	}	
	else if (str=="")
	{
		return str;
	}	
	else if(str!="OK")
	{
		cerr<< "ERROR: set txns fallo \n"<< str << endl;
	   	exit(1);
	}
	return "OK";
}

block::block()
{
	header.setPrevBlock(NULL_HASH);
	header.setTxnsHash(NULL_HASH);
	header.setBits(0);
	header.setNonce(0);

	body.setTxnCount(0);
	//El campo txn tiene su propio inicializador base. No hace falta ponerlo
}

block::block(const string str,const  size_t diffic, istream *iss)
{
	setBody(iss);
	setHeader(str, diffic);
}

block::block(const string block_str)
{
//  cout <<  "entre a block () argumento :\n" << block_str << ">>" << endl;
	istringstream ss(block_str);
	string aux;
	int aux_int;
	getline(ss, aux, '\n');
	this->header.setPrevBlock(aux);
	getline(ss, aux, '\n');
	this->header.setTxnsHash(aux);
	getline(ss, aux, '\n');
	aux_int=stoi(aux);
	this->header.setBits(aux_int);
	getline(ss, aux, '\n');
// cout << "nonce " << aux << endl;
	aux_int=stoi(aux);
	this->header.setNonce(aux_int);
	getline(ss, aux, '\n');
	aux_int=stoi(aux);
	body.setTxnCount(aux_int);
// cout << "txn count " << aux << endl;
	//string prueba;
	//ss >> prueba;
//cout << "prueba  loq ue queda de ss \n" << prueba << ">>" << endl;
	body.setTxns(&ss);

}

block::~block()
{
}


void block::addTxn(txn aux_txn)
{
	//cout << "add txn body  < " << body << ">>" << endl;
	// cout << "add txn  < " << aux_txn << ">>" << body.getTxnCount()<< endl; 
	body.setTxnCount(body.getTxnCount()+ 1);
	
	// cout << "add txn body txn count < " << body << ">>" << body.getTxnCount() << endl; 
	body.txnsArrRedim(body.getTxnCount());
	// cout << "add txn body txn arrredim < " << body << ">>" << endl;
	body.getTxns()[body.getTxnCount()-1]=aux_txn;
	// cout << "add txn body txns < " << body.getTxns() << ">>" << endl;
	// cout << "add txn body  < " << body << ">>" << endl;
	// cout << "add txn body  to string < " << body.toString() << ">>" << endl;
	//cout << "add txn body  get < " << body.getBody() << ">>" << endl;
}


void block::show(ostream& oss)
{
	oss << header;
	oss << body;
}

string block::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}
#endif //_BLOCK_H_