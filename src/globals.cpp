
#include <Arduino.h>
//#include <iostream>
#include <string.h>
#include <EEPROM.h>
#include "Cmd.h"
#include "globals.h"


global_t globals[] = {
    //name  value, min, max, updated 
    //min = max  = read only     
    {(char*)"xx",(globaldata_t)  0, (globaldata_t)0,(globaldata_t) 3, true},  // frequency in Hz of the synthesized sine wave 1
    {(char*)"khz", (globaldata_t)8,(globaldata_t) 7, (globaldata_t) 18000, true}  // frequency in Hz of the synthesized sine wave 2  
};

int MAX_GLOBALS = sizeof(globals)/sizeof(global_t);


const int ee_size = MAX_GLOBALS*sizeof(int)+2;

int index(String arg){
  int index;
  bool ok = false;
     
  for(index = 0; index < MAX_GLOBALS; index++){
    if( strcmp(arg.c_str() , globals[index].name) == 0){
      ok = true;
      break;
    }
  } 
  int retval = -1;
  if(ok) retval =index; 
  return retval;
}


void globals_from_eeprom(void){
  for(int i=0; i< MAX_GLOBALS; i++){
    if(globals[i].min_val != globals[i].max_val ){
      EEPROM.get(i*sizeof( globals[i].value), globals[i].value);
      globals[i].updated = true;
    }
  }
  cmdGetStream()->println("Ok");    
}

void globals_to_eeprom(void){ 
  for(int i=0; i< MAX_GLOBALS; i++){
    if(globals[i].min_val != globals[i].max_val ){
      EEPROM.put(i*sizeof( globals[i].value), globals[i].value);
    }
  }
  cmdGetStream()->println("Ok"); 
}


/* Historical .... 
   (int argc, char** argv) are the standard parameters of main()
   argc  = argument count — the number of command-line arguments passed to the program
   argv  = argument vector — an array of C strings (char pointers), each holding one argument
   argv[0] usually contains the program name, and argv[1]..argv[argc-1] contain the user arguments
  
   The names 'argc' and 'argv' are just conventional variable names;
   they can be renamed (e.g., to 'count' and 'args') without changing the meaning.
   These parameters can also be passed to other functions like normal variables.

*/

void _list_gloabals(int argc, char **argv){
// The parameters (int argc, char** argv) allow a C program to receive
// command-line arguments. 'argc' holds the number of arguments,
// and 'argv' is an array of strings representing each argument.


  for(int i=0; i< MAX_GLOBALS; i++){
      cmdGetStream()->print(globals[i].name);
      cmdGetStream()->print(", ");
      cmdGetStream()->print( globals[i].value);
      cmdGetStream()->print(", ");
      cmdGetStream()->print( globals[i].min_val);
      cmdGetStream()->print(", ");
      cmdGetStream()->print( globals[i].max_val);
      cmdGetStream()->print(", ");
      cmdGetStream()->println( globals[i].updated);   

  }
}

float parse_float(int nargs, char **args){
//These parameters can also be passed to other functions like normal variables.
    float ret_value; 
  
    if((nargs > 0)){
      float  tmparg1 = (float)cmdStr2Num(args[0], 10);
      int  tmparg2 = 0;
      if (nargs > 1){ 
        tmparg2 = cmdStr2Num(args[1], 10);
      }
      ret_value =  tmparg1*pow(10.0,tmparg2);
    }
    return ret_value;
  }

void _glob_reg(int nargs, char **args){
  int iter;
  iter = index(args[0]);                // args[0] = name of command 
  if((nargs > 1) && (iter < MAX_GLOBALS)){
    globaldata_t tmp = (globaldata_t)(parse_float(--nargs, ++args));
    if(tmp < globals[iter].min_val) tmp = globals[iter].min_val;
    if(tmp > globals[iter].max_val) tmp = globals[iter].max_val;
    if(globals[iter].min_val != globals[iter].max_val ){ //if not read only
         globals[iter].value = tmp; 
         globals[iter].updated = true;
    } 
  }
  cmdGetStream()->println(globals[iter].value);
}

void addGlobals(void){   // Adds commands for reading and writing of all global variables
  cmdGetStream()->println("Adding globals...");
  for(int index = 0; index < MAX_GLOBALS; index++){
    cmdAdd(globals[index].name,_glob_reg);
  }
}

void setup_globals(void){
   addGlobals(); // Adds individual commands for all global registers
   cmdAdd("globs", _list_gloabals);// Lists a table with all globals 
                                   // format: value, min, max, updated  
  // Arduino dipended 
   cmdAdd("globs_to_ee", [](int argcnt, char**){globals_to_eeprom();});// save globals in ee
   cmdAdd("globs_from_ee", [](int argcnt, char**){globals_from_eeprom();});// read globals from ee
} 