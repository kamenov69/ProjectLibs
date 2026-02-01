#include <Arduino.h>
#include "board.h"

#include "clocksynt.h"
#include "VarListGlobals.h"

void setup() {
    setup_board();
    setup_SI5351_clck_synt();
    
    vars.add("glob",1,-10,10);
    vars.add("glob2",1,1,100);
    vars.add("glob3", 0,1,2);
    
    add_vars_commands();
}

void loop() {
   loop_board();
   loop_clocksynt();

}

 
