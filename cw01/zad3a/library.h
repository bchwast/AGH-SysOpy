//
// Created by bchwast on 03/03/2022.
//

#ifndef SYSOPY_LIBRARY_H
#define SYSOPY_LIBRARY_H


void create_array(unsigned int size_);

void execute_wc(char* files);

int insert(char* block);

char* allocate_block(void);

int wc_files(char* files);

void remove_block(int index);

void print_array(void);

void remove_array(void);

#endif //SYSOPY_LIBRARY_H
