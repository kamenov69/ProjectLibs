#include <Arduino.h>
#include "board.h"

#include "clocksynt.h"
#include "globalVars.h"

void setup() {
    setup_board();
    setup_SI5351_clck_synt();
    
    add_new_global_var("glo",1,10,10);
    
    setup_var_list_cmd();
}

void loop() {
   loop_board();
   loop_clocksynt();

}

 
