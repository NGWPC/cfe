#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include "logger.h"

#define MODULE_COUNT 14


Logger* loggerInstance = NULL;

long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

char* createTimestamp() {
    static char buffer[256];
    char buffer1[256];
    time_t now = time(NULL);
    
    long millis = (long)(timeInMilliseconds() % 1000);
    

    struct tm* timeinfo = gmtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S:", timeinfo);
    snprintf(buffer1, sizeof(buffer1), "%03d", (int)millis);
    strcat(buffer, buffer1);

    return buffer;
}

void SetLogPreferences(Logger* logger) {
    // get the log level for CFE module
    char * logLevel = getenv("cfe_ll");
    if (logLevel != NULL) {
        logger->logLevel = GetLogLevel(logLevel);
    }
    else {
        logger->logLevel = INFO;
    }
    printf("CFE module's log level is set at: %s\n", logLevel);

	// get the log file path
	char * log_file_path;
	log_file_path = getenv("NGEN_LOG_FILE_PATH");

    logger->logFile = fopen(log_file_path, "a");
    if (logger->logFile == NULL) {
        printf("Can't Open Log File for CFE\n");
        // create local log directory and file
        const char* logFileDir = "./run_logs/ngen_";
        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "%s%s/", logFileDir, createTimestamp());

        printf("CFE Log File Directory: %s\n", fullPath);

        char mkdir_cmd[512];
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", fullPath);
        int status = system(mkdir_cmd);
        if (status == -1) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
        } else {
            printf("Directories are created for CFE\n");
        }

        char logFilePath[512];
        snprintf(logFilePath, sizeof(logFilePath), "%scfe_log.txt", fullPath);
        logger->logFile = fopen(logFilePath, "w");
        if (logger->logFile == NULL) {
            fprintf(stderr, "Can't Open local Log File for CFE\n");
        }
        else {
            printf("CFE Log File Path: %s\n", logFilePath);
        }
    }
    else {
        printf("CFE Log File Path: %s\n", log_file_path);
    }
}

Logger* GetInstance() {
    if (loggerInstance == NULL) {
        loggerInstance = (Logger*)malloc(sizeof(Logger));
        SetLogPreferences(loggerInstance);
    }
    return loggerInstance;
}

const char* getLogLevelString(LogLevel level) {
    const char* logType;
    switch (level) {
        case FATAL: logType = "FATAL "; break;
        case DEBUG: logType = "DEBUG "; break;
        case INFO:  logType = "INFO  "; break;
        case WARN:  logType = "WARN  "; break;
        case ERROR: logType = "ERROR "; break;
        default:    logType = "NONE  "; break;
    }

    return logType;
}

void Log(LogLevel messageLevel, const char* message, ...) {
    Logger* logger = GetInstance();
    va_list arglist;
    va_start(arglist, message);

    int length = vsnprintf(NULL, 0, message, arglist);
    char *buffer = malloc(length * sizeof *buffer); 
    vsnprintf(buffer, length, message, arglist);
    va_end(arglist);

    if (messageLevel >= logger->logLevel) {
        char final_message[1024];
        snprintf(final_message, sizeof(final_message), "%s %s %s%s\n", 
                 createTimestamp(), module_name[CFE], getLogLevelString(messageLevel), buffer);
        
        if (logger->logFile != NULL) {
            fprintf(logger->logFile, "%s", final_message);
            fflush(logger->logFile);
        }
    }
    
    free(buffer);
}

LogLevel GetLogLevel(const char* logLevel) {
    if (strcmp(logLevel, "DEBUG") == 0) return DEBUG;
    if (strcmp(logLevel, "INFO") == 0)  return INFO;
    if (strcmp(logLevel, "WARN") == 0)  return ERROR;
    if (strcmp(logLevel, "ERROR") == 0) return ERROR;
    if (strcmp(logLevel, "FATAL") == 0) return ERROR;
    return NONE;
}
