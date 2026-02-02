#include <Arduino.h>
#include "Cmd.h"
#include "globalVars.h"



Vars globals;



float parse_float(int nargs, char **args){
//These parameters can also be passed to other functions like normal variables.
    float ret_value; 
  
    if((nargs > 0)){
      float  tmparg1 = (float)cmdStr2Num(args[0], 10);
      int  tmparg2 = 0;
      if (nargs > 1){ 
        tmparg2 =  cmdStr2Num(args[1], 10);
      }
      ret_value =  tmparg1*pow(10.0,tmparg2);
    }
    return ret_value;
  }

void _vars_update(int argc, char **args){
    char n[16];
    strcpy_P(n, args[0]);
    if(argc > 1){
       VARS_TYPE_ tmp = (VARS_TYPE_)(parse_float(--argc, ++args));
       globals.set(n, tmp);
    }
    cmdGetStream()->println(globals.get(n));
    
}


void _vars_list(int argc, char **args){
   for (auto* n = globals.head(); n != nullptr; n = n->next) {
       Stream *s = cmdGetStream();
       //s->print(F("name="));
       char buf[16];
       strcpy_P(buf, n->name_P);
       s->print(buf);
       s->print(F(", "));
       s->print(n->value);
       s->print(F(", "));
       s->print(n->minVal);
       s->print(F(", "));
       s->print(n->maxVal);
       s->print(F(", "));
       s->println(n->updated);
    } 


}

void add_new_global_var(const char * name, VARS_TYPE_ value, VARS_TYPE_ min, VARS_TYPE_ max){
    globals.add(name, value, min, max);
    cmdAdd(name, _vars_update);

}


void setup_var_list_cmd(){cmdAdd("varlst", _vars_list);}


