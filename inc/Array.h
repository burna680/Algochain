#ifndef  _ARRAY_INCLUDED_
#define _ARRAY_INCLUDED_
#include <iostream>
#include "arrayPrototype.h"

using namespace std;

#define INIT_SIZE_ARRAY 10 

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
bool Array<T>::operator==(const Array<T> &right) const
{
	if(vsize != right.vsize)
	{
		return false;
	}
	else
	{
		for(size_t i = 0; i<right.vsize; i++)
		{
			if(ptr[i] == right.ptr[i])
				continue;
			else
				return false;
			
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

template <class T>
Array<T> Array<T>::getSubArray(const size_t n1,const size_t n2)
{
	Array<T> aux;
	aux.vsize=0;
	if(n1>this->getSize())
		return aux;
	else 
	{
		if(n2<=this->getSize())
		{
			aux.ArrayRedim(n2-n1+1);
			for(size_t i=n1-1; i<n2; i++)
			aux[i-n1+1]=this->ptr[i];
		}
		if(n2>this->getSize())
		{
			aux.ArrayRedim(this->getSize()-n1+1);
			for(size_t i=n1-1; i<this->getSize(); i++)
			aux[i-n1+1]=this->ptr[i];
		}
	}
	return aux;
}

#endif  //ARRAY_INCLUDED