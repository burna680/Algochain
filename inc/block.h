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
#include "math.h"
#include "tools.h"
#include "Array.h"

// o Se usaran strings para representar los hash
//
// o Jerarquía de clases: Bloque > Header/Body>Txn > Input/Output > Outpoint (estructura que pertenece a Input).

//--------------------------ESTRUCTURA OUTPOINT----------------------------------------------------------------------------------------
struct outpnt
{
	string tx_id; //Es un hash de la transaccion de donde este input toma fondos
	size_t idx; //Valor entero no negativo que representa un indice sobre la secuencia de outputs de la transaccion con hash tx id
};
//--------------------------CLASE INPUT----------------------------------------------------------------------------------------
class inpt
{   
	outpnt outpoint;
	string addr; //La direccion de origen de los fondos (que debe coincidir con la direccion del output referenciado)
	public:
	inpt(); //Creador base
	inpt(string&); //Creador mediante una string
	~inpt( ); //Destructor
	//Si hay getters deberian haber setters. Si no se usan, eliminarlos.
	string getAddr();
	outpnt getOutPoint();
	string getInputAsString();
};
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

outpnt inpt::getOutPoint(){return outpoint;}

string inpt::getAddr(){return addr;}

string inpt::getInputAsString()
{
	stringstream ss; 
	string aux;

	ss<<(this->getOutPoint().idx);	 //Pasaje de size_t   
	ss>>aux; 						// a string
	
	string result;
	result.append((this->getOutPoint()).tx_id);
	result.append(" ");
	result.append(aux);
	result.append(" ");
	result.append((this->getAddr()));

	return result;
}
//--------------------------CLASE OUTPUT----------------------------------------------------------------------------------------
class outpt
{
	double value; //La  cantidad de Algocoins a transferir en este output
	string addr; //La direccion de origen de los fondos (que debe coincidir con la direccion del output referenciado)

	public:

	outpt(); //Creador base
	outpt(string&); //Creador mediante una string
	~outpt( ); //Destructor
	double getValue();
	string getAddr();
	string getOutputAsString();
};

outpt::outpt() //Creador base
{    
}

outpt::~outpt() //Destructor base
{    
}

outpt::outpt(string & str) //Creador mediante una string
{
	istringstream ss(str);
	string str_value,str_addr;

	getline(ss, str_value, ' '); // Se recorren los campos. Si el formato es erroneo, se detecta como una cadena vacia.
	getline(ss, str_addr, '\n');

	if((isNumber<double>(str_value)==1) && (isHash(str_addr)==true))
	{
		this->value=stod(str_value);
		this->addr=str_addr;
	}
	else
	{
		this->addr=ERROR;
	}
}

string outpt::getAddr(){return addr;}

double outpt::getValue(){return value;}

string outpt::getOutputAsString()
{
	string aux;
	string str_exact_precision;
	string result;
	ostringstream str_os;

	size_t i;
	aux=to_string(this->value);	
	for(i=aux.length()-1; aux[i] -'0'==0 ;i--); //Indica la posicion con decimales exactos (sin ceros de mas)
	str_exact_precision=aux.substr(0,i+1); //Se copia la sub cadena desdeada

	result.append(str_exact_precision);
	result.append(" ");
	result.append((this->getAddr()));
	return result;
}

//--------------------------CLASE TXN----------------------------------------------------------------------------------------


class txn
{   
	private:
	size_t n_tx_in; //Indica la cantidad total de inputs en la transaccion
	size_t n_tx_out; //Indica la cantidad total de outputs en la transaccion
	Array <inpt> tx_in; //Datos de entrada para las transacciones
	Array <outpt> tx_out; //Datos de salida para las transacciones

	public:
	txn(); //Creador base
	~txn( ); //Destructor

	void setNTxIn(const size_t) ;
	void setNTxOut(const size_t);
	bool setTxIn(const size_t n, istream *iss); // Seteador que valida los datos y devuelve un booleano para el error
	bool setTxOut(const size_t n, istream *iss);

	size_t getNTxIn();
	size_t getNTxOut();
	Array<inpt>& getInputs();
	Array<outpt>& getOutPuts();

	string getTxnAsString();
	string validateTxn();
};


txn::txn()
{
	n_tx_in=0;
	n_tx_out=0;
	tx_in.ArrayRedim(0);
	tx_out.ArrayRedim(0);
}

txn::~txn()
{
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


bool txn::setTxIn(const size_t n, istream *iss)  //Se modifica el retorno del setter por defecto (void) por
{												// necesidad. Verifica si el setteo pudo realizarse correctamente.
	string aux_s;
	for (size_t i = 0; i < n; i++)
	{
		getline(*iss, aux_s, '\n');
		inpt in(aux_s);
		if(isError(in.getAddr())==false)
			return false;
		tx_in[i] = in;
	}
	return true;
}


bool txn::setTxOut(const size_t n, istream *iss) //Se modifica el retorno del setter por defecto (void) por
												// necesidad. Verifica si el setteo pudo realizarse correctamente.
{
	string aux_s;
	for (size_t i = 0; i < n; i++)
	{
		getline(*iss, aux_s, '\n');
		outpt out(aux_s);
		if(isError(out.getAddr())==false)
			return false;
		tx_out[i] = out;
	}
	return true;
}

size_t txn::getNTxIn(){return n_tx_in;}

size_t txn::getNTxOut(){return n_tx_out;}

Array<inpt>& txn::getInputs(){return tx_in;}
Array<outpt>& txn::getOutPuts(){return tx_out;}

string txn::getTxnAsString()
{
	
	string result, aux;
	aux = to_string(n_tx_in);
	if(((this->getNTxIn()==0)) && ((this->getNTxOut())==0))
		return result.append("0");
	result.append(aux);
	result.append("\n");
	for(size_t i = 0; i < n_tx_in; i++)
	{
		result.append(tx_in[i].getInputAsString());
		result.append("\n");
	}
	aux = to_string(n_tx_out);
	result.append(aux);
	result.append("\n");
	for(size_t i = 0; i < n_tx_out; i++)
	{
		result.append(tx_out[i].getOutputAsString());
		result.append("\n"); //Es necesario para separar las transacciones al enviarlas al flujo de salida
	}
	return result;
}
//--------------------------CLASE BODY----------------------------------------------------------------------------------------

class bdy
{
	size_t txn_count;
	Array <txn> txns;
	public:
	bdy();
	~bdy();
	bdy getBody();
	string getBodyAsString();
	size_t getTxnCount();
	Array<txn> getTxns();
	string getTxnAsString();
	string setTxns(istream *iss);
	void setTxnCount(const size_t n);
	void txnsArrRedim(const size_t );
};
bdy::bdy()
{	
}

bdy::~bdy(){}

void bdy::setTxnCount(const size_t n)
{
	txn_count = n;
	if(this->txns.getSize() == 0)
	{
		txns.ArrayRedim(n);
	}
}

string bdy::setTxns(istream *iss)
{
	string str,error_string;
	size_t aux, i = 0;
	bool err;

	while(getline(*iss, str, '\n'))
	{
		if(i >=txns.getSize())
		{
			txns.ArrayRedim(txns.getSize()*2); // Dependiendo de cuantos datos haya que analizar se puede modificar
		}									  // la estrategia de crecimiento del arreglo.

		// Se verifica n_tx_in
		if(isNumber<size_t>(str)==0 || (str[0]) == '\0')
		{
			err=true;
			break;
		}

		aux = stoi(str);
		txns[i].setNTxIn(aux);

		// Se verifican las entradas
		if(txns[i].setTxIn(aux, iss)==false)
		{
			err=true;
			break;
		}

		// Se verifica n_tx_out
		getline(*iss, str, '\n');

		if(isNumber<size_t>(str)==0 || (str[0]) == '\0')
		{
			err=true;
			break;
		}
		aux = stoi(str);
		txns[i].setNTxOut(aux);

		// Se verifican las salidas
		if(txns[i].setTxOut(aux, iss)==false)
		{
			err=true;
			break;
		}
		i++;
	}
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
	txn_count = i;
	return "\0";
}

string bdy::getBodyAsString()
{
	string result, str,aux;
	str = to_string(txn_count);
	result.append(str);
	result.append("\n");

	if((txn_count==1) && (!txns[0].getTxnAsString().compare("0")))
		return "0\n";

	for (size_t i = 0; i < txn_count; i++)
	{
		result.append(txns[i].getTxnAsString());
	}
	
	return result;
}

size_t bdy::getTxnCount(){return txn_count;}
Array<txn> bdy::getTxns(){return txns;}

bdy bdy::getBody(){return *this;}

void bdy::txnsArrRedim(const size_t n ){txns.ArrayRedim(n);}

//--------------------------CLASE HEADER----------------------------------------------------------------------------------------

class hdr
{
	string prev_block;//El hash del bloque completo que antecede al bloque actual en la Algochain.
	string txns_hash;//El hash de todas las transacciones incluidas en el bloque.
	size_t bits;    // Valor entero positivo que indica la dificultad con la que fue minada este bloque.
	size_t nonce;  // Un valor entero no negativo que puede contener valores arbitrarios. El objetivo de este 
				  // campo es tener un espacio de prueba modificable para poder generar hashes sucesivos hasta 
				 // satisfacer la dificultad del minado.
	public:
	hdr();
	~hdr();
	bool setPrevBlock(const string&);
	void setTxnsHash(const string&);
	void setBits(const size_t n);
	void setNonce(const string prev_block,const  string txns ,const  size_t bits);
	string getPrevBlock();
	string getTxnHash();
	size_t getBits();
	size_t getNonce();
	string getHeaderAsString();
};

hdr::hdr()
{
	prev_block = "\0";
	txns_hash = "\0";
	bits = 0; 	//Se podría hacer que los bits y el nonce fueran ints para detectar errores haciendo que estos valgan -1 (por ej)
	nonce = 0;
}

hdr::~hdr(){}


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


void hdr::setTxnsHash(const string & str){txns_hash = sha256(sha256(str));}


void hdr::setBits(const size_t n){bits = n;}


string hdr::getPrevBlock(){return prev_block;}


string hdr::getTxnHash(){return txns_hash;}


size_t hdr::getBits(){return bits;}


size_t hdr::getNonce(){return nonce;}


string hdr::getHeaderAsString()
{
	string str;
	string bit_string = to_string(bits); //convierto el bits a string y lo agrego a la string
	string nonce_string; //para guardar cuando transforme en string del nonce

	str.append(prev_block);//pongo primero en la string el prev block
	str.append("\n");//PREGUNTAR si no viene con el barra n, creo que no pero si no ya esta
	str.append(txns_hash);//agrego el txns, ver comentario de la linea 25
	str.append("\n");
	str.append(bit_string);
	str.append("\n");
	nonce_string = to_string(nonce); //convierto el nonce a string
	str.append(nonce_string); 
	return str;

}
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
		header_str = getHeaderAsString();
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

//--------------------------CLASE BLOCK----------------------------------------------------------------------------------------
class block
{   
	private:
	hdr header;
	bdy body;

	public:
	block(); //Creador base
	block(const string,const  size_t, istream*); //Creador en base al hash del bloque previo, al nivel de  
												//dificultad y un flujo de entrada por el que se reciben las 
											   // transacciones.
	~block( ); //Destructor
	void setHeader(const string&,const size_t);
	void setBody(istream *iss);

	string getBlockAsString();
};

void block::setHeader(const string& prev_block_str,const size_t diffic)
{
	string aux;
	header.setPrevBlock(prev_block_str);
	aux = body.getBodyAsString();
	header.setTxnsHash(aux);
	header.setBits(diffic);
	header.setNonce(header.getPrevBlock(),header.getTxnHash(),header.getBits());
}

void block::setBody(istream *iss)
{
	string str;
	body.setTxnCount(0);
	body.txnsArrRedim(1); //Se inicializa en uno. Tiene redimensionamiento automatico a
						 // traves de metodos de la clase.
	if((str=body.setTxns(iss))!="\0")
	{
	   cerr<<str<<endl;
	   exit(1);
	};
}

block::block(const string str,const  size_t diffic, istream *iss)
{
	setBody(iss);
	setHeader(str, diffic);
}

block::~block()
{
}

string block::getBlockAsString()
{
	string result, str;
	result.append(header.getHeaderAsString());
	result.append("\n");
	result.append(body.getBodyAsString());
	return result;
}
#endif //_BLOCK_H_