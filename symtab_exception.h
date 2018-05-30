#ifndef SYMTAB_EXCEPTION_H
#define SYMTAB_EXCEPTION_H

using namespace std;

class symtab_exception {

public:
   symtab_exception(string s) {
        message = s;        
        }
        
    symtab_exception() {
        message = "An error has occurred";
        }
        
    string getMessage() {
        return message;
    }
    
private:
    string message;
};    
#endif

