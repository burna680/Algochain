#ifndef _BLOCKPROTOTYPE_H_
#define _BLOCKPROTOTYPE_H_

#include <iostream>
#include <string.h>
#include "Array.h"

using namespace std;

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
	inpt & operator=(const inpt &);
	//Si hay getters deberian haber setters. Si no se usan, eliminarlos.
	string getAddr();
	outpnt getOutPoint();
	void setInput(string aux_tx_id, size_t aux_idx, string aux_addr);
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, inpt& in) 
	{
		in.show(oss);
		return oss;
	}
	bool operator==( const inpt &) const;
	bool operator!=(const inpt &);
	string toString();
};

//--------------------------CLASE OUTPUT----------------------------------------------------------------------------------------
class outpt
{
	string value; //La  cantidad de Algocoins a transferir en este output
	string addr; //La direccion de origen de los fondos (que debe coincidir con la direccion del output referenciado)

	public:

	outpt(); //Creador base
	outpt(string&); //Creador mediante una string
	outpt(string&, string &);
	~outpt( ); //Destructor
	outpt & operator=(const outpt &);
	string getValue();
	string getAddr();
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, outpt& out) 
	{
		out.show(oss);
		return oss;
	}
	bool operator==( const outpt &) const;
	string toString();
};


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
	txn(Array<string>&); //Creador en base a un array de cadenas. El array debe contener todos los campos necesarios
						// para crear la transaccion.
	txn(string txn_str); //Creador en base a una cadenas que contiene toda la informacion para crear la transaccion.
	~txn( ); //Destructor
	txn &operator=( const txn & );

	void setNTxIn(const size_t) ;
	void setNTxOut(const size_t);
	string setTxIn(const size_t n, istream *iss); // Seteador que valida los datos y devuelve un booleano para el error
	bool setTxIn(Array<inpt>&);
	bool setTxIn(const size_t n, Array<string>& tx_in_str_arr);
	string setTxOut(Array <string>, Array<string>);
	bool setTxOut(const size_t, Array<string>&);
    string setTxOut(const size_t n, istream *iss);
	string setTxOutFile(const size_t n, istream *iss);

	size_t getNTxIn();
	size_t getNTxOut();
	Array<inpt>& getInputs();
	Array<outpt>& getOutputs();

	string validateTxn();
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, txn& tx) 
	{
		tx.show(oss);
		return oss;
	}
	string toString();
	bool operator==( const txn &) const;
};

//--------------------------CLASE BODY----------------------------------------------------------------------------------------

class bdy
{
	friend class block;
	size_t txn_count;
	Array <txn> txns;
	public:
	bdy();
	~bdy();
	bdy & operator=(const bdy &);
	bdy getBody();
	size_t getTxnCount();
	Array<txn> &getTxns();
	// void setTxns(Array <txn> txns);
	string setTxns(istream *iss);
	void setTxnCount(const size_t n);
	void txnsArrRedim(const size_t );
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, bdy& body) 
	{
		body.show(oss);
		return oss;
	}
	string toString();
};	

//--------------------------CLASE HEADER----------------------------------------------------------------------------------------

class hdr
{
	friend class block;
	string prev_block;//El hash del bloque completo que antecede al bloque actual en la Algochain.
	string txns_hash;//El hash de todas las transacciones incluidas en el bloque.
	size_t bits;    // Valor entero positivo que indica la dificultad con la que fue minada este bloque.
	size_t nonce;  // Un valor entero no negativo que puede contener valores arbitrarios. El objetivo de este 
				  // campo es tener un espacio de prueba modificable para poder generar hashes sucesivos hasta 
				 // satisfacer la dificultad del minado.
	public:
	hdr();
	~hdr();
    
	hdr & operator=(const hdr &);
	bool setPrevBlock(const string&);
	void setTxnsHash(const string&);
	void setBits(const size_t n);
	void setNonce(const string prev_block,const  string txns ,const  size_t bits);
	void setNonce(const  size_t nonce);
	string getPrevBlock();
	string getTxnHash();
	size_t getBits();
	size_t getNonce();
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, hdr& header) 
	{
		header.show(oss);
		return oss;
	}
	string toString();
};


//--------------------------CLASE BLOCK----------------------------------------------------------------------------------------

class block{   
	private:
	hdr header;
	bdy body;

	public:
	block(); //Constructor base
	block(const string,const  size_t, istream*); //Constructor en base al hash del bloque previo, al nivel de  
												//dificultad y un flujo de entrada por el que se reciben las 
											   // transacciones.
	block(const string ); //Constructor en base a una string.
	~block( ); //Destructor
	block & operator=(const block &);
	void setHeader(const string&,const size_t);
	string setBody(istream *iss);
	void setHeader(hdr header);
	void setBody(bdy body);
	void setBlockFromFile(istream *iss);
	hdr getHeader();
	bdy getBody();
	void addTxn(txn aux_txn);
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, block& block) 
	{
		block.show(oss);
		return oss;
	}
	string toString();
};

#endif// _BLOCKPROTOTYPE_H_