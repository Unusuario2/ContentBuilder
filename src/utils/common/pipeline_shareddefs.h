//= PipeLine_SharedDefs -> Written by Unusuario2, https://github.com/Unusuario2  =//
//
// Purpose: Contains shared constants, paths, filenames,
//          extensions, tool names, used by the
//          content build pipeline (materials, models, maps,
//          sounds, scenes, captions, VPKs, addons).
//          Centralizes definitions so all build tools and
//          subsystems remain consistent.
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
#ifndef PIPELINE_SHAREDDEFS_H
#define PIPELINE_SHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif // _WIN32


//-----------------------------------------------------------------------------
// Purpose:	Verbose levels
//-----------------------------------------------------------------------------
enum class SpewMode
{
    k_Quiet,
    k_Normal,
    k_Verbose,
    k_VeryHighVerbose
};


// Naming convertions:
//  FILE, CONFIG, LOG, REPORT   -> for files
//  DIR, OUTDIR, TEMPDIR        -> for directories
//  TOOL_*                      -> for tool executables
//  EXT_*                       -> for extensions


//-----------------------------------------------------------------------------
// Purpose:	Common dir defs
//-----------------------------------------------------------------------------
#define MAX_CMD_BUFFER_SIZE			8192u 
#define DIR_TOOLS_X64              "bin\\x64"
#define DIR_TOOLS_X86              "bin"       


//-----------------------------------------------------------------------------
// Purpose:	Common tools defs
//-----------------------------------------------------------------------------
#define TOOL_COPY                   "resourcecopy.exe"


//-----------------------------------------------------------------------------
// Purpose:	Common Materials defs
//-----------------------------------------------------------------------------
#define DIR_MATERIALS              "materials"
#define DIR_MATERIALSRC            "materialsrc"

#define EXT_MATERIAL               ".vmt"
#define EXT_TEXTURE                ".vtf"
#define EXT_TEXTURESRC_TGA         ".tga"
#define EXT_TEXTURESRC_PFM         ".pfm"
#define EXT_TEXTURESRC_PSD         ".psd"

#define TOOL_MATERIAL              "vtex.exe"


//-----------------------------------------------------------------------------
// Purpose:	Common Models defs
//-----------------------------------------------------------------------------
#define DIR_MODELS                 "models"
#define DIR_MODELSRC               "modelsrc"

#define EXT_MODEL                  ".mdl"
#define EXT_MODELSRC               ".qc"

#define TOOL_MODEL                 "studiomdl.exe"


//-----------------------------------------------------------------------------
// Purpose:	Common sounds defs
//-----------------------------------------------------------------------------
#define DIR_SOUNDS                 "sounds"
#define DIR_SOUNDSRC               "soundsrc"

#define EXT_SOUND                  ".wav"
#define EXT_SOUNDSRC               ".mp3"


//-----------------------------------------------------------------------------
// Purpose:	Common Scene defs
//-----------------------------------------------------------------------------
#define DIR_SCENES                 "scenes"
#define DIR_SCENESRC               "scenes"

#define EXT_SCENE                  ".image"
#define EXT_SCENESRC               ".vcd"
#define FILE_SCENE				   "scene" EXT_SCENE

#define TOOL_SCENE                 "sceneimagebuilder.exe"


//-----------------------------------------------------------------------------
// Purpose:	Common Captions defs
//-----------------------------------------------------------------------------==
#define DIR_CAPTIONS               "resource"
#define DIR_CAPTIONSRC             "resource\\closecaptionsrc"

#define EXT_CAPTION                ".dat"
#define EXT_CAPTIONSRC             ".txt"

#define TOOL_CAPTION               "captioncompiler.exe"


//-----------------------------------------------------------------------------
// Purpose:	Common Maps defs
//-----------------------------------------------------------------------------
#define DIR_MAPS                   "maps"
#define DIR_MAPSRC                 "mapsrc"
#define EXT_MAP                    ".bsp"
#define EXT_MAPSRC_VMF             ".vmf"
#define EXT_MAPSRC_VMN             ".vmn"
#define TOOL_MAP_VBSP              "vbsp.exe"
#define TOOL_MAP_VVIS              "vvis.exe"
#define TOOL_MAP_VRAD              "vrad.exe"
#define TOOL_MAP_INFO              "vbspinfo.exe"
#define TOOL_MAP_BSPZIP            "bspzip.exe"
#define TOOL_MAPBUILDER            "mapbuilder.exe"


//-----------------------------------------------------------------------------
// Purpose:	Common Valve Pack File defs
//-----------------------------------------------------------------------------
#define EXT_VPK                    ".vpk"
#define TOOL_VPK                   "vpk.exe"


#endif // PIPELINE_SHAREDDEFS_H

