/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_LOG_H
#define VIGILSDK_LOG_H

#include <plog/Log.h>
#include <plog/Init.h>

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

namespace vigil
{
    void InitializeLogging();
}

#endif //VIGILSDK_LOG_H
