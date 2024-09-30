#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include "logger.h"

#define MODULE_COUNT 14


Logger* loggerInstance = NULL;

char* createTimestamp() {
    static char buffer[80];
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H:%M:%S", timeinfo);
    return buffer;
}

void SetLogPreferences(Logger* logger, LogLevel level) {
    logger->logLevel = level;
    const char* logFileDir = "./run_logs/ngen_";
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s%s/", logFileDir, createTimestamp());
    
    printf("Log File Directory: %s\n", fullPath);

    char mkdir_cmd[512];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", fullPath);
    int status = system(mkdir_cmd);
    if (status == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    } else {
        printf("Directories are created\n");
    }

    char logFilePath[512];
    snprintf(logFilePath, sizeof(logFilePath), "%sngen_log.txt", fullPath);
    logger->logFile = fopen(logFilePath, "w");
    if (logger->logFile == NULL) {
        fprintf(stderr, "Can't Open Log File\n");
    }
}

Logger* GetInstance() {
    if (loggerInstance == NULL) {
        loggerInstance = (Logger*)malloc(sizeof(Logger));
        SetLogPreferences(loggerInstance, NONE);
    }
    return loggerInstance;
}

void Log(Logger* logger, const char* message, LogLevel messageLevel, LoggingModule module) {
    if (messageLevel >= logger->logLevel) {
        const char* logType;
        switch (messageLevel) {
            case FATAL: logType = "FATAL "; break;
            case DEBUG: logType = "DEBUG "; break;
            case INFO:  logType = "INFO  "; break;
            case WARN:  logType = "WARN  "; break;
            case ERROR: logType = "ERROR "; break;
            default:    logType = "NONE  "; break;
        }

        char final_message[1024];
        snprintf(final_message, sizeof(final_message), "%s %s %s%s\n", 
                 createTimestamp(), module_name[module], logType, message);
        
        fprintf(logger->logFile, "%s", final_message);
        fflush(logger->logFile);
    }
}

LogLevel GetLogLevel(const char* logLevel) {
    if (strcmp(logLevel, "DEBUG") == 0) return DEBUG;
    if (strcmp(logLevel, "INFO") == 0)  return INFO;
    if (strcmp(logLevel, "WARN") == 0)  return ERROR;
    if (strcmp(logLevel, "ERROR") == 0) return ERROR;
    if (strcmp(logLevel, "FATAL") == 0) return ERROR;
    return NONE;
}

void setup_logger(void) {
    Logger* logger = GetInstance();

    Log(logger, "Sample Log for LogLevel::ERROR", ERROR, CFE);
    Log(logger, "Sample Log for LogLevel::FATAL", FATAL, CFE);
    Log(logger, "Sample Log for LogLevel::WARN", WARN, CFE);
    Log(logger, "Sample Log for LogLevel::INFO", INFO, CFE);
    
    const char* multiline_log = 
        "First line of multiline log:\n"
        "   Indented second line of multiline log\n"
        "         Indented third line of multiline log\n"
        "                Indented fourth line of multiline log";
    Log(logger, multiline_log, INFO, CFE);
    
    Log(logger, "Sample Log for LogLevel::DEBUG", DEBUG, CFE);
}
