#include <Arduino.h>
#include "VarListGlobals.h"
#include "Cmd.h"
Vars vars;



float parse_float(int nargs, char **args){
//These parameters can also be passed to other functions like normal variables.
    float ret_value; 
  
    if((nargs > 0)){
      float  tmparg1 = (float)cmdStr2Num(args[0], 10);
      Serial.print("debug parse   ");
      Serial.println(tmparg1);
      VARS_TYPE_  tmparg2 = 0;
      if (nargs > 1){ 
        tmparg2 = cmdStr2Num(args[1], 10);
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
       vars.set(n, tmp);
       vars.isUpdated(n);
    }
    VARS_TYPE_ val;
    if(vars.get(n, val)){
        cmdGetStream()->println(val);
    }
    else cmdGetStream()->println("None");
    
}


void _vars_list(int argc, char **args){
   for (auto* n = vars.head(); n != nullptr; n = n->next) {
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

void add_vars_commands(){
    cmdAdd("varlst", _vars_list);
    for (auto* n = vars.head(); n != nullptr; n = n->next) {
       char buf[16];
       strcpy_P(buf, n->name_P);
       cmdAdd(buf,_vars_update);
       
    }
}