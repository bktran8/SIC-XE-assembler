#ifndef OPCODETAB_H
#define OPCODETAB_H

#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include "opcode_error_exception.h"

using namespace std;

	const string keyVal[] = {	"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF",
                         	"DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA", 
                         	"LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF", 
                         	"MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK", 
                         	"STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX", 
                         	"SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD"};
                         	
        const int sizeArr[] = {	3,3,2,3,2,3,3,2,3,3,
				            2,1,1,1,3,3,3,3,3,3,
                        	3,3,3,3,3,3,3,3,3,3,
                        	2,1,3,3,2,3,2,2,1,3,
                        	3,3,3,3,3,3,3,3,3,3,
                        	3,3,2,2,3,1,3,2,3 };
                        	
        const string machine[] = {"18","58","90","40","B4","28","88","A0","24","64",
                   		"9C","C4","C0","F4","3C","30","34","38","48","00",
                   		"68","50","70","08","6C","74","04","D0","20","60",
                   		"98","C8","44","D8","AC","4C","A4","A8","F0","EC",
                   		"0C","78","54","80","D4","14","7C","E8","84","10",
                   		"1C","5C","94","B0","E0","F8","2C","B8","DC"};

class opcodetab {
    public:
        // ctor
        // creates a new dictionary structure and loads all of the opcodes for 
        // the SIC/XE architecture into the table.  Use the STL
        // map for this.
        opcodetab(); 
        
        // takes a SIC/XE opcode and returns the machine code 
        // equivalent as a two byte string in hexadecimal.
        // Example:  get_machine_code("ADD") returns the value 18
        // Note that opcodes may be prepended with a '+'.
        // throws an opcode_error_exception if the opcode is not 
        // found in the table.
        string get_machine_code(string);  
        
        // takes a SIC/XE opcode and returns the number of bytes 
        // needed to encode the instruction, which is an int in
        // the range 1..4.
        // NOTE: the opcode must be prepended with a '+' for format 4.
        // throws an opcode_error_exception if the opcode is not 
        // found in the table.        
        int get_instruction_size(string);
        
        
                        
        
    private:
        map<string, pair<int, string> > optab;
        static const int numCodes = 59;
        map<string, pair<int, string> >::iterator iter;
        bool is_form_four;
        string form_check(string);
        string case_change(string);
        

};

#endif    

