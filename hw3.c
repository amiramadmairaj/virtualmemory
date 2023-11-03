#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>


// WHEN DO WE CHANGE THE DIRTY/VALID BITS??
// WE CHANGE DIRTY BIT WHEN WE WRITE TO A PAGE
// WE CHANGE VALID BIT WHEN WE READ FROM A PAGE

// HOW DO I PERFORM PAGE REPLCEMNENT? WHERE DOES CONTENT MOVE?

int number_of_pages_in_physical = -1;
int FIFO_counter = 0;
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
   int virtual_page_number;
   int times_used;
};

struct VirtualPage virt_mem[16];
struct PhysicalPage physical_mem[4];


void pageFaultHandler(int page_number){
    // if there are 4 pages in physical memory, we need to replace one
    if (number_of_pages_in_physical > 4){
        // FIFO
        if (algo_type_fifo == true){
            if (FIFO_counter > 3){ // reset counter
                FIFO_counter = 0;
            }
            if (virt_mem[physical_mem[FIFO_counter].virtual_page_number].dirty == 1){
                // copy content from main memory to virtual memory
                for (int j = 0; j < 8; j++) {
                    virt_mem[physical_mem[FIFO_counter].virtual_page_number].content[j] = physical_mem[FIFO_counter].content[j];
                }
            }
            virt_mem[physical_mem[FIFO_counter].virtual_page_number].valid = 0;
            virt_mem[physical_mem[FIFO_counter].virtual_page_number].dirty = 0;
            virt_mem[physical_mem[FIFO_counter].virtual_page_number].physical_page_number = -1;
            // set content to -1 in main memory
            for (int j = 0; j < 8; j++) {
                physical_mem[FIFO_counter].content[j] = -1;
            }
            physical_mem[FIFO_counter].virtual_page_number = page_number;
            FIFO_counter += 1;
            return;
            
        }// LRU
        else if (algo_type_lru == true){
            int least_used = 0;
            for (int i = 0; i < 4; i++) {
                if (physical_mem[i].times_used < physical_mem[least_used].times_used) {
                    least_used = i;
                }
            }
            virt_mem[physical_mem[least_used].virtual_page_number].valid = 0;
            virt_mem[physical_mem[least_used].virtual_page_number].dirty = 0;
            virt_mem[physical_mem[least_used].virtual_page_number].physical_page_number = -1;
            // if dirty bit is 1, then we need to copy the content from main memory to virtual memory
            if (virt_mem[physical_mem[least_used].virtual_page_number].dirty == 1){
               for (int j = 0; j < 8; j++) {
                virt_mem[physical_mem[least_used].virtual_page_number].content[j] = physical_mem[least_used].content[j];
                }
            }
            // set content to -1 in main memory
            for (int j = 0; j < 8; j++) {
                physical_mem[least_used].content[j] = -1;
            }
            // put page in main memory
            physical_mem[least_used].virtual_page_number = page_number;
            for (int i = 0; i < 8; i++) {
                physical_mem[least_used].content[i] = virt_mem[page_number].content[i];
            }
            virt_mem[page_number].valid = 1;
            virt_mem[page_number].dirty = 0;
            virt_mem[page_number].physical_page_number = least_used;
            physical_mem[least_used].times_used = 1;
            return;
        }
    }
    else{
        // put page in main memory
        physical_mem[number_of_pages_in_physical].virtual_page_number = page_number;
        for (int i = 0; i < 8; i++) {
            physical_mem[number_of_pages_in_physical].content[i] = virt_mem[page_number].content[i];
        }
        virt_mem[page_number].valid = 1;
        virt_mem[page_number].dirty = 0;
        virt_mem[page_number].physical_page_number = number_of_pages_in_physical;
        physical_mem[number_of_pages_in_physical].times_used = 1;
        return;
    }


}



void initializeMemory() {
    for (int i = 0; i < 16; i++) {
        virt_mem[i].valid = 0;
        virt_mem[i].dirty = 0;
        virt_mem[i].physical_page_number = -1;
        // initialize content (all 8 spots) to -1
        for (int j = 0; j < 8; j++) {
            virt_mem[i].content[j] = -1;
        }
    }
    for (int i = 0; i < 4; i++) {
        physical_mem[i].virtual_page_number = -1;
        // initialize content (all 8 spots) to -1
        for (int j = 0; j < 8; j++) {
            physical_mem[i].content[j] = -1;
        }
    }
}

void readMemory(int virtual_addy) {
    int page_number = virtual_addy / 8;
    if (virt_mem[page_number].valid == 0) { // check if valid, valid meaning in physical memory
        printf("A Page Fault Has Occurred\n");
        pageFaultHandler(page_number);
    } else {
        int physical_page_index = virt_mem[page_number].physical_page_number;
        if (physical_page_index == -1) {
            // This shouldn't happen if everything is working correctly, but it's good to check.
            printf("Error: Valid page not found in physical memory.\n");
            return;
        }
        int offset_in_page = virtual_addy % 8; // offset within the page
        printf("The value is %d\n", physical_mem[physical_page_index].content[offset_in_page]);
        physical_mem[physical_page_index].times_used += 1; // Only for LRU, not needed for FIFO
    }
}



void writeMemory(int virtual_addy, int data){
    int page_number = virtual_addy / 8; // this is divided by 8 because there are 8 integers per page 
    if (virt_mem[page_number].valid == 0) { // check if valid, valid meaning in physical memory
        printf("A Page Fault Has Occurred\n");
        number_of_pages_in_physical += 1;
        pageFaultHandler(page_number);
        }
    virt_mem[page_number].dirty = 1; // set dirty bit to 1 so we know we have written to this page
    int place_in_main = virtual_addy % 8;
    physical_mem[virt_mem[page_number].physical_page_number].content[place_in_main] = data;
    physical_mem[virt_mem[page_number].physical_page_number].times_used += 1;
    
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
