#ifndef  _ARRAY_INCLUDED_
#define _ARRAY_INCLUDED_
#include <iostream>
#define INIT_SIZE_ARRAY 10 
using namespace std;

template<class T>
class Array
{
	public:
		Array(); //Constructor base
		Array( const Array<T> & ); //Constructor-Copiador
		Array(size_t); //Constructor mediante tamaño del array
		~Array( ); //Destructor
		void ArrayRedim(size_t); //Redimensionador de arrays.
		size_t getSize( )const; //Método: determina el tamaño del array
		Array<T> &operator=( const Array<T> & ); //Operador asignación para una array: A=B, donde A y B son arrays. Recibe como parámetro un array por referencia constante, para no modificar lo que tiene dentro
		bool operator==( const Array<T> & ) ; //Operador lógico para comprobar si son iguales 2 arrays. Recibe como parámetri un array por referencia constante, para no modificar lo que tiene dentro
		T & operator[](size_t); //Operador indexación: Retorna un elemento del vector (se puede cambiar, pues se retorna por referencia)
		template <class TT>
		friend std::ostream &operator<<(std::ostream&, Array <TT> &); //Operador de impresion de salida
		bool empty(); // Verifica si un arreglo esta vacio.


	private:
		size_t rsize; //Atributo que indica el tamaño del array 
		size_t vsize;
		T *ptr; //Atributo que indica la dirección donde inicia el puntero
};


template <class T>
Array<T>::Array()
{
	ptr = new T[INIT_SIZE_ARRAY];
	rsize = INIT_SIZE_ARRAY;
	vsize = INIT_SIZE_ARRAY;
}

template <class T>
Array<T>::Array(const size_t init_size)
{
	ptr = new T[init_size];
	rsize = init_size;
	vsize = init_size;
}


template <class T>
Array<T>::Array(const Array<T> &init)
{
	rsize = init.vsize;
	vsize = init.vsize;

	ptr = new T[init.vsize];
	for(size_t i=0; i< init.vsize; i++)
	{
		ptr[i] = init.ptr[i]; //ASUMO QUE T tiene el operador = 
	}
}

template <class T> 
Array<T>::~Array() //Implementación del destructor de Array
{
	if (ptr)
	delete [ ] ptr; //Destructor de memoria dinámica.
}


template <class T>
size_t Array<T>::getSize()const { return vsize; } //Implementación del getter del tamaño del Array.


template <class T>
Array<T>& Array<T>::operator=(const Array<T> &right)
{
	if(&right != this)
	{
		if(rsize != right.vsize)
		{
			T *aux;
			aux = new T[right.vsize];
			delete [] ptr;
			rsize = right.vsize;
			ptr = aux;
		}

		for (size_t i = 0; i<right.vsize; i++)
		{
			ptr[i] = right.ptr[i];
		}
		vsize = right.vsize;
		return *this;
	}
	return *this;
}

template <class T>
bool Array<T>::operator==(const Array<T> &right)
{
	if(vsize != right.vsize)
	{
		return false;
	}
	else
	{
		for(int i = 0; i<right.vsize; i++)
		{
			if(ptr[i] != right.ptr[i])
			{
				return false;
			}	
		}
		return true;
	}
}

template<class T>
T &Array<T>::operator[](size_t subscript)
{	
	if(subscript >= vsize)
		std::abort();
	return ptr[subscript];
}

template <class T>
std::ostream &operator <<(std::ostream &os, Array<T> &arr)
{
		int i, size;
		size = arr.getSize();
		for(i=0; i<size; i++)
		{
				os << arr[i] << endl; //asumo que T tiene sobrecargado el <<
		}
		return os;
}


template<class T>
void Array<T>::ArrayRedim(size_t new_size)
{
	if (new_size > rsize) {
		T *aux = new T[new_size];
		for (size_t i = 0; i < vsize; ++i)
			aux[i] = ptr[i];
		delete[] ptr, ptr = aux;
		rsize = new_size;
	}
	
	vsize = new_size;
}

template <class T>
bool Array<T>::empty()
{
	if(vsize)
		return true;
	else 
		return false; 
}

#endif  //ARRAY_INCLUDED