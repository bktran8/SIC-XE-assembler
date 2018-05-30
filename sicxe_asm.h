/* Group: Iowa
Nicholas Gale
Neda Heydari
Hosna Paiam
Michael Totten
Brenda Tran
Vaso Vasich
*/

#ifndef SICXE_ASM_H
#define SICXE_ASM_H

#include "file_parser.h"
#include "symtab.h"
#include "opcodetab.h"
#include "file_parse_exception.h"
#include "opcode_error_exception.h"
#include "symtab_exception.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstdio>
#include <math.h>

using namespace std;


class sicxe_asm{
	struct list_row{
    		string line_num;
    		string address;
    		string label;
    		string opcode;
    		string operand;
    		string machine_code;
	};
	

	public: 
		sicxe_asm(string);
		~sicxe_asm();
        	void first_pass(string);
        	void second_pass(ostream &, string, string);

	private: 
		vector<list_row> lis;
		string file_name;
		file_parser* getTokens;
		symtab label_table;
		opcodetab opcode_table;
		map<string, int> directives;
		map<string,string> registers;
		symtab equ_table;
		
		////////Helper Functions/////////
		////Testers
		bool is_number(string);
		int byte_mult(string, int, string);
		bool is_hex_number(string);
		
		////Modifiers
        	string case_change(string);
        	string int_to_hex(int, int);
        	string int_to_string(int);
        	int string_to_int(string);
        	int hex_to_int(string);
        	string string_format(string, int);
        	
        	////Printing
        	void center_file_output(string, ofstream &);
        	void print_header(string, ofstream &);
        	void print_line(string [], ofstream &, int, int, bool);
        	void second_pass_print(ostream &);
        	void add_to_table(int, int, string, string, string);
        	
        	int sicxe_error(string, unsigned int, string);
        	void get_asm_line(file_parser *, string *, unsigned int);
        	string format_two(string, string, int, string);
        	string format_three(string, string, string, string, int, string);
        	string format_four(string, string, int, string);
        	void set_flags(string);
        	string get_ascii(string);
        	int get_offset(string, int, string, string);
        	string get_address(string, int, string);
};















#endif

