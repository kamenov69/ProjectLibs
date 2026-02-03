#include <Arduino.h>
#include "board.h"
#include "globalVars.h"

void setup() {
    setup_board();
    
    add_new_global_var("glo",1,10,10);
    
    setup_var_list_cmd();
}

void loop() {
   loop_board();
}

 
