#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>

#include "../inc/cmdline.h"
#include "../inc/sha256.h"
#include "../inc/block.h"
#include "../inc/main.h"

using namespace std;

int main(int argc, char * const argv[])
{
	string str = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	cmdline cmdl(options);	// Objeto con parametro tipo option_t (struct) declarado globalmente. Ver línea 51 main.cc
	cmdl.parse(argc, argv); // Metodo de parseo de la clase cmdline

	if (iss->bad()) {
		cerr << "cannot read from input stream."
		     << endl;
		exit(1);
	}

	block block0(str, difficulty, iss);

	str = block0.getBlockAsString();
	
	if (oss->bad()) {
		cerr << "cannot write to output stream."
		     << endl;
		exit(1);
	}
	*oss << str;
}