#include "sound.h"
#include "lib.h"
#include "clock.h"
#include "types.h"



//Play sound using built in speaker
extern void play_sound(int nFrequence) {
 	int Div;
 	char tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb( 0xb6, 0x43);
 	outb((char) (Div) ,0x42);
 	outb( (char) (Div >> 8), 0x42);
 
        //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb( tmp | 3, 0x61);
 	}
 }
 
 //make it shutup
extern void nosound() {
 	char tmp = inb(0x61) & 0xFC;
 
 	outb( tmp, 0x61);
 }
 
 //Make a beep
extern void beep() {
 	 play_sound(1000);
 	 delay(10);
 	 nosound();
          //set_PIT_2(old_frequency);
 }
