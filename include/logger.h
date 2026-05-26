#ifndef CFE_LOGGER_H
#define CFE_LOGGER_H

#include "ewts/module_constants.h"
#include "ewts/logger.h"
#include "ewts/log_levels.h"

#define Log(level, ...) EwtsLogModule(EWTS_ID_CFE, (level), __VA_ARGS__)
#define LOG(level, ...) EwtsLogModule(EWTS_ID_CFE, (level), __VA_ARGS__)
#define GetLogLevel() EwtsGetLogLevelModule(EWTS_ID_CFE)
#define IsLoggingEnabled() EwtsIsLoggingEnabledModule(EWTS_ID_CFE)

#endif // CFE_LOGGER_H
