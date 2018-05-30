/* Team Iowa
 * Brenda Tran, Hosna Paiam, Mike Totten, Neda Heydari, Nick Gale, Vaso Vasich
*/
#include <fstream>
#include <iostream>
#include <cstring>
#include <iomanip>

#include <string>
#include <sstream>

#include "file_parser.h"
#include "file_parse_exception.h"

using namespace std;
string int_to_string(int);

file_parser::file_parser(string name){
	file_name = name;
}

file_parser::~file_parser()
{
}

void file_parser::read_file(){
    string inLine;	
    string delimiters = " \t\n";

	    ///////Open File///////
    ifstream fileN(file_name.c_str());
    if(!fileN.good())
        throw file_parse_exception(file_name + " does not exist.");
	        
    while(getline(fileN, inLine))
        lineVector.push_back(inLine);
//Parse through each line    	
    for(unsigned int i =0; i < int(lineVector.size()); i++){
		tokenVector.push_back(line());
		string target = lineVector[i];
		int index = 0,tend = 0;
		int end = target.length();
		bool isOpcode = false,isOperand = false,isLabel = false;
		string token = "";
//Check for comment		
	    while(index < end){
	    	if(target[index] == '.'){
	    		token = target.substr(index, end-index);   //(pos,len)
	    		tokenVector[i].comment = token;
	    		index = end;
 		    }
//Check Label
		    else if(index == 0){
	    		if(isspace(target[index])){
		    		if(target.find_first_not_of(delimiters)==-1)
					    index =end;
        			else
		        		index = target.find_first_not_of(delimiters);
			    }
			    else if(isalpha(target[index])){
		    		tend = target.find_first_of(delimiters);
		    		token = target.substr(index, tend-index);
		    		string temp ="";
		    		temp = token.substr(0, 8);	//Allows for using labels max size 8
		    		tokenVector[i].label = token;
		    		isLabel = true;
		    		index = tend;
		    		if(target.find_first_not_of(delimiters, index)==-1)
			    		index = end;
				    else
				    	index = target.find_first_not_of(delimiters, index);
			    }
			    else
				    throw file_parse_exception("Incorrect Label at line " + int_to_string(++i) + ".");
		    }
//Check Opcode
		    else if(isOpcode==false){
			    if(isLabel==false)
				    index = target.find_first_not_of(delimiters, index); 
		    	tend = target.find_first_of(delimiters, index);
		    	token = target.substr(index, tend-index);
		    	tokenVector[i].opcode = token;
	    		isOpcode = true;
		    	index = tend;
			    if(target.find_first_not_of(delimiters, index) == -1)
				    index = end;
			    else
	    	    	index = target.find_first_not_of(delimiters, index);
		    }
//Check operand
		    else if(isOperand == false){
		    	if(target[index+1] == '\''){
			    	tend = target.find_last_of('\'')+1;
				    token = target.substr(index, tend-index);
		    		tokenVector[i].operand = token;
		    		index = tend;
		    		if(target.find_last_of('.')==-1){
		    			if(target.find_first_not_of(delimiters,index)==-1)
			    			index=end;
			    		else
				    		throw file_parse_exception("Incorrect operand at line " + int_to_string(++i) + ".");
                        //   This part above should test for incorrect formatted comments, IE no
				    }
				    else
				    	index = target.find_last_of('.');
				    
				    isOperand = true;
			    }
			    else{		
	    			tend = target.find_first_of(delimiters, index);
	    			token = target.substr(index, tend-index);
	    			if(token.find_first_of('\'',0)!=-1)
	    				throw file_parse_exception("Incorrect operand at line " + int_to_string(++i)+".");
	    			tokenVector[i].operand = token;
	    			index = tend;
	    			if(target.find_last_of('.')==-1){
	    				if(target.find_first_not_of(delimiters,index)==-1)
	    					index=end;
	    				else
                            throw file_parse_exception("Incorrect comment at line " + int_to_string(++i) + ".");
				    }
				    else
				    	index = target.find_last_of('.');
				    isOperand = true;
			    }
		    }else{
		    	cout << "index is " << index << ", end is " << end << ", and line is " << i << endl;
		    	throw file_parse_exception("Error unknown");
	    	}
	    } // end of while loop
   } 

}

string file_parser::get_token(unsigned int row, unsigned int col){
	if(row>lineVector.size())	
		throw file_parse_exception("Row " + int_to_string(row) + " does not exist.");
	if(col>3)
		throw file_parse_exception("Column " + int_to_string(col) + " does not exist");
	if(col == 0)
		return tokenVector[row].label;
	if(col == 1)
		return tokenVector[row].opcode;
	if(col == 2)
		return tokenVector[row].operand;
	if(col == 3)
		return tokenVector[row].comment;
        else
	    return "";   //Suppresses warning of non-void function return.
}

void file_parser::print_file(){
	for(int i =0; i<int(lineVector.size()); i++){
		if(tokenVector[i].label !="" && tokenVector[i].opcode == "")
			cout << setw(8) << left << tokenVector[i].label << endl; 
		else if(tokenVector[i].opcode == "")
			cout << left << tokenVector[i].comment << endl;
		else
		    cout << setw(8) << left << tokenVector[i].label << setw(8) << left << tokenVector[i].opcode << setw(10) << left << tokenVector[i].operand << setw(40) << left << tokenVector[i].comment << endl;
	}
}

string int_to_string(int x){
    stringstream ss;
	ss << x;
	string err = ss.str();
	return err;
}

int file_parser::size(){
     return lineVector.size();
}
