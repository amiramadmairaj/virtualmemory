#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>



// WHEN DO WE CHANGE THE DIRTY/VALID BITS??
// WE CHANGE DIRTY BIT WHEN WE WRITE TO A PAGE
// WE CHANGE VALID BIT WHEN WE READ FROM A PAGE

// HOW DO I PERFORM PAGE REPLCEMNENT? WHERE DOES CONTENT MOVE?

bool algo_type_fifo = true; // default to fifo
bool algo_type_lru = false; 
struct VirtualPage {
    int valid;
    int dirty;
    int physical_page_number;
    int content[8]; // 8 integers per page
};

struct PhysicalPage {
   int content[8];
   int page_number;
};

struct VirtualPage virt_mem[16];
struct PhysicalPage physical_mem[4];


void pageFaultHandler(){

}



void initializeMemory() {
    for (int i = 0; i < 16; i++) {
        virt_mem[i].valid = 0;
        virt_mem[i].dirty = 0;
        virt_mem[i].physical_page_number = i; //physical page number
        // initialize content (all 8 spots) to -1
        for (int j = 0; j < 8; j++) {
            virt_mem[i].content[j] = -1;
        }
    }
    for (int i = 0; i < 4; i++) {
        physical_mem[i].page_number = i;
        // initialize content (all 8 spots) to -1
        for (int j = 0; j < 8; j++) {
            physical_mem[i].content[j] = -1;
        }
    }
}
void readMemory(int virtual_addy){
    if (virtual_addy < 0 || virtual_addy > 127) {
        printf("Invalid virtual address\n");
        return;
    }
    else{
        int page_number = virtual_addy / 8; // this is divided by 8 because there are 8 integers per page 
        int offset = virtual_addy % 8; // offset tells us which integer in the page we want (0-7)
        if (virt_mem[page_number].valid == 0) { // check if valid, valid meaning in physical memory
            printf("A Page Fault Has Occurred\n");
            printf("%d\n",virt_mem[page_number].content[offset]);
            return;
        }
        else{
            int content = virt_mem[page_number].content[offset];
            printf("%d\n",content);
            return;
        }
    }
}


void writeMemory(int virtual_addy, int data){
    int page_number = virtual_addy / 8; // this is divided by 8 because there are 8 integers per page 
    if (virt_mem[page_number].dirty) {
        printf("A Page Fault Has Occurred\n");

        return;
    }
    else{
        int desired_spot = virtual_addy % 8; // offset tells us which integer in the page we want (0-7)
        int actual_page_in_mem = page_number / 4; // this is divided by 4 because there are 4 pages in physical memory
        int actual_place_in_mem = page_number % 4; // this is mod by 4 it tells us where within the page we want to write to
        virt_mem[page_number].content[desired_spot] = data;
        virt_mem[page_number].dirty = 1;
        virt_mem[page_number].valid = 1;
        virt_mem[page_number].physical_page_number = actual_page_in_mem;
        physical_mem[actual_page_in_mem].content[actual_place_in_mem] = data;
        return;
        }
    }


void showMainMemory(int physical_page_addy){
    if (physical_page_addy < 0 || physical_page_addy > 3) {
        printf("Invalid physical page number\n");
        return;
    }
    int numbering = physical_page_addy * 8;
    for (int j = 0; j < 8; j++) {
        printf("%d: %d\n",numbering + j ,physical_mem[physical_page_addy].content[j]);
    }
}

void showPageTable(){
    //virtual page number,Valid bit, Dirty bit, and Page Frame Number
    for (int i = 0; i < 16; i++) {
        printf("%d:%d:%d:%d\n",i,virt_mem[i].valid,virt_mem[i].dirty,virt_mem[i].physical_page_number);
    }
}

int main(int argc, char* arg[]) {
//Your simulator should accept EITHER “FIFO” or “LRU” as the command-line argument OR no command-line argument, which by default selects the FIFO algorithm.
    if (argc > 1 && strcmp(arg[1], "LRU") == 0) {
        algo_type_fifo = false;
        algo_type_lru = true;
    }
    printf("Welcome to the Virtual Memory Simulator!\n");
    printf("ALGO TYPE SET TO: %s\n", algo_type_fifo ? "FIFO" : "LRU");

    initializeMemory();
    while (1) {
        printf("> ");
        char command[50];
        int virtual_address, data, ppn;
        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("Error reading command\n");
        } else if  (sscanf(command, "read %d", &virtual_address) == 1) {
            readMemory(virtual_address);
        } else if (sscanf(command, "write %d %d", &virtual_address, &data) == 2) {
            writeMemory(virtual_address, data);
        } else if (sscanf(command, "showmain %d", &ppn) == 1) {
            showMainMemory(ppn);
        } else if (strcmp(command, "showptable\n") == 0) {
            showPageTable();
        } else if (strcmp(command, "quit\n") == 0) {
            exit(0);
            break;
        } 
        else {
            printf("Invalid command\n");
        }
    }
    return 0;
}
