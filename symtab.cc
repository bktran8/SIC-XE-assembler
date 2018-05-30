/* Group: Iowa
Nicholas Gale
Neda Heydari
Hosna Paiam
Michael Totten
Brenda Tran
Vaso Vasich
*/

#include "symtab.h"
#include "symtab_exception.h"
#include <sstream>


	symtab::symtab(){
	}
		
	symtab::~symtab(){
		
	}
		
	void symtab::add_symbol(string symbol, string address, unsigned int line_number){
		string temp = symbol.substr(0, 8);
		if(symbol_exists(temp))
			error_msg(line_number, symbol);
		
		sym_table[temp] = address;
	}
	
	string symtab::get_address(string symbol){
		if(sym_table.find(symbol) == sym_table.end())
			return "";
		else
			return sym_table[symbol];
		
	}
	
	bool symtab::symbol_exists(string symbol){
		if(sym_table.find(symbol) == sym_table.end())
			return false;
		return true;
	}
	
	void symtab::error_msg(unsigned int line_num, string symbol){
        	stringstream ss;
		ss << line_num;
		string line = ss.str();
		throw symtab_exception("Symbol: " + symbol + " on line: " + line + " has already been declared.");
	}
