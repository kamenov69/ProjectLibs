#include <Arduino.h>
#include "board.h"

#include "clocksynt.h"
#include "globalVars.h"

void setup() {
    setup_board();
    setup_SI5351_clck_synt();
    
    globals.add("glo",1,-10,10);
    
    add_vars_commands();  // globals -> Cmd
}

void loop() {
   loop_board();
   loop_clocksynt();

}

 
