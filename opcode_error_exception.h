#ifndef OPCODE_ERROR_EXCEPTION_H
#define OPCODE_ERROR_EXCEPTION_H

using namespace std;

class opcode_error_exception {

public:
   opcode_error_exception(string s) {
        message = s;        
        }
        
    opcode_error_exception() {
        message = "An Opcode error has occurred";
        }
        
    string getMessage() {
        return message;
    }
    
private:
    string message;
};    
#endif

