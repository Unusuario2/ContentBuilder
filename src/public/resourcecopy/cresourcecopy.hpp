//=== CResourceCopy -> Written by Unusuario2, https://github.com/Unusuario2  ====//
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
#ifndef CRESOURCECOPY_H
#define CRESOURCECOPY_H

#ifdef _WIN32
#pragma once
#endif // _WIN32

#include <array>
#include <vector>
#include <mutex>
#include <pipeline_shareddefs.h>


//-----------------------------------------------------------------------------
// Purpose: Generic data containers
//-----------------------------------------------------------------------------
using ContainerString           = std::array<char, MAX_PATH>;
using ContainerList             = std::vector<ContainerString>;
using ContainerStringExtended   = std::array<char, MAX_CMD_BUFFER_SIZE>;
using ContainerListExtended     = std::vector<ContainerStringExtended>;


//-----------------------------------------------------------------------------
// Purpose: Specific File containers
//-----------------------------------------------------------------------------
using FileString                = ContainerString;
using FileStringExtended        = ContainerStringExtended;
using FileList                  = ContainerList;
using FileListExtended          = ContainerListExtended;


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CResourceCopy
{
private:
    float       m_flTime;
    int         m_iThreads;
    int         m_iCompletedOperations;
    int         m_iSkippedOperations;
    int         m_iFailedOperations;
    SpewMode    m_eSpewMode;
    std::mutex  m_MsgMutex;

private:
    void SetLogicalProcessorCount();
    FileList ScanDirectoryWorker(const char* baseDir, bool fullScan, const char* pExtFilter);

public:
    CResourceCopy::CResourceCopy();

    // Single File Operations
    bool CopyFileTo(const char* pSrcPath, const char* pDstPath, const bool bOverwrite);
    bool TransferFileTo(const char* pSrcPath, const char* pDstPath, const bool bOverwrite);
    bool DeleteFileIn(const char* pDir);
    bool DirExist(const char* pszDir);
    bool FileExist(const char* pszFile);
    bool DeleteEmptyFolder(const char* pszDir);
    bool CreateDir(const char* pszDir);
    bool IsWritable(const char* pSrcDir, const char* pDstDir, const bool bIsPath);

    // Batch File Operations (Note: They support multithreading and wildcards!)
    void CopyDirTo(const char* pSrcDir, const char* pDstDir, const bool bMultiThread = true, const bool bOverwrite = true, const FileList* pCopyList = nullptr);
    void TransferDirTo(const char* pSrcDir, const char* pDstDir, const bool bMultiThread, const bool bDeleteMainFolder = false, const bool bOverwrite = true, const FileList* pDeleteList = nullptr);
    void DeleteDirRecursive(const char* pDir, const bool bMultiThread = true, const bool bDeleteMainFolder = false, const FileList* pDeleteList = nullptr);
    FileList ScanDirectoryRecursive(const char* pszDir);

    // Sanity checks
    std::size_t GetFileSizeFast(const char* pszFile);
    std::size_t GetDriveFreeSpace(const char* folderPath);
    std::size_t GetFolderSize(const char* inputPath);

    // Misc
    void PrintDirContents(const char* pDir);
    void GenerateGlobalOperationReport();
    void GenerateHardwareReport(const char* pSrc, const char* pDst, const bool bIsPath);
    void GenerateErrorReport();
    void SetThreads(const int iThreads);
    void SetVerboseSpewMode();
    void SetNormalSpewMode();
    void SetQuietSpewMode();
};


#endif // CRESOURCECOPY_H

