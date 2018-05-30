/* Group: Iowa
Nicholas Gale
Neda Heydari
Hosna Paiam
Michael Totten
Brenda Tran
Vaso Vasich
*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include <iostream>
#include "file_parser.h"
#include <map>
#include <string>

using namespace std;

class symtab{
	public:
		symtab();
		~symtab();
		
		void add_symbol(string, string, unsigned int);
		string get_address(string);
		bool symbol_exists(string);
	


	private:
		map<string, string> sym_table;
		string int_to_string(int);
		void error_msg(unsigned int, string);

};




#endif
