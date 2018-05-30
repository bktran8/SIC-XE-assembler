/* Group: Iowa
Nicholas Gale
Neda Heydari
Hosna Paiam
Michael Totten
Brenda Tran
Vaso Vasich
*/

#include "sicxe_asm.h"
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>


sicxe_asm::sicxe_asm(string file_name){
	
	getTokens = new file_parser(file_name);
	
	try{	
		getTokens->read_file();
	}
	catch(file_parse_exception& e){
		cout << e.getMessage() << endl;
		exit(0);
	}
	//Nick+Vaso added registers
	registers["A"] = "0";
	registers["X"] = "1";
	registers["L"] = "2";
	registers["B"] = "3";
	registers["S"] = "4";
	registers["T"] = "5";
	registers["PC"] = "8";
	registers["SW"] = "9";
	
	directives.insert(pair<string, int>("START", 0));
	directives.insert(pair<string, int>("END", 0));
	directives.insert(pair<string, int>("BYTE", 1));
	directives.insert(pair<string, int>("WORD", 3));
	directives.insert(pair<string, int>("RESB", 1));
	directives.insert(pair<string, int>("RESW", 3));
	directives.insert(pair<string, int>("BASE", 0));
	directives.insert(pair<string, int>("NOBASE", 0));
	directives.insert(pair<string, int>("EQU", 0));
	
	first_pass(file_name);
	
}

sicxe_asm::~sicxe_asm(){}

void sicxe_asm::first_pass(string file_name){
	string line_contents[3];
	int LOC_CTR = 0;
	int next_loc_ctr = 0;
	int START_LOC = 0;
	int line_count = 0;
	string PROG_NAME;
	string BASE = "";
	bool start_found = false;
	ofstream output_file;
	string list_name = file_name.substr(0, file_name.length() - 4) + ".lis";
	string temp_counter;
	string temp_word;


	output_file.open(list_name.c_str());
	

	for(unsigned int i = 0; i < getTokens->size() && start_found == false; i++){
		get_asm_line(getTokens, line_contents, i);
		
		if(line_contents[1] == "START"){
			if(line_contents[0] == "" && line_contents[2] == "")
				sicxe_error("There must be a label and an operand.", i + 1, list_name);
			START_LOC = i;
			line_count++;
			PROG_NAME = line_contents[0];
			add_to_table(line_count, LOC_CTR, line_contents[0], line_contents[1], line_contents[2]);
			start_found = true;
		}
		
		else if(line_contents[0] != "" || line_contents[1] != "" || line_contents[2] != ""){
			sicxe_error("There can be no labels, opcodes, or operands prior to \"start\"", i + 1, list_name);
		}
		else if(i == getTokens->size() - 1)
			sicxe_error("No program start was found.", i + 1, list_name);
		else
			line_count++;		
	}
	
	if(!is_number(line_contents[2]))
		sicxe_error("Invalid starting address", START_LOC + 1, list_name);
	
	
	if(line_contents[2][0] == '$'){
		temp_counter = line_contents[2].erase(0,1);
		next_loc_ctr = hex_to_int(temp_counter);
	}
	
	else
		next_loc_ctr = string_to_int(line_contents[2]);
	
	LOC_CTR = next_loc_ctr;
	
	for(unsigned int i = START_LOC+1; i < getTokens->size(); i++){
		get_asm_line(getTokens, line_contents, i);
		
		if(line_contents[1] == "END"){
			for(int h = i + 1; h < getTokens->size(); h++){
				get_asm_line(getTokens, line_contents, i);
				
				if(line_contents[0] != "" || line_contents[1] != "" || line_contents[2] != "")
					sicxe_error("No labels, opcodes, or operands allowed after \"end\".", h + 1, list_name);	
			}
		}

		if(directives.find(line_contents[1]) != directives.end()){
			if(line_contents[1] == "EQU"){
				if(line_contents[0] == "" || line_contents[2] == "")
					sicxe_error("Invalid label or operand.", i + 1, list_name);
				
				if(label_table.symbol_exists(line_contents[0]))
					sicxe_error("Symbol already exists.", i + 1, list_name);
					
				if(is_number(line_contents[2]))
					equ_table.add_symbol(line_contents[0], int_to_hex(string_to_int(line_contents[2]), 5), i);
				else
					equ_table.add_symbol(line_contents[0], line_contents[2], i);
					
				label_table.add_symbol(line_contents[0], int_to_hex(LOC_CTR, 5), i);
			}
			
			else{			
				if(line_contents[0] != ""){
					if(label_table.symbol_exists(line_contents[0]))
						sicxe_error("Symbol already exists.", i + 1, list_name);
					
					label_table.add_symbol(line_contents[0], int_to_hex(LOC_CTR, 5), i);
				}
			}
			
			if(line_contents[1] == "BASE")
				BASE = line_contents[2];
			
			if(line_contents[1] == "NOBASE")
				BASE = "";
			
			if(line_contents[1] == "WORD"){
				if(is_hex_number(line_contents[2])){
					if(line_contents[2][0] == 'X' || line_contents[2][0] == 'x'){
						if((line_contents[2].length() - 3) < 7)
							next_loc_ctr += 3;
						else
							sicxe_error("Operand must be less than 7 characters.", i + 1, list_name);
					}
					
					else
						next_loc_ctr += 3;
				}
				else
					sicxe_error("Operand must be a number.", i + 1, list_name);
			}
			
			if(line_contents[1] == "BYTE")
				next_loc_ctr += byte_mult(line_contents[2], i, list_name);
			
			if(line_contents[1] == "RESW"){
				if(is_number(line_contents[2]))
					next_loc_ctr += (3 * atoi(line_contents[2].c_str()));
				else
					sicxe_error("Operand must be a number.", i + 1, list_name);
			}
			
			if(line_contents[1] == "RESB")
				if(is_number(line_contents[2]))
					next_loc_ctr += atoi(line_contents[2].c_str());
				else
					sicxe_error("Operand must be a number.", i + 1, list_name);
		}
		
		else{
			if(line_contents[0] != ""){
				if(label_table.symbol_exists(line_contents[0]))
					sicxe_error("Symbol already exists.", i + 1, list_name);
				
				else
					label_table.add_symbol(line_contents[0], int_to_hex(next_loc_ctr, 5), i);
			}

			if(line_contents[1] == "")
				continue;
			
			else
				next_loc_ctr += opcode_table.get_instruction_size(line_contents[1]);	
		}
		line_count++;
		add_to_table(line_count, LOC_CTR, line_contents[0], line_contents[1], line_contents[2]);

		LOC_CTR = next_loc_ctr;
		
	}
		
	second_pass(output_file, list_name, BASE);
	print_header(list_name, output_file);
	second_pass_print(output_file);
	output_file.close();
}

//Now we need to check all operands for '#', '@', and '$'. 'X' and 'C' are already handled for BYTE and the 'X' is handled for WORD
//Labels must now be checked, with their memory locations returned from the symbol table
void sicxe_asm::second_pass(ostream &output_file, string list_name, string BASE){
	int machine_code = 0;
	vector<string> equVect;
	string equTemp = "";
	
	for(unsigned int i = 0; i < lis.size(); i++){
		
		if (lis[i].opcode == "WORD"){
			if(lis[i].operand[0] == 'X'){
				lis[i].machine_code = string_format(lis[i].operand.substr(2, lis[i].operand.length() - 3), 6);
			}
			//Nick+Vaso, handle word value being hex
			else
			    if(lis[i].operand[0] == '$'){
			        lis[i].machine_code = string_format(lis[i].operand.substr(1, lis[i].operand.length() - 1), 6);
			    }
			    else{
				    lis[i].machine_code = int_to_hex(string_to_int(lis[i].operand), 6);
			    }
		}//Nick+Vaso, not sure if the int_to_hex needs to be cast for the ascii numbers
		else if(lis[i].opcode == "BYTE"){
		    if(lis[i].operand[0] == 'C' || lis[i].operand[0] == 'c')
		        lis[i].machine_code = get_ascii(lis[i].operand.substr(2, lis[i].operand.length() - 3));		//what if its EO instead of EOF etc etc
		    
		    else if(lis[i].operand[0] == 'X' || lis[i].operand[0] == 'x')
	            	lis[i].machine_code = lis[i].operand.substr(2, lis[i].operand.length() - 3);
	        }
			
		else if(directives.find(lis[i].opcode) != directives.end())
			continue;

		else if(opcode_table.get_instruction_size(lis[i].opcode) == 1)
			lis[i].machine_code = opcode_table.get_machine_code(lis[i].opcode);
		
		else if(opcode_table.get_instruction_size(lis[i].opcode) == 2)
			lis[i].machine_code = format_two(lis[i].opcode, lis[i].operand, string_to_int(lis[i].line_num), list_name);
			
		else if(opcode_table.get_instruction_size(lis[i].opcode) == 3)
			lis[i].machine_code = format_three(lis[i].opcode, lis[i].operand, BASE, lis[i].address, string_to_int(lis[i].line_num), list_name);
		
		else if(opcode_table.get_instruction_size(lis[i].opcode) == 4)
			lis[i].machine_code = format_four(lis[i].opcode, lis[i].operand, string_to_int(lis[i].line_num), list_name);
	}
}

///////////////////////Begin Helper Functions
///////////Testers
bool sicxe_asm::is_number(string token){
	for(unsigned int i = 0; i < token.length(); i++){
		if(i == 0 && token[i] == '$')
			i++;
		if(!isdigit(token[i]))
			return false;	
	}
	return true;
}

bool sicxe_asm::is_hex_number(string token){
	string temp;
	if(token[0] == 'X' || token[0] == 'x')
		temp = token.substr(2, token.length() - 3);
	else
		temp = token;
	for(unsigned int i = 0; i < temp.length(); i++){
		if(i == 0 && temp[i] == '$')
			i++;
		if(!isxdigit(temp[i]))
			return false;
	}
	return true;
}

int sicxe_asm::byte_mult(string token, int i, string list_name){
	if(token[0] == 'C' || token[0] == 'c')
		return (token.length() - 3);
		
	else if(token[0] == 'X' || token[0] == 'x'){
		if(((token.length() - 3) & 1) == 1)
			sicxe_error("Invalid character count.", i + 1, list_name);
		else if(!is_hex_number(token))
			sicxe_error("Value is not a hexadecimal number.", i + 1, list_name);
		return ((token.length() - 3) >> 1);
	}				
	else
		sicxe_error("Invalid byte operand.", i + 1, list_name);
		
	return -1;
}

///////////Modifiers
string sicxe_asm::case_change(string token){
    	string temp;
    	for(unsigned int i = 0; i < token.length(); i++)
        	temp += toupper(token[i]);
        return temp;
}  

string sicxe_asm::int_to_hex(int num, int width){
	stringstream out;
	out << setw(width) << setfill('0') << hex << num;
	return case_change(out.str());
}

string sicxe_asm::get_ascii(string num){
	stringstream out;
	
	for(int i = 0; i < num.length(); i++)
		out << hex << (int)num[i];
		
	return case_change(out.str());
}

string sicxe_asm::int_to_string(int x){
    	stringstream ss;
	ss << x;
	string err = ss.str();
	return err;
}

int sicxe_asm::hex_to_int(string number){
	int value;
	if(number[0] == '$')
		number = number.erase(0,1);
	
	sscanf(number.c_str(), "%x", &value);
	return value;
}

int sicxe_asm::string_to_int(string token){
	int value = 0;
	sscanf(token.c_str(), "%d", &value);
	return value;
}

string sicxe_asm::string_format(string token, int width){
	stringstream temp;
	
	temp << setw(width) << setfill('0') << token;
	
	return temp.str();
}

///////////Printing
void sicxe_asm::center_file_output(string s, ofstream &file){
	for(int i = 0; i < (80 - s.length()) >> 1; i++)
		file << " ";
	file << "**" << s  << "**" << endl;
}

void sicxe_asm::print_header(string prog_name, ofstream &file){
	string col_labels[] = {"Line #", "Address", "Label", "Opcode", "Operand", "Machine Code"};
	string format_lines[] = {"======", "=======", "=====", "======", "=======", "============"};
	
	center_file_output(prog_name, file);
	
	for(int k = 0; k < 6; k++)
		file << setw(15) << setfill(' ') << left << col_labels[k];
		
	file << endl;
		
	for(int k = 0; k < 6; k++)
		file << setw(15) << setfill(' ') << left <<  format_lines[k];
		
	file << endl;
}

void sicxe_asm::print_line(string line_arr[3], ofstream &file, int lines, int LOC_CTR, bool start_found){
	if(!start_found){	
		for(int h = 0; h < lines - 1; h++){
			file << setw(15) << setfill(' ') << h + 1;
			file << setw(15) << setfill(' ') << left << int_to_hex(LOC_CTR, 5) << endl;
		}
	}
	file << setw(15) << setfill(' ') << lines;
	file << setw(15) << setfill(' ') << int_to_hex(LOC_CTR, 5);
	file << setw(15) << setfill(' ') << line_arr[0];
	file << setw(15) << setfill(' ') << line_arr[1];
	file << setw(15) << setfill(' ') << left << line_arr[2] << endl;
}

void sicxe_asm::add_to_table(int line_num, int address, string label, string opcode, string operand){
	list_row temp;
	
	temp.line_num = int_to_string(line_num);
	temp.address = int_to_hex(address, 5);
	temp.label = label;
	temp.opcode = opcode;
	temp.operand = operand;
	temp.machine_code = "";
	
	lis.push_back(temp);
}

void sicxe_asm::second_pass_print(ostream &file){
	for(int i = 0; i < lis.size(); i++){
		file << setw(15) << setfill(' ') << lis[i].line_num;
		file << setw(15) << setfill(' ') << lis[i].address;
		file << setw(15) << setfill(' ') << lis[i].label;
		file << setw(15) << setfill(' ') << lis[i].opcode;
		file << setw(15) << setfill(' ') << lis[i].operand;
		file << setw(15) << setfill(' ') << left << lis[i].machine_code << endl;
	}
}

int sicxe_asm::get_offset(string temp_lbl, int line, string list_name, string source){
	int offset = 0;
	
	if(is_number(temp_lbl)){
		offset = string_to_int(temp_lbl);
		if(offset >= 0 && offset < 4095)
			return offset;
		else
			sicxe_error("Value is greater that 4095 or less than 0.", line, list_name);
	}
	else if(is_hex_number(temp_lbl))
		offset = hex_to_int(temp_lbl.substr(temp_lbl[1], temp_lbl.length())) - (hex_to_int(source) + 3);
		
	else if(label_table.symbol_exists(temp_lbl)){
		offset = hex_to_int(label_table.get_address(temp_lbl)) - (hex_to_int(source) + 3);
	}
		
	else
		sicxe_error("Invalid operand.", line, list_name);
		
	return offset;
}

string sicxe_asm::get_address(string temp_lbl, int line, string list_name){
	string hex_off;
	
	if(is_number(temp_lbl))
		hex_off = int_to_hex(string_to_int(temp_lbl), 5);
		
	else if(is_hex_number(temp_lbl))
		hex_off = temp_lbl.substr(temp_lbl[1], temp_lbl.length() - 1);
		
	else if(label_table.symbol_exists(temp_lbl))
		hex_off = label_table.get_address(temp_lbl);
		
	else
		sicxe_error("Invalid operand.", line, list_name);
		
	return hex_off;
}

///////////Format-Specific Machine Encoders

string sicxe_asm::format_two(string opcode, string operand, int line, string list_name){
    int machine_code = 0;
    int comma = operand.find_first_of(",");
    string r1 = operand.substr(0, comma);
    string r2 = "";
    string temp = "";
    map<string, string>::iterator it;
    
    if(comma != -1)
    	r2 = operand.substr(comma+1, operand.length() - r1.length()+1);
    else
    	r2 = "1";

    if(opcode == "TIXR"){
    	temp += opcode_table.get_machine_code(opcode);
    	temp += registers[operand];
    	temp += "0";
    }
    
    else if(opcode == "SVC"){
    	temp += opcode_table.get_machine_code(opcode);
    	
    	if(string_to_int(operand) > 15 || string_to_int(operand) < 1)
    		sicxe_error("Invalid SVC operand not within range of 1 - 16. ", line, list_name);
    	else{
    		temp += int_to_hex(string_to_int(operand), 1);
    		temp += "0";
    	}
    }
    
    else{
    	temp += opcode_table.get_machine_code(opcode);
    	
    	if(registers.find(r1) != registers.end())
    		temp += registers.find(r1)->second;
    	else{
    		if(string_to_int(r1) < 16 && string_to_int(r1) > 0)
    			temp += int_to_string(string_to_int(r1) - 1);
    		
    		else
    			sicxe_error("Value is greater than 15 or less than 0.", line, list_name);
    	}
    	if(registers.find(r2) != registers.end())
    		temp += registers.find(r2)->second;
    	else{
    		if(string_to_int(r2) < 16 && string_to_int(r2) >= 0)
    			temp += int_to_string(string_to_int(r2) - 1);
    		else
    			sicxe_error("Value is greater than 15 or less than 0.", line, list_name);
    	}
    }
    
    return temp;
}

string sicxe_asm::format_three(string opcode, string operand, string base, string source, int line, string list_name){
	int machine_code = 0;
	bool flags[] = {0, 0, 0, 0, 0, 0};
	int destination = 0;
	int offset = 0;
	string temp_lbl = operand;
	string hex_off = "";
	string first_op_char = opcode_table.get_machine_code(opcode).substr(0,1);
	string second_op_char = opcode_table.get_machine_code(opcode).substr(1,2);
	
	machine_code += ((hex_to_int(first_op_char)) * pow(16,5));
	
	//Set flags and remove '#' or '@'.
	if(operand[0] == '@'){
		flags[0] = 1;
		temp_lbl = operand.substr(1, operand.length());
		machine_code += ((hex_to_int(second_op_char) + 2) * pow(16,4));
		offset = get_offset(temp_lbl, line, list_name, source);
	}
	else if(operand[0] == '#'){
		flags[1] = 1;
		temp_lbl = operand.substr(1, operand.length());
		machine_code += ((hex_to_int(second_op_char) + 1) * pow(16,4));
		offset = get_offset(temp_lbl, line, list_name, source);
	}
	else{
		flags[0] = 1;
		flags[1] = 1;
		machine_code += ((hex_to_int(second_op_char) + 3) * pow(16,4));
	}
	
	
	if(flags[0] == 1 && flags[1] == 1){
		if(operand.find_first_of(",") != string::npos){
			flags[2] = 1;
			temp_lbl = operand.substr(0, operand.find_first_of(","));
			machine_code += (8 * pow(16,3));
			
			offset = get_offset(temp_lbl, line, list_name, source);
		}
		else
			offset = get_offset(temp_lbl, line, list_name, source);
	}
	
	else{
		if(operand.find_first_of(",") != string::npos)
			sicxe_error("Index not allowed with indirect or immediate modes.", line, list_name);
		
		else{
			offset = get_offset(temp_lbl, line, list_name, source);
		}
		
	}

	
	if(offset > 2047 || offset < -2048){
		if(base != "" && opcode != "LDB")
			offset = hex_to_int(label_table.get_address(temp_lbl)) - hex_to_int(label_table.get_address(base));	
		
		else
			sicxe_error("Memory out of range.", line, list_name);
	}
	
	else{
		hex_off = int_to_hex(offset, 3);
		hex_off = hex_off.substr(hex_off.length()-3,3);

		if(!is_number(temp_lbl))
			machine_code += (2 * pow(16,3));
			
		machine_code += (hex_to_int(hex_off.substr(0,1)) * pow(16,2));
		machine_code += (hex_to_int(hex_off.substr(1,1)) * 16);
		machine_code += hex_to_int(hex_off.substr(2,1));

		return int_to_hex(machine_code, 6);
	}
	
	if(offset > 4095)
		sicxe_error("Memory out of range.", line, list_name);
	else{
		hex_off = int_to_hex(offset, 3);
		hex_off = hex_off.substr(hex_off.length()-3,3);

		machine_code += (4 * pow(16,3));
		machine_code += (hex_to_int(hex_off.substr(0,1)) * pow(16,2));
		machine_code += (hex_to_int(hex_off.substr(1,1)) * 16);
		machine_code += (hex_to_int(hex_off.substr(2,1)));
	}
	
	return int_to_hex(machine_code, 6);
}


//ALMOST/PROBABLY DONE
string sicxe_asm::format_four(string opcode, string operand, int line, string list_name){
	int machine_code = 0;
	bool flags[] = {0, 0, 0, 0, 0, 0};
	int destination = 0;
	string temp_lbl = operand;
	string hex_off = "";
	string first_op_char = opcode_table.get_machine_code(opcode).substr(0,1);
	string second_op_char = opcode_table.get_machine_code(opcode).substr(1,2);
	
	machine_code += ((hex_to_int(first_op_char)) * pow(16,7));
	
	//Set "e" flag
	machine_code += pow(16,5);
	flags[5] = 1;
	
	//Set flags and remove '#' or '@'.
	if(operand[0] == '@'){
		flags[0] = 1;
		temp_lbl = operand.substr(1, operand.length() - 1);
		machine_code += ((hex_to_int(second_op_char) + 2) * pow(16,6));
		
		hex_off = get_address(temp_lbl, line, list_name);	
	}
	else if(operand[0] == '#'){
		flags[1] = 1;
		temp_lbl = operand.substr(1, operand.length() - 1);
		machine_code += ((hex_to_int(second_op_char) + 1) * pow(16,6));
		
		hex_off = get_address(temp_lbl, line, list_name);
	}
	else{
		flags[0] = 1;
		flags[1] = 1;
		machine_code += ((hex_to_int(second_op_char) + 3) * pow(16,6));
	}

	//Check for offset flag 'X'
	if(flags[0] == 1 && flags[1] == 1){
		if(operand.find_first_of(",") != string::npos){
			flags[2] = 1;
			temp_lbl = operand.substr(0, operand.find_first_of(","));
			machine_code += (8 * pow(16,5));
			
			hex_off = get_address(temp_lbl, line, list_name);
		}		
		else{
			hex_off = get_address(temp_lbl, line, list_name);
		}
	}
	
	else{
		if(operand.find_first_of(",") != string::npos)
			sicxe_error("Index not allowed with indirect or immediate modes.", line, list_name);
			
		else{
			hex_off = get_address(temp_lbl, line, list_name);
		}
	}
	
	if(equ_table.symbol_exists(temp_lbl)){
		if(is_hex_number(equ_table.get_address(temp_lbl))){
			hex_off = equ_table.get_address(temp_lbl);
		}
	}

	machine_code += (hex_to_int(hex_off.substr(0,1)) * pow(16,4));
	machine_code += (hex_to_int(hex_off.substr(1,1)) * pow(16,3));
	machine_code += (hex_to_int(hex_off.substr(2,1)) * pow(16,2));
	machine_code += (hex_to_int(hex_off.substr(3,1)) * 16);
	machine_code += hex_to_int(hex_off.substr(4,1));

	return int_to_hex(machine_code, 8);
}

//
void sicxe_asm::get_asm_line(file_parser *line_table, string *line_array, unsigned int row){
	for(int i = 0; i < 3; i++){
		if((line_table->get_token(row, 2)[0] == 'c' || line_table->get_token(row, 2)[0] == 'C') && i == 2){
			if(line_table->get_token(row, 2)[1] == '\'')
				line_array[i] = line_table->get_token(row, i);
		}
		else
			line_array[i] = case_change(line_table->get_token(row, i));
	}		
}

int sicxe_asm::sicxe_error(string s, unsigned int line, string file_name){
	 cout << "Error on line: " << int_to_string(line) << ". " << s << endl;
	 remove(file_name.c_str());
	 exit(0);
}
///////////////////////End Helper Functions


int main(int argc, char* argv[]){
	if(argc != 2){
		cout << "Error: Must provide the name of the file in the command line." << endl;
		return 0;
	}
	string name = argv[1];	
	sicxe_asm pass(name);	

	return 0;	
}








