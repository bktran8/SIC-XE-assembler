#include "opcodetab.h"


using namespace std;
	opcodetab::opcodetab(){
		for (unsigned int i = 0; i < numCodes; ++i) {
        	    optab[keyVal[i]] = pair<int, string>(sizeArr[i], machine[i]);
    		}
    	}
        string opcodetab::get_machine_code(string key){
        	string temp = form_check(key);
        	
        	temp = case_change(temp);
		iter = optab.find(temp);
        	
        	if(iter == optab.end())
        		throw opcode_error_exception("Error: " + key + " is not a valid opcode.");
        	else
        		return iter->second.second;
    	} 
             

	int opcodetab::get_instruction_size(string key){
		string temp = form_check(key);
        	
        	temp = case_change(temp);
		iter = optab.find(temp);
		
		if(is_form_four == true)
			if(iter->second.first == 3)
				if(temp == "RSUB")
					throw opcode_error_exception("Error: " + key + " is not the proper format.");
				else
					return iter->second.first + 1;
			else
				throw opcode_error_exception("Error: " + key + " is not the proper format.");
		else
			if(iter == optab.end())
				throw opcode_error_exception("Error: " + key + " is not a valid opcode."); 
			else
				return iter->second.first;
	}
    
	string opcodetab::case_change(string token){
    		string temp;
    		for(int i = 0; i < token.length(); i++)
        		temp += toupper(token[i]);
        	return temp;
    	}           

	string opcodetab::form_check(string token){
		is_form_four = false;
		if(token[0] == '+'){
			token.erase(0, 1);
			is_form_four = true;
			return token;
		}
		
		else
			return token;
			
	}
    	

