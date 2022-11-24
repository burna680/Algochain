#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
using namespace std;

#include "../inc/Lista.h"
#include "../inc/cmdline.h"
#include "../inc/sha256.h"
#include "../inc/block.h"
#include "../inc/dictionary.h"
#include "../inc/main.h"


// agregar una variable global lista algochain static list<block> algochain;
// static block mempool();


int main(int argc, char * const argv[])
{
	cmdline cmdl(options);
	cmdl.parse(argc, argv); // Metodo de parseo de la clase cmdline

	if (iss->bad()) {
		cerr << "cannot read from input stream."
		     << endl;
		exit(1);
	}

	string aux, aux1, aux2;
	Array <string> arr;
	p_func cmd;
	int num_param;

	cout << "Empieza el codigo" << endl;
	while(aux1 != "exit")
	{
		getline(*iss, aux1, ' ');
		if(aux1=="Exit" || aux1=="exit") break;
		cmd = dictCmds(aux1, num_param);
		getline(*iss, aux2, '\n');
		arr = parseCmdArgs(aux2, num_param);
		aux = cmd(arr);
		*oss << aux << endl;
	}


 	return 0;
}

