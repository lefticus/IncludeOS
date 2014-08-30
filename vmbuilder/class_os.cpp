#include <os>
#include <malloc.h>
#include <stdio.h>
#include <assert.h>



//C++ stuff
void* operator new(size_t size){
  return malloc(size);
}


// A private class to handle IRQ
#include "class_irq_handler.h"


//char huge_array[200]; //{'!'}; Initialize it, puts all the data into the binary.

class global_test
{
public:
  static int calls;
  global_test(){
    calls++;
      test_print_result("Global constructor 1 is called",calls);
    if(calls>=3)
      test_print_result("Global constructor 1 should be called 3 times",calls==3);
      
  }
}globtest1, globtest3,globtest4;

int global_test::calls=0;

//TODO
class global_test2{
 public:
  global_test2(){
    test_print_result("Global constructor 2 is called once",true);
  }  
}globtest2;



extern char _end;
extern int _includeos;

static int stat1=0;
static int stat2=7777;
int glob1=0;
int glob2=8888;

void OS::start(){  

  rsprint(">>> OS class started\n");

  //char buf[100];
  test_print_hdr("Global variables");
  //sprintf(buf,"(Statics: stat1 %i stat2 %i local-static: %i )\n",stat1,stat2,global_test::calls);
  //rsprint(buf);
  test_print_result("Static variables have been properly initialized",stat1==0 && stat2==7777);
  test_print_result("Global variables have been properly initialized",glob1==0 && glob2==8888);  
  //sprintf(buf,"(Globals: glob1 %i glob2 %i )\n",glob1,glob2);
  //rsprint(buf);


  
#ifdef TESTS_H
  test_malloc();
  test_new();
#endif
  //IRQ_handler::set_IDT();        
  //Service::start();
  
  halt();
};

void OS::halt(){
  rsprint("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  rsprint(">>> System halting - OK. Done.\n");
  __asm__ volatile("hlt; jmp _start;");
}

int OS::rsprint(const char* str){
  char* ptr=(char*)str;
  while(*ptr)
    rswrite(*(ptr++));  
  return ptr-str;
}


/* STEAL: Read byte from I/O address space */
uint8_t OS::inb(int port) {  
  int ret;

  __asm__ volatile ("xorl %eax,%eax");
  __asm__ volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

  return ret;
}


/* STEAL: Write byte to I/O address space */
void OS::outb(int port, uint8_t data) {
  __asm__ volatile ("outb %%al,%%dx"::"a" (data), "d"(port));
}



/* 
 * STEAL: Print to serial port 0x3F8
 */
int OS::rswrite(char c) {
  /* Wait for the previous character to be sent */
  while ((inb(0x3FD) & 0x20) != 0x20);

  /* Send the character */
  outb(0x3F8, c);

  return 1;
}



const char* OS::boot_msg="[PASS]\t Include OS successfully booted (32-bit protected mode)\n"; 

