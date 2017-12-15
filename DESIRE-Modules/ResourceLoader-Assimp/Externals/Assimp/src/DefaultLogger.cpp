/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2017, assimp team


All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file  DefaultLogger.cpp
 *  @brief Implementation of DefaultLogger (and Logger)
 */


// Default log streams
#include "StringUtils.h"

#include <assimp/DefaultIOSystem.h>
#include <assimp/NullLogger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ai_assert.h>
#include <iostream>
#include <stdio.h>

#ifndef ASSIMP_BUILD_SINGLETHREADED
#   include <thread>
#   include <mutex>

std::mutex loggerMutex;
#endif

namespace Assimp    {

// ----------------------------------------------------------------------------------
NullLogger DefaultLogger::s_pNullLogger;
Logger *DefaultLogger::m_pLogger = &DefaultLogger::s_pNullLogger;

static const unsigned int SeverityAll = Logger::Info | Logger::Err | Logger::Warn | Logger::Debugging;

// ----------------------------------------------------------------------------------
// Represents a log-stream + its error severity
struct LogStreamInfo
{
    unsigned int m_uiErrorSeverity;
    LogStream *m_pStream;

    // Constructor
    LogStreamInfo( unsigned int uiErrorSev, LogStream *pStream ) :
        m_uiErrorSeverity( uiErrorSev ),
        m_pStream( pStream )
    {
        // empty
    }

    // Destructor
    ~LogStreamInfo()
    {
        delete m_pStream;
    }
};

// ----------------------------------------------------------------------------------
void Logger::debug(const char* message) {

    // SECURITY FIX: otherwise it's easy to produce overruns since
    // sometimes importers will include data from the input file
    // (i.e. node names) in their messages.
    if (strlen(message)>MAX_LOG_MESSAGE_LENGTH) {
        return;
    }
    return OnDebug(message);
}

// ----------------------------------------------------------------------------------
void Logger::info(const char* message)  {

    // SECURITY FIX: see above
    if (strlen(message)>MAX_LOG_MESSAGE_LENGTH) {
        return;
    }
    return OnInfo(message);
}

// ----------------------------------------------------------------------------------
void Logger::warn(const char* message)  {

    // SECURITY FIX: see above
    if (strlen(message)>MAX_LOG_MESSAGE_LENGTH) {
        return;
    }
    return OnWarn(message);
}

// ----------------------------------------------------------------------------------
void Logger::error(const char* message) {

    // SECURITY FIX: see above
    if (strlen(message)>MAX_LOG_MESSAGE_LENGTH) {
        return;
    }
    return OnError(message);
}

// ----------------------------------------------------------------------------------
void DefaultLogger::set( Logger *logger )
{
    // enter the mutex here to avoid concurrency problems
#ifndef ASSIMP_BUILD_SINGLETHREADED
    std::lock_guard<std::mutex> lock(loggerMutex);
#endif

    if (!logger)logger = &s_pNullLogger;
    if (m_pLogger && !isNullLogger() )
        delete m_pLogger;

    DefaultLogger::m_pLogger = logger;
}

// ----------------------------------------------------------------------------------
bool DefaultLogger::isNullLogger()
{
    return m_pLogger == &s_pNullLogger;
}

// ----------------------------------------------------------------------------------
Logger *DefaultLogger::get() {
    return m_pLogger;
}

// ----------------------------------------------------------------------------------
//  Kills the only instance
void DefaultLogger::kill()
{
    // enter the mutex here to avoid concurrency problems
#ifndef ASSIMP_BUILD_SINGLETHREADED
    std::lock_guard<std::mutex> lock(loggerMutex);
#endif

	if ( m_pLogger == &s_pNullLogger ) {
		return;
	}
    delete m_pLogger;
    m_pLogger = &s_pNullLogger;
}

// ----------------------------------------------------------------------------------
} // !namespace Assimp
