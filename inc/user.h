#ifndef _USER_H_
#define _USER_H_

#include "finders.h"
#include "block.h"
#include "Lista.h"
#include "Array.h"


class user
{
	string name;
	double balance;
	list <txn> transactions; //Solo se guardan las transacciones donde user aparece como output.
	public:
	user();
	user(string);
	~user();
	string getName();
	double getBalance();
	list<txn>& getTransactions();

	void setName(const string &);
	void setBalance(const double &);
	void setTransactions(const list<txn> &);
	void show(ostream&);
	
	string toString();
	friend ostream& operator<<(ostream& oss, user& usr) 
	{
		usr.show(oss);
		return oss;
	}

	user const &operator=(user const &);
	Array<inpt> trackMoney(const double);
	void addTxn(const txn);
	void loadTxn(txn tran);

	bool operator==( const user & ) const;
};

user::user()
{
	name = "";
	balance = 0;
}

user::~user()
{
}

user::user(string str_user)
{
	//Para crear el usuario a partir de una string, se considera que dicha string contiene todos los campos de user separados 
	//por fines de linea (\n): name,balance y transactions.
	istringstream ss(str_user);
	bdy aux_body;

	string aux_str;
	txn aux_txn;
	getline(ss, this->name, '\n');
	getline(ss, aux_str, '\n');
	balance = stod(aux_str);
	aux_body.setTxns(&ss); 

	Array <txn> array_aux_txns=aux_body.getTxns();
	for(size_t i=0; i<aux_body.getTxnCount(); i++)
	{
		this->transactions.append(array_aux_txns[i]);
	}
}

string user::getName()
{
	return name;
}
double user::getBalance(){return balance;}
list<txn>& user::getTransactions(){return transactions;}

void user::setName(const string & str){name = str;}
void user::setBalance(const double & n){balance = n;}
void user::setTransactions(const list<txn> & d){transactions = d;}

void user::show(ostream& oss)
{
	if(name == "")
		return ;
	oss << name << endl;

	oss << balance << endl;
	
	oss << transactions;
}

string user::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}

user const &user::operator=(user const &right)
{
	if(&right != this)
	{
		name = right.name;
		balance = right.balance;
		transactions = right.transactions;
		return *this;
	}
	return *this;
}

Array<inpt> user::trackMoney(const double money)
{
	txn aux_txn;
	size_t inpt_iter = 0;
	size_t i, j = 0;
	Array<outpt> aux_outputs;
	Array<inpt> inputs;
	double utxo = 0;
	while(utxo < money)
	{
		aux_txn = transactions.getLastNode();
		aux_outputs = aux_txn.getOutputs();
		for (i = 0; i < aux_txn.getNTxOut(); i++)
		{
			if(aux_outputs[i].getAddr() == name)
			{
				utxo += stod(aux_outputs[i].getValue());
				break;
			}
		}
		inputs[inpt_iter].setInput(sha256(sha256(aux_txn.toString())), i, name);
		transactions.removeElement(aux_txn);
		inpt_iter++;
	}
	balance -= utxo;
	inputs.ArrayRedim(inpt_iter);
	return inputs;
}

void user::addTxn(txn tran)
{
	// cout << "NOMBRE DEL USUARIO QUE SE LE AGREGA UNA TRANSACCION " << name << endl;
	// cout << "TRANSACCION QUE SE QUIERE AGREGAR " << tran << endl;
	// cout << "HASH DE LA TRANSACCION " << sha256(sha256(tran.toString())) << endl;
	
	Array<inpt> aux_inpt;
	transactions.append(tran);
	if(name != tran.getInputs()[0].getAddr())
	{
		for (size_t i = 0; i < tran.getNTxOut(); i++)
		{
			if(name == tran.getOutputs()[i].getAddr())
			{
				balance += stod(tran.getOutputs()[i].getValue());
				break;
			}
		}
	}
	else 
	{
		double spent_value = 0;
		for (size_t i = 0; i < tran.getNTxOut(); i++)
		{
			if(tran.getOutputs()[i].getAddr() == name) 
				continue;
			else 
			{
 				spent_value += stod(tran.getOutputs()[i].getValue());
			}
		}
		balance -= spent_value;
	}
}

void user::loadTxn(txn tran)
{
	string aux_str_txn;
	double source_value = 0, spent_value = 0, change;
	Array<inpt> inputs = tran.getInputs();
	Array<outpt> outputs = tran.getOutputs();
	
	if(name == inputs[0].getAddr())
	{
		for (size_t i = 0; i < tran.getNTxIn(); i++)
		{
			if((aux_str_txn = transactions.find(STR_TXN_BY_HASH, inputs[i].getOutPoint().tx_id)) == "Findnt")
			{
				cerr << "Error en la carga del user: < " << name << ">" << endl;
				exit(1);
			}
			txn aux_txn(aux_str_txn);
			transactions.removeElement(aux_txn);

			if(aux_txn.getOutputs()[inputs[i].getOutPoint().idx].getAddr() != name)
			{
				cerr << "Error en la carga del user: << " << name << ">>" << endl;
				exit(1);
			}
			source_value += stod(aux_txn.getOutputs()[inputs[i].getOutPoint().idx].getValue());
		}
		for (size_t i = 0; i < tran.getNTxOut(); i++)
		{
			spent_value += stod(outputs[i].getValue());
			if(outputs[i].getAddr() == name)
				change = stod(outputs[i].getValue());
		}

		if(spent_value != source_value)
		{
				cerr << "Error en la carga del user: <<< " << name << ">>>" << endl;
			exit(1);
		}

		balance = balance - spent_value + change;
	}
	else
	{
		for (size_t i = 0; i < tran.getNTxOut(); i++)
		{
			if(outputs[i].getAddr() == name)
			{
				balance += stod(outputs[i].getValue());
				break;
			}
		}
	}
	transactions.append(tran);
}


bool user::operator==( const user & right) const
{
	if(name == right.name && balance == right.balance) //FALTA EL == DE LISTA
		return true;
	else
		return false;
}
#endif //_USER_H_