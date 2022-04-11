//
// Created by bchwast on 11/04/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_mails(int mode) {
    FILE* file;

    if (mode == 0) {
        file = popen("mail -f | tail -n +2 | sort -k3d -", "w");
    }
    else {
        file = popen("mail -f | tail -n +2", "w");
    }

    fputs("exit", file);
    pclose(file);
}


void write_mail(char* mail, char* subject, char* message) {
    FILE* file;
    char buff[2000];
    sprintf(buff, "mail -s %s %s", subject, mail);
    file = popen(buff, "w");

    fputs(message, file);
    pclose(file);
}


int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 4) {
        printf("Wrong number of arguments, try: \"<nadawca|data> | [address] [subject] [contents]\"\n");
        exit(1);
    }

    if (argc == 2) {
        if (strcmp(argv[1], "nadawca") == 0) {
            print_mails(0);
        } else if (strcmp(argv[1], "data") == 0) {
            print_mails(1);
        } else {
            printf("Wrong argument, try: <nadawca|data>\n");
            exit(1);
        }
    } else {
        write_mail(argv[1], argv[2], argv[3]);
    }

    return 0;
}