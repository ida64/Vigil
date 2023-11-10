/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/

#include "Log.h"

void vigil::InitializeLogging()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> sColorConsoleAppender;

    plog::init(plog::verbose, &sColorConsoleAppender);
}
