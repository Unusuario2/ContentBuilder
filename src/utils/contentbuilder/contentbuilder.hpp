//== ContentBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ===//
//
// Purpose: 
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
#ifndef CONTENTBUILDER_HPP
#define CONTENTBUILDER_HPP

#ifdef _WIN32
#pragma once
#endif // _WIN32

#include <platform.h>
#include <resourcecopy/cresourcecopy.hpp>
#include <consolelogger.hpp>


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
#define KV_BUILDER_MAIN            "ContentBuilder"
#define KV_EXCLUDE                 "Exclude"
#define KV_EXCLUDE_STRING          "ExcludeFileOrFolder"
#define KV_BUILDPARAMS             "BuildParams"

#define BUILDER_CONFIG_FILE        "scripts\\tools\\contentbuilder_settings.txt"
#define BUILDER_SPECIFIC_LOG	   "standart.log"
#define BUILDER_WARNING_LOG        "warning.log"
#define BUILDER_ERROR_LOG          "error.log"
#define BUILDER_ASSET_REPORT_SRC   "asset_report_source.contentlist"
#define BUILDER_ASSET_REPORT_COM   "asset_report_compiled.contentlist"

#define BUILDER_OUTDIR				"_build"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
extern int  g_iThreads;
extern bool g_bForceBuildContent;
extern bool g_bContentDstDelete;
extern char g_szGameBin[MAX_PATH];
extern char g_szContentBuilderOutPath[MAX_PATH];
extern char g_szGameInfoFile[MAX_PATH];
extern char g_szSteamDir[MAX_PATH];
extern char g_szContentBuilderScriptFile[MAX_PATH];
extern CConsoleLogger*	g_pConsoleLogger;
extern SpewMode			g_eSpewMode;
extern CResourceCopy*	g_pResourceCopy;


#endif // CONTENTBUILDER_HPP

