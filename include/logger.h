#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {      
    NONE = 0,
    FATAL,
    DEBUG,
    INFO,
    WARN,
    ERROR,
} LogLevel;

typedef enum {
    NGEN,
    NOAHOWP,
    SNOW17,
    UEB,
    CFE,
    SACSMA,
    LASAM,
    SMP,
    SFT,
    TROUTE,
    SCHISM,
    SFINCS,
    GC2D,
    TOPOFLOW,
    MODULE_COUNT
} LoggingModule;

static const char* module_name[MODULE_COUNT] = {
    "NGEN    ",
    "NOAHOWP ",
    "SNOW17  ",
    "UEB     ",
    "CFE     ",
    "SACSMA  ",
    "LASAM   ",
    "SMP     ",
    "SFT     ",
    "TROUTE  ",
    "SCHISM  ",
    "SFINCS  ",
    "GC2D    ",
    "TOPOFLOW"
};

typedef struct {
    LogLevel logLevel;
    FILE* logFile;
} Logger;

Logger* GetInstance();
void SetLogPreferences(Logger* logger, LogLevel level);
void Log(Logger* logger, const char* message, LogLevel messageLevel, LoggingModule module);
LogLevel GetLogLevel(const char* logLevel);
char* createTimestamp();

#endif
