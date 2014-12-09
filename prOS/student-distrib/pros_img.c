/**
  * pros_img c 
  * Nov  8, 2014
  * imaging for the booting image
  */

#include "pros_img.h"
#include "lib.h"
#include "terminal.h"
#include "clock.h"

void booting_img(){

/* char array contains the booting image */
char pro_img[2000] = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                sssssssssssssssssssssss     sssssssss         ssss                             sssssssssssssssssssssssss  ssssssssssss     sssssssss                           sssssssssssssssssssssssss ssssssssssssss  sssssssssss                           sssss  ssssssssss  sssss sssssssssssssss ssssssssssss                           sssss  ssssssssss ssssss sssssss    ssssssssss                                  ssssssssss  ssssssssss  sssssss     sssssssssssssssss                           sssssssss   ssssssssss  ssssss       ssssssssssssssssss                         sssssss     sssssssssss ssssss      sssssssssssssssssss                         sssss      sssssssssssssssssss     sssss  s      sssss                          sssss      sssssssssssssssssss     sssss  s      sssss                         ssssss      sssss  ssssssssssssssssssss    sssssssssss                          sssss       sssss   sssss ssssssssssss    ssssssssss                            sssss      ssssss   ssss    sssssssss      ssssssss                                                                                                                                                                                                                                                                                          loading                                                                                                                                                                                                              ";

	clear();
	/* filling the booting image buffer */
	int temp = 0;

	terminal_write(magic_fd, pro_img, 2000);
	delay(2);
//	while (temp <= 100000000){
//		temp++;
//	}
//	temp = 0;
//	pro_img[1795] = 46;
//	clear();
//	terminal_write(magic_fd, pro_img, 2000);
//	while (temp <= 100000000){
//		temp++;
//	}
//	pro_img[1796] = 46;
//	temp = 0;
//	clear();
//	terminal_write(magic_fd, pro_img, 2000);
//	while (temp <= 100000000){
//		temp++;
//	}
//		pro_img[1797] = 46;
//		temp = 0;
//		clear();
//		terminal_write(magic_fd, pro_img, 2000);
//	while (temp <= 100000000){
//		temp++;
//	}
//		pro_img[1798] = 46;
//		temp = 0;
//		clear();
//		terminal_write(magic_fd, pro_img, 2000);
//	while (temp <= 100000000){
//		temp++;
//	}
//		pro_img[1795] = 32;
//		pro_img[1796] = 32;
//		pro_img[1797] = 32;
//		pro_img[1798] = 32;
//			temp = 0;
//			clear();
//			terminal_write(magic_fd, pro_img, 2000);

}

