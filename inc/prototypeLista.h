#ifndef _PROTOTYPELISTA_H_
#define _PROTOTYPELISTA_H_
template<class T>
class list
{
	class node  
	{
		// La list puede tener  acceso a los atributos privados del nodo para poder 
		//modificarlos, dado que la clase nodo es inherente a la clase list.

		T data;
		node* next;
		node* prev;
		friend class list;       

		public:
		node(T const &data_){data=data_ ; next= NULL; prev=NULL;}; //Constructor a partir de dato. 
		//Se asume que siempre que se quiera crear un nodo se tendrá la informacion que va a contener.
		node* getNext(){return this->next;}
		node* getPrev(){return this->prev;}
		T getData(){return this->data;}
		~node(){}; //Destructor.
	};

	node* first;
	node* last;
	size_t max_size; //Tamaño de la lista.Si la lista no esta vacia max_size>=1
					// Si la lista esta vacia max_size=0
	public:
	list(); //Constructor basico
	list(const list &); //Constructor en base a otra list.
	~list(); //Destructor
	void append(const T& t); //Agregar nodo al final de la list.
	void insert(const T& t); //Agregar nodo al principio de la list.
	bool placeElement(const T& t, size_t n=1);//Agregar nodo en la posicion n de la list. Si no lo pudo agregar
											 //devuelve false. Por defecto lo agrega al principio.
	bool empty(); //Verifica si la list está vacia.
	T  find(const T& t); //Encuentra el ultimo nodo que contiene el dato T. Si  no lo encuentra, debe devolver NULL.
							  //DEBERIA devolver un puntero a un dato dentro de un nodo de la lista,DEBE devolver un puntero a un dato constante.
	string find(const string& ref,const string& d ); //Encuentra el dato "d" de  tipo "ref" en su última aparición en la lista.
													//Ejemplo: ref=value d=Carla. Devuelve una string con el el ultimo output de Carla
												   // Ejemplo: ref= id   d=<valor del hash>. Devuelve el bloque como string.
												  // Si  no lo encuentra, devuelve una cadena vacia.                          
	void removeElement(const T& t); //Elimina el primer nodo que contiene al dato t. Devuelve false si no pudo eliminarlo.
	size_t size(); //Obtiene el tamaño de la list
	// list const &operator=(const list& other_list);
	void show(ostream&);
	friend ostream& operator<<(ostream& oss, list& l) 
	{
		l.show(oss);
		return oss;
	}

	list const &operator=(list const &);
	string toString();
	T getFirstNode();
	T getLastNode();
	bool deleteFirstNode();
	bool deleteLastNode();
	bool contains(const T &elem) const;
};

#endif //_PROTOTYPELISTA_H_