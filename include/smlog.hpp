/* Simple Mlib Logging header
    author: M-Try
    cera reconstruction note: done

    last edited: 12.1.2023
*/



#pragma once

#ifndef MLIB_LOGGING
#define MLIB_LOGGING

// default format of a log message
#define LOG_DEFAULT_FORMAT "%s [%s] %s\n"

#include <stdio.h>

struct loglevel {
    const short unsigned level;
    const char *name;
};

// built-in loglevels
static constexpr struct loglevel LOG_ALL     { 0U,   "ALL"    };
static constexpr struct loglevel LOG_FATAL   { 100U, "FATAL"  };
static constexpr struct loglevel LOG_ERROR   { 200U, "ERROR"  };
static constexpr struct loglevel LOG_WARN    { 300U, "WARN"   };
static constexpr struct loglevel LOG_INFO    { 400U, "INFO"   };
static constexpr struct loglevel LOG_DEBUG   { 500U, "DEBUG"  };

struct logrecord {
    loglevel level;
    char *text;
};

// base logger class
class base_logger {
public:
    char *name;
    char *format_string = LOG_DEFAULT_FORMAT;
    unsigned short level = LOG_ALL.level;

    void set_level(unsigned short level);
    void set_level(loglevel level);

    virtual void log(loglevel level, char *message);
    virtual void log(logrecord *message);
};

// file logger
class file_logger : public base_logger {
public:
    FILE *logstream = nullptr;

    file_logger(char *name, char *path);
    ~file_logger();

    void log(loglevel level, char *message);
    void log(logrecord *message);
};

// console logger
class con_logger : public base_logger {
public:
    con_logger(char *name);

    void log(loglevel level, char *message);
    void log(logrecord *message);
};

#endif
