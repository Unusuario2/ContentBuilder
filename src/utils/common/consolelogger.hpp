//=== ConsoleLogger -> Written by Unusuario2, https://github.com/Unusuario2  ===//
//
// Purpose: Shared class across the tooling, used for speficic logging options.
//
// License:
//        MIT License
//
//        Copyright (c) 2025 [un usuario], https://github.com/Unusuario2
//
//        Permission is hereby granted, free of charge, to any person obtaining a copy
//        of this software and associated documentation files (the "Software"), to deal
//        in the Software without restriction, including without limitation the rights
//        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//        copies of the Software, and to permit persons to whom the Software is
//        furnished to do so, subject to the following conditions:
//
//        The above copyright notice and this permission notice shall be included in all
//        copies or substantial portions of the Software.
//
//        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//        SOFTWARE.
//
// $NoKeywords: $
//==============================================================================//
#ifndef CONSOLELOGGER_HPP
#define CONSOLELOGGER_HPP

#ifdef _WIN32
#pragma once
#endif // _WIN32

#include <colorschemetools.h>
#include <filesystem_init.h>
#include <pipeline_shareddefs.h>
#include <resourcecopy/cresourcecopy.hpp>


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CConsoleLogger
{
private:
    CResourceCopy*  m_pResourceCopy         = nullptr;
    FILE*           m_pLogFile              = nullptr;
    FILE*           m_pWarningFile          = nullptr;
    FILE*           m_pErrorFile            = nullptr;
    const char*     m_szLogBaseOutDir       = nullptr;
    const char*     m_szLogWarningOutFile   = nullptr;
    const char*     m_szLogErrorOutFile     = nullptr;
    const char*     m_szLogNormalOutFile    = nullptr;
    bool            m_bRunLogger            = true;

public:
    inline CConsoleLogger(const char* szLogBaseDir, const char* szLogBaseFile, const char* szLogWarningFile, const char* szLogErrorFile);
    inline ~CConsoleLogger();

    inline void Error(const char* format, ...);
    inline void Warning(const char* format, ...);
    inline void Print(const char* format, ...);
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CConsoleLogger::CConsoleLogger(const char* szLogBaseDir, const char* szLogBaseFile, const char* szLogWarningFile, const char* szLogErrorFile)
{
    m_pResourceCopy = new CResourceCopy();
    m_szLogBaseOutDir = V_strdup(szLogBaseDir);
    m_szLogNormalOutFile = V_strdup(szLogBaseFile);
    m_szLogWarningOutFile = V_strdup(szLogWarningFile);
    m_szLogErrorOutFile = V_strdup(szLogErrorFile);
    
    if(!m_pResourceCopy->CreateDir(szLogBaseDir))
    {
        Warning("ConsoleLogger -> Could not create base dir for loggers!\n");
        m_bRunLogger = false;
        return;
    }

    char szLogFile[MAX_PATH];
    V_sprintf_safe(szLogFile, "%s\\%s", m_szLogBaseOutDir, m_szLogNormalOutFile);
    m_pLogFile = fopen(szLogFile, "a");
    if(!m_pLogFile)
    {
        Warning("ConsoleLogger -> Cannot create log message!\n"
                "ConsoleLogger -> Skipping logging!\n");
        m_bRunLogger = false;
        return;
    }

    char szWarningFile[MAX_PATH];
    V_sprintf_safe(szWarningFile, "%s\\%s", m_szLogBaseOutDir, m_szLogWarningOutFile);
    m_pWarningFile = fopen(szWarningFile, "a");
    if (!m_pWarningFile)
    {
        Warning("ConsoleLogger -> Cannot create warning log message!\n"
                "ConsoleLogger -> Skipping logging!\n");
        m_bRunLogger = false;
        return;
    }

    char szErrorFile[MAX_PATH];
    V_sprintf_safe(szErrorFile, "%s\\%s", m_szLogBaseOutDir, m_szLogErrorOutFile);
    m_pErrorFile = fopen(szWarningFile, "a");
    if (!m_pErrorFile)
    {
        Warning("ConsoleLogger -> Cannot create error log message!\n"
                "ConsoleLogger -> Skipping logging!\n");
        m_bRunLogger = false;
        return;
    }
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CConsoleLogger::~CConsoleLogger()
{
    delete m_pResourceCopy;
    delete[] m_szLogBaseOutDir;
    delete[] m_szLogNormalOutFile;
    delete[] m_szLogWarningOutFile;
    delete[] m_szLogErrorOutFile;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CConsoleLogger::Error(const char* format, ...)
{
    char str[MAX_CMD_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, format);

    V_vsnprintf(str, sizeof(str), format, argptr);
    if(m_bRunLogger)
        fprintf(m_pErrorFile, "%s", str);
    if (m_pErrorFile)
        fclose(m_pErrorFile);

    ::Error("%s", str);

    va_end(argptr);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline void CConsoleLogger::Warning(const char* format, ...)
{
    char str[MAX_CMD_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, format);

    V_vsnprintf(str, sizeof(str), format, argptr);
    if (m_bRunLogger)
        fprintf(m_pWarningFile, "%s", str);
    if (m_pWarningFile)
        fclose(m_pWarningFile);

     ::Warning("%s", str);

    va_end(argptr);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline void CConsoleLogger::Print(const char* format, ...)
{
    char str[MAX_CMD_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, format);

    V_vsnprintf(str, sizeof(str), format, argptr);
    if (m_bRunLogger)
        fprintf(m_pLogFile, "%s", str);
    if (m_pLogFile)
        fclose(m_pLogFile);

    ::Msg("%s", str);

    va_end(argptr);
}


#endif // CONSOLELOGGER_HPP

