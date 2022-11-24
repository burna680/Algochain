#ifndef _LISTA_H_
#define _LISTA_H_

#include "math.h"
#include "prototypeLista.h"
#include "finders.h"

// Implementacion de list doblemente enlazada no circular utilizando
// templates e iteradores.

//Se aprovecha el hecho de que la lista tiene una referencia del principio y el final para reducir a la mitad
//el procesamiento de las altas,bajas y borrado de las listas.

//Los nodos son creados con memoria dinamica.


template<typename T>
list<T>::list(){first=NULL;last=NULL;max_size=0;}

template<typename T>
list<T>::list(const list &orig) : first(0), last(0), max_size(orig.max_size)
{
	node *iter;
	node *ant;

	// Recorremos la secuencia original en sentido directo. En cada paso,
	// creamos un nodo, copiando el dato correspondiente, y lo enganchamos
	// al final de nuestra nueva lista.
	//
	for (iter = orig.last, ant = 0; iter != 0; iter = iter->next)
	{
		// Creamos un nodo, copiando el dato, y lo enganchamos en e
		// final de nuestra lista.
		//
		node *new_node = new node(iter->data);
		new_node->prev = ant;
		new_node->next = 0;

		// Si ésta no es la primera pasada, es decir, si no se trata
		// del primer nodo de la lista, ajustamos el enlace sig_ del
		// nodo anterior.
		//
		if (ant != 0)
			ant->next = new_node;

		// Además, tenemos que ajustar los punteros a los elementos
		// distinguidos de la secuencia, primero y último. En el caso
		// de pri_ (enlace al primer elemento), esto lo vamos a
		// hacer una única vez; para el caso de ult_, iremos tomando
		// registro del último nodo procesado, para ajustarlo antes
		// de retornar.
		//
		if (first == 0)
			first = new_node;
		ant = new_node;
	}
	
	// Ajustamos el puntero al último elemento de la copia.
	last = ant;
}

template<typename T>
bool list<T>::empty()
{
	//Forma complicada de preguntar si la lista esta vacia
	// if((this->first==this->last)&&(this->first==NULL)) 
	if(!max_size)
		return true;
	return false;

}


template<typename T>
list<T>::~list()
{
	for (node *p = first; p; )
	{
		node *q = p->next;
		delete p;
		p = q;
	}
}

template<typename T>
size_t list<T>::size(){return this->max_size;}

template<typename T>
void list<T>::append(const T& t)
{
	node *aux= new node(t);

	if( this->empty() )
	{
		aux->next=NULL;
		aux->prev=NULL;
		this->last=aux;
		this->first=aux;
	}
	else
	{
		aux->next=NULL;
		aux->prev=this->last;
		this->last->next=aux;
		this->last=aux;
	}
	this->max_size=this->max_size+1;

}

template<typename T>
void list<T>::insert(const T& t)
{
	node *aux= new node(t);

	if( this->empty() )
	{
		aux->next=NULL;
		aux->prev=NULL;
		this->last=aux;
		this->first=aux;
	}
	else
	{
		aux->prev=NULL;
		aux->next=this->first;
		this->first->prev=aux;
		this->first=aux;
	}
	this->max_size=this->max_size+1;

}

template <typename T>
bool list<T>::placeElement(const T& t, size_t n)
{
	if(n>max_size)
		return false;

	node*aux=new node(t);
	node* p_aux;

	if(n==1)
		this->insert(t);
	else if(n==this->max_size)
		this->append(t);
	else if(n<=floor(this->max_size)/2)
	{    
		aux=this->first;
		aux->prev=NULL;
		for(size_t i=1; i<n ;i++)
		{    
			p_aux=aux;
			aux=aux->next;
			aux->prev=p_aux;
		}
	}
	else if(n>floor(this->max_size)/2)
	{    
		aux=this->last;
		aux->next=NULL;
		for(size_t i=1; i<n ;i++)
		{    
			p_aux=aux;
			aux=aux->prev;
			aux->next=p_aux;
		}
	}
	else 
		return false; //Si por algún motivo no pudo realizarse ninguna de las acciones previas, devuelve error.
	return true;
}


template<typename T>
T list<T>::find(const T& t)
{
	node* prev_;
	node* aux;


	if(this->empty())
	{    
		// cout<<"La lista esta vacia"<<endl;
		return NULL;
	}
	else
	{
		aux=this->last;
		aux->next=this->last->next;
		aux->prev=this->last->prev;

		if(aux->data==t) //Si se encuentra en el ultimo, se devuelve el dato contenido en el ultimo.
		{   
			// cout<<"Encontre el dato, es "<<aux->data<<endl;
			// *data_pointer=aux->data; 
			// cout<<"El valor del contenido del puntero que se devuelve es: "<<*data_pointer<<endl;
			// cout<<"El valor del puntero es "<< data_pointer<<endl;
			return aux->data;
		}

		for(size_t i=this->max_size; i>=1;i--)
		{   
			// cout<<"Llego a la "<<this->max_size-i+1<<"esima iteracion del for"<<endl;
			//Se fija el nodo anterior correctamente
			prev_=aux->prev;
			//Se comprueba que no se haya llegado al ppio de la lista
			if(!prev_)
			{   
				// cout<<"Llegue al principio de la lista. No encontre el dato. Devuelvo NULL"<<endl; 
				return NULL;
			}

			prev_->next=aux;
			prev_->prev=aux->prev->prev; 

			// cout<<"El valor del dato actual es "<<aux->data<<endl;
			// cout<<"El valor del dato actual de prev_ es "<<prev_->data<<endl;

			// cout<<"El valor del dato previo es "<<aux->prev->data<<endl;
			// cout<<"El valor del dato previo de prev_ es "<<prev_->prev->data<<endl;
			//Se comprueba si el dato buscado está en nodo anterior
			if(prev_->data==t)
			{
				// cout<<"Encontre el dato, es "<<prev_->data<<endl;
				// cout<<"Data_pointer tiene cargado: "<<data_pointer<<endl;
				// cout<<"Data_pointer tiene adentro: "<<*data_pointer<<endl;
				// (*data_pointer)=prev_->data; 
				// cout<<"Pude asignarle algo a data_pointer: "<< *data_pointer <<endl;
				return prev_->data;
			}
			//Se retrocede en la lista
			aux=prev_;
			aux->next=prev_->next;
			aux->prev=prev_->prev;
		} 
	}
	// cout<<"No lo encontre, devuelvo NULL"<<endl;
	return NULL; //Si se hubo algun error se devuelve NULL.
}


template<typename T>
string list<T>::toString()
{
	ostringstream ss;
    ss << *this;
    return ss.str();
}

template<typename T>
string list<T>::find(const string& ref,const string& d )
{
	finder aux_finder;
	aux_finder = finderParse(ref);
	string result;

	node* prev_;
	node* aux;

	if(this->empty())
	{
		return "Findnt";
	}
	else
	{
		aux=this->last;
		aux->next=this->last->next;
		aux->prev=this->last->prev;
		if((result = aux_finder(d,(aux->data).toString())) != "Findnt") //Si se encuentra en el ultimo, se devuelve el dato contenido en el ultimo.
		{   
			return result;
		}

		for(size_t i=this->max_size; i>=1;i--)
		{   	
			prev_=aux->prev;
			if(!prev_)
			{  
				return "Findnt";//antes decia null
			}

			prev_->next=aux;
			prev_->prev=aux->prev->prev; 

			if((result = aux_finder(d,(prev_->data).toString())) != "Findnt")
			{
				return result;
			}
			aux=prev_;
			aux->next=prev_->next;
			aux->prev=prev_->prev;
		} 
	}
	return "Findnt";
}


template<typename T>
void list<T>::removeElement(const T &t)
{

	node *iter, *next_=0;

	for (iter = first; iter != 0; iter = next_)
	{
		next_ = iter->next;
		if (t == iter->data)
		{
			node *prev = iter->prev;
			if (prev == 0)
				first = next_;
			else
				prev->next = next_;
			if (next_ == 0)
				last = prev;
			else
				next_->prev = prev;
			delete iter;

			max_size--;
			break;
		}
	}
}

template <typename T>
void list<T>::show(ostream& oss) {
	if(this->empty())
	{
		oss << "Lista vacia" << endl;
		return;
	}
	if(first == NULL){
		oss << "NULL";
		return;
	}
	node* now = first;
	while(now->next != NULL){
		oss << now->data << endl;
		now = now->next;
	}
	oss << now->data << endl;
}
template<typename T>
T list<T>::getFirstNode()
{
    return this->first->data;
}

template<typename T>
T list<T>::getLastNode()
{
    return this->last->data;
}

// string user::toString()
// {
//     ostringstream ss;
//     ss << *this;
//     return ss.str();
// }

template<typename T>
list<T> const &list<T>::operator=(list const &orig)
{
	node *iter_;
	node *next_;
	node *prev_;

	if (this != &orig)
	{
		for (iter_ = first; iter_ != 0; )
		{
			next_ = iter_->next;
			delete iter_;
			iter_ = next_;	
		}

		first = 0;
		last = 0;

		for (iter_ = orig.first, prev_ = 0; iter_ != 0; iter_ = iter_->next)
		{
			node *new_node = new node(iter_->data);
			new_node->prev = prev_;
			new_node->next = 0;
			if (prev_ != 0)
				prev_->next = new_node;
			if (first == 0)
				first = new_node;
			prev_ = new_node;
		}
		last = prev_;
		max_size = orig.max_size;
	}

	return *this;
}


template<typename T>
bool list<T>::contains(const T &elem) const
{
    node *iter;
        
	for (iter = first; iter != 0; iter = iter->next)
		if (elem == iter->data)
			return true;
	return false;
}


#endif // _LIST_H_