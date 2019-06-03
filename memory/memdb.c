// #include from dbdump.c file
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "memdb.h"      // reminder from Richard Pham

int main(char argc, int **argv) {
    // fd mmap code from dbdump.c and suggestion from Andy Park
    const char *dbfile = argv[1];
    
    int fd = open(dbfile, O_RDONLY);
    if (fd == -1) {
        perror(dbfile);
        exit(2);
    }
    struct stat s;
    if (fstat(fd, &s) == -1) {
        perror(dbfile);
        exit(3);
    }
    
    struct fhdr_s *fhdr = mmap(NULL, MAX_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (fhdr == (void *) -1) {
        perror(dbfile);
        exit(4);
    }
    // get a byte pointer to the beginning of the file so that we can
    // just add byte offsets
    char *ptr = (char *)fhdr;
    
    char c;
    char data[100]; // suggestion from Jefferson Ly
    struct entry_s* list = NULL; 
    // print statement that says command is running suggestion from Professor Reed
    // while-loop code help from https://www.includehelp.com/code-snippets/how-to-identify-enter-key-is-pressed-in-c-programming-language.aspx
    while (1) { 
        c = fgetc(stdin); 
        struct entry_s* s = NULL; 
        // list
        
        if (c == 0x6C) {   
            // * reminder from Richard Pham
            while (s != NULL) {
                printf("%s\n", s->str);
                s = s->next; 
            }
            printf("You are now seeing the list. %c is now running\n", c); 
        }
        // add code suggestion from 
        // https://www.geeksforgeeks.org/linked-list-set-1-introduction/
        else if (c == 0x61) {
            s = fgets(data, 100, stdin);   
            
            struct entry_s* current = list;
            if (list == NULL) {
                *list = s; 
            }
            
        else {
            while(current->next != NULL) {
                if (s->str == current->str) {
                    perror("Item is already in list.\n");
                    exit(3); 
                }
                else if (s->str < current->str) {
                    break; 
                }
                else {
                    current = current->next; 
                }
            }
            current->next = s; 
        }
            printf("You are now adding % c to the list. %c is now running\n", s, c); 
        } 
        
        c = getchar();
    }
    return 0; 
}
