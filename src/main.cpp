#include <Arduino.h>
#include "board.h"

#include "clocksynt.h"
#include "globals.h"

void setup() {
    setup_board();
    setup_SI5351_clck_synt();
    setup_globals();
}

void loop() {
   loop_board();
   loop_clocksynt();
}

 
