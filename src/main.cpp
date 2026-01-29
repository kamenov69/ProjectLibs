#include <Arduino.h>
#include "board.h"

#include "clocksynt.h"


void setup() {
    setup_board();
    setup_clocksynt();

}

void loop() {
   loop_board();
   loop_clocksynt();
}

 
