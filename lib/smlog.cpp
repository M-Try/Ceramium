/* Simple Mlib Logging main file
    author: M-Try
    cera reconstruction note: done

    last edited: 12.1.2023
*/

#include <stdio.h>

#include "./smlog.hpp"

void base_logger::set_level(unsigned short level) {
    this->level = level;
}

void base_logger::set_level(loglevel level) {
    this->level = level.level;
}


// TODO: Add something like this:
/*
void flog(loglevel level, char *format, ...) {
    if (this->level >= level.level) {
        char *_buf;
        vsprintf(_buf, format, );
        fprintf(this->logstream, "%s [%s] %s", this->name, level.name, _buf);
    }
}
basically a function that allows you to use flog like printf with varargs

*/


file_logger::file_logger(char *name, char *path) {
    this->name = name;
    logstream = fopen(path, "w");
}

file_logger::~file_logger() {
    fflush(logstream);
    fclose(logstream);
}

void file_logger::log(loglevel level, char *message) {
    if (this->level >= level.level) {
        fprintf(this->logstream, format_string, this->name, level.name, message);
    }
}

void file_logger::log(logrecord *message) {
    if (this->level >= message->level.level) {
        fprintf(this->logstream, format_string, this->name, message->level.name, message->text);
    }
}

con_logger::con_logger(char *name) {
    this->name = name;
}

void con_logger::log(loglevel level, char *message) {
    if (this->level >= level.level) {
        printf(format_string, this->name, level.name, message);
    }
}

void con_logger::log(logrecord *message) {
    if (this->level >= message->level.level) {
        printf(format_string, this->name, message->level.name, message->text);
    }
}
