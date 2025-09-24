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
#include <io.h>
#include <windows.h>
#include <wchar.h>
#include <vector>
#include <array>
#include <future>
#include <tier0/dbg.h>
#include <filesystem_tools.h>
#include <colorschemetools.h>
#include "resourcecopy/cresourcecopy.hpp"

#if !defined(_WIN32) || !defined(_WIN64) 
	#error "CResourceCopy: Suported platform only Windows!"
#endif

#pragma warning(disable : 4238)


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::SetLogicalProcessorCount()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_iThreads = sysInfo.dwNumberOfProcessors;
}


//-----------------------------------------------------------------------------
// Purpose: Worker function (internal, recursive/shallow handling)
//-----------------------------------------------------------------------------
FileList CResourceCopy::ScanDirectoryWorker(const char* baseDir, bool fullScan, const char* pExtFilter)
{
    FileList DirFileList;
    char searchPath[MAX_PATH];
    V_snprintf(searchPath, sizeof(searchPath), "%s\\*.*", baseDir);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return DirFileList;

    do
    {
        if (V_strcmp(findData.cFileName, ".") == 0 || V_strcmp(findData.cFileName, "..") == 0)
            continue;

        char fullPath[MAX_PATH];
        V_snprintf(fullPath, sizeof(fullPath), "%s\\%s", baseDir, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (fullScan)
            {
                FileList subFiles = this->ScanDirectoryWorker(fullPath, fullScan, pExtFilter);
                DirFileList.insert(DirFileList.end(), subFiles.begin(), subFiles.end());
            }
        }
        else
        {
            // Filter by extension
            if (pExtFilter)
            {
                const char* dot = V_strrchr(findData.cFileName, '.');
                if (!dot) continue;

                const char* fileExt = dot + 1;
                const char* filterExt = (pExtFilter[0] == '.') ? pExtFilter + 1 : pExtFilter;

                if (V_stricmp(fileExt, filterExt) != 0)
                    continue;
            }

            FileString Temp;
            V_strcpy(Temp.data(), fullPath);
            DirFileList.push_back(Temp);
        }

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return DirFileList;
}

//-----------------------------------------------------------------------------
// Purpose: Entry function (handles wildcards and decides shallow vs recursive)
// Input  : pszDir -> Can be both a path or wildcard (e.g: C:\test or C:\test\*.txt)
//-----------------------------------------------------------------------------
FileList CResourceCopy::ScanDirectoryRecursive(const char* pszDir)
{
    FileList DirFileList;
    if (!pszDir || !*pszDir)
        return DirFileList;

    char baseDir[MAX_PATH];
    V_strcpy(baseDir, pszDir);

    // Detect if path has a wildcard
    const char* pStar = V_strrchr(baseDir, '*');
    bool hasWildcard = (pStar != nullptr);

    // Extension filter (stable buffer for recursion)
    static thread_local char sExtFilter[64];
    const char* pExtFilter = nullptr;

    if (hasWildcard)
    {
        // If wildcard is like "*.ext", extract extension
        const char* dot = V_strchr(pStar, '.');
        if (dot && *(dot + 1) != '\0')
        {
            V_strncpy(sExtFilter, dot + 1, sizeof(sExtFilter));
            pExtFilter = sExtFilter;
        }

        // Trim back to the directory containing the wildcard
        char* pSlash = V_strrchr(baseDir, '\\');
        if (pSlash && pSlash < pStar)
            *pSlash = '\0';

        // Recursive full scan
        return this->ScanDirectoryWorker(baseDir, true, pExtFilter);
    }
    else
    {
        // No wildcard -> shallow scan, no extension filter
        return this->ScanDirectoryWorker(baseDir, false, nullptr);
    }
}





//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CResourceCopy::CResourceCopy()
{
    m_eSpewMode = SpewMode::k_Normal;
    m_flTime = Plat_FloatTime();
    this->SetLogicalProcessorCount();
    m_iCompletedOperations  = 0;
    m_iSkippedOperations    = 0;
    m_iFailedOperations     = 0;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::CopyFileTo(const char* pSrcPath, const char* pDstPath, const bool bOverwrite)
{
    if ((!pSrcPath || !*pSrcPath) || (!pDstPath || !*pDstPath))
        return false;

    bool bSuccess = CopyFileA(pSrcPath, pDstPath, !bOverwrite);
    if (!bSuccess)
    {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS || err == ERROR_FILE_EXISTS)
            bSuccess = true;
    }

    if (bSuccess)
        m_iCompletedOperations++;
    else
        m_iFailedOperations++;

    return bSuccess;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::TransferFileTo(const char* pSrcPath, const char* pDstPath, const bool bOverwrite)
{
    if ((!pSrcPath || !*pSrcPath) || (!pDstPath || !*pDstPath))
        return false;

    bool bCopySucess = CopyFileA(pSrcPath, pDstPath, !bOverwrite);
    bool bDeleteSucess = DeleteFileA(pSrcPath);

    if (bCopySucess)
        m_iCompletedOperations++;
    else
        m_iFailedOperations++;

    if (bDeleteSucess)
        m_iCompletedOperations++;
    else
        m_iFailedOperations++;

    return bCopySucess && bDeleteSucess;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::DeleteFileIn(const char* pSrcPath)
{
    if (!pSrcPath || !*pSrcPath)
        return false;

    bool bSuccess = DeleteFileA(pSrcPath);
    if (bSuccess)
        m_iCompletedOperations++;
    else
        m_iFailedOperations++;

    return bSuccess;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::DirExist(const char* pszDir)
{
    if (!pszDir || !*pszDir)
        return false;

    return _access(pszDir, 0) != -1;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::FileExist(const char* pszFile)
{
    if (!pszFile || !*pszFile)
        return false;

    bool bYes = (GetFileAttributesA(pszFile) != INVALID_FILE_ATTRIBUTES);
    if (!bYes)
        m_iSkippedOperations++;

    return bYes;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::DeleteEmptyFolder(const char* pszDir)
{
    if (!pszDir || !*pszDir)
        return false;

    WIN32_FIND_DATAA findData;
    char szSearchPath[MAX_PATH];
    V_sprintf_safe(szSearchPath, "%s\\*", pszDir);

    HANDLE hFind = FindFirstFileA(szSearchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE)
        return false;

    bool bIsEmpty = true;

    do
    {
        if (V_strcmp(findData.cFileName, ".") == 0 || V_strcmp(findData.cFileName, "..") == 0)
            continue;

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            char szSubDir[MAX_PATH];
            V_sprintf_safe(szSubDir, "%s\\%s", pszDir, findData.cFileName);
            if (!DeleteEmptyFolder(szSubDir))
                bIsEmpty = false; 
        }
        else
        {
            bIsEmpty = false;
        }

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

    if (bIsEmpty)
    {
        bool bSuccess = RemoveDirectoryA(pszDir) != 0;
        if (bSuccess)
            m_iCompletedOperations++;
        else
            m_iFailedOperations++;
        return bSuccess;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::CreateDir(const char* pszDir)
{
    if (!pszDir || !*pszDir)
        return false;

    if (this->DirExist(pszDir))
        return true;

    bool bSuccess = true;
    char* szPath = V_strdup(pszDir);
    V_FixSlashes(szPath);

    for (char* p = V_strchr(szPath, '\\') + 1; *p; p++)
    {
        if (*p == '\\')
        {
            *p = '\0';
            if (!CreateDirectoryA(szPath, NULL))
            {
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    this->GenerateErrorReport();
                    bSuccess = false;
                    m_iFailedOperations++;
                    break;
                }
            }
            *p = '\\';
        }
    }

    if (bSuccess)
    {
        if (!CreateDirectoryA(szPath, NULL))
        {
            if (GetLastError() != ERROR_ALREADY_EXISTS) 
            {
                bSuccess = true;
            }
            else 
            {
                this->GenerateErrorReport();
                bSuccess = false;
            }
        }
    }

    if (bSuccess)
        m_iCompletedOperations++;
    else
        m_iFailedOperations++;

    delete[] szPath;
    return bSuccess;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CResourceCopy::IsWritable(const char* pSrcDir, const char* pDstDir, const bool bIsPath = true)
{
    if ((!pSrcDir || !*pSrcDir) || (!pDstDir || !*pDstDir))
        return false;

    std::size_t uiSrcSize;
    if (bIsPath)
        uiSrcSize = this->GetFolderSize(pSrcDir);
    else
        uiSrcSize = this->GetFileSizeFast(pSrcDir);

    std::size_t uiFolderSizeDst = this->GetDriveFreeSpace(pDstDir);

    if (uiFolderSizeDst - uiSrcSize > 0)
        return true;
    else
        return false;
}


// TODO: Make a separate funtion for the FileList support!!
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::CopyDirTo(const char* pSrcDir, const char* pDstDir, const bool bMultiThread, const bool bOverwrite, const FileList* pCopyList)
{
    if ((!pSrcDir || !*pSrcDir) || (!pDstDir || !*pDstDir))
        return;

    if (m_eSpewMode != SpewMode::k_Quiet)
    {
        Msg("---- Operation: Copying ----\n");
        Msg("Source: ");      ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", pSrcDir);
        Msg("Destination: "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", pDstDir);
    }

    // Check if it is a wildcard
    char* pszSourceDir = V_strdup(pSrcDir);
    V_FixSlashes(pszSourceDir);

    {
        char* pszString = V_strrchr(pszSourceDir, '*');
        if (pszString)
        {
            *(pszString - 1) = '\0';
        }

        if (m_eSpewMode != SpewMode::k_Quiet)
            Msg("Wildcard: %s\n", pszString ? V_strrchr(pSrcDir, '*') : "None");
    }

    if (!this->DirExist(pszSourceDir))
    {
        ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "Folder: %s, doesnt exist\n", pszSourceDir);
        m_iSkippedOperations++;
        return;
    }

    if(!this->IsWritable(pszSourceDir, pDstDir, true))
    {
        char* pTemp = V_strdup(pDstDir);
        *(V_strchr(pTemp, ':') + 1) = '\0';
        Error("There is not enough free space on unit %s!\n", pTemp);
    }

    FileList DirAssetList;
    if (pCopyList)
        DirAssetList.insert(DirAssetList.begin(), pCopyList->begin(), pCopyList->end());
    else
        DirAssetList = this->ScanDirectoryRecursive(pSrcDir);

    if (DirAssetList.empty())
    {
        Warning("There are no files to copy!\n"
                "Skipping operation!\n");
        m_iSkippedOperations++;
    }

    const int k_iThreads = bMultiThread ? m_iThreads : 1;
    std::vector<std::future<void>> tasks;
    int iSourceDirLen = V_strlen(pszSourceDir);
    for (auto& File : DirAssetList)
    {
        char szDstPath[MAX_PATH];
        V_snprintf(szDstPath, sizeof(szDstPath), "%s\\%s", pDstDir, &File.data()[iSourceDirLen + 1]);

        {
            char* pTempString = V_strdup(szDstPath);
            *(V_strrchr(pTempString, '\\')) = '\0';
            if (!this->CreateDir(pTempString))
            {
                this->GenerateErrorReport();
                delete[] pTempString;
                continue;
            }
            delete[] pTempString;
        }

        tasks.emplace_back(std::async(std::launch::async, [this, File, szDstPath, bOverwrite](void)
            {
                if (this->CopyFileTo(File.data(), szDstPath, bOverwrite))
                {
                    std::lock_guard<std::mutex> lock(m_MsgMutex);
                    Msg("Coping file: "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s", File.data()); Msg(" -> "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s", szDstPath);
                    Msg("\n");
                }
                else
                {
                    std::lock_guard<std::mutex> lock(m_MsgMutex);
                    ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "Failed to copy file: %s -> %s\n", File.data(), szDstPath);
                }
            }
        ));

        // If we have reached the limit of concurrent threads, wait for the first to finish
        if (tasks.size() >= k_iThreads)
        {
            tasks.front().get();        // wait
            tasks.erase(tasks.begin()); // remove finished
        }
    }

    for (auto& t : tasks) 
    {
        t.get();
    }

    delete[] pszSourceDir;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::TransferDirTo(const char* pSrcDir, const char* pDstDir, const bool bMultiThread, const bool bDeleteMainFolder, const bool bOverwrite, const FileList* pDeleteList)
{
    this->CopyDirTo(pSrcDir, pDstDir, bMultiThread, bOverwrite);
    this->DeleteDirRecursive(pSrcDir, bMultiThread, bDeleteMainFolder);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::DeleteDirRecursive(const char* pDir, const bool bMultiThread, const bool bDeleteMainFolder, const FileList* pDeleteList)
{
    if (!pDir || !*pDir)
        return;

    if (m_eSpewMode != SpewMode::k_Quiet)
    {
        Msg("---- Operation: Deletion ----\n");
        Msg("Source: "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", pDir);
    }

    char* szDir = V_strdup(pDir);
    V_FixSlashes(szDir);

    char* wildcard = V_strrchr(szDir, '*');
    if (wildcard)
        *(wildcard - 1) = '\0';

    if (!this->DirExist(szDir))
    {
        ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "Folder: %s, doesnt exist\n", szDir);
        m_iSkippedOperations++;
        return;
    }

    const int k_iThreads = bMultiThread ? m_iThreads : 1;

    FileList DirAssetList;
    if (pDeleteList)
        DirAssetList.insert(DirAssetList.begin(), pDeleteList->begin(), pDeleteList->end());
    else
        DirAssetList = this->ScanDirectoryRecursive(pDir);

    if (DirAssetList.empty())
    {
        Warning("There are no files to delete!\n"
                "Skipping operation!\n");
        m_iSkippedOperations++;
    }

    std::vector<std::future<void>> tasks;
    for (auto& File : DirAssetList)
    {
        tasks.emplace_back(std::async(std::launch::async, [this, File](void) 
            {
                if (this->DeleteFileIn(File.data()))
                {
                    std::lock_guard<std::mutex> lock(m_MsgMutex);
                    Msg("Deleting file: ");
                    ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", File.data());
                }
                else
                {
                    std::lock_guard<std::mutex> lock(m_MsgMutex);
                    ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "Failed to delete file at: %s\n", File.data());
                }
            }
        ));

        // If we have reached the limit of concurrent threads, wait for the first to finish
        if (tasks.size() >= k_iThreads)
        {
            tasks.front().get();        // wait
            tasks.erase(tasks.begin()); // remove finished
        }
    }

    // wait for all to finish
    for (auto& t : tasks)
    {
        t.get();
    }

    if (bDeleteMainFolder) 
    {
        if (this->DeleteEmptyFolder(szDir))
        {
            Msg("Deleting folder: "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", szDir);
        }
        else
        {
            ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "Failed to delete folder: %s\n", szDir);
        }
    }
    delete[] szDir;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
std::size_t CResourceCopy::GetFileSizeFast(const char* pszFile)
{
    if (!pszFile || !*pszFile)
        return 0;

    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA(pszFile, GetFileExInfoStandard, &fad))
        return 0;

    return (static_cast<std::size_t>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
std::size_t CResourceCopy::GetDriveFreeSpace(const char* folderPath)
{
    if (!folderPath || !*folderPath)
        return NULL;

    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (GetDiskFreeSpaceExA(folderPath, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
    {
        std::size_t uiRet = freeBytesAvailable.QuadPart;
        return uiRet;
    }

    return NULL;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
std::size_t CResourceCopy::GetFolderSize(const char* inputPath)
{
    if (!inputPath || !*inputPath)
        return NULL;

    std::size_t uiFolderSize = 0;
    FileList AssetList = this->ScanDirectoryRecursive(inputPath);
    for (const auto& Ts : AssetList)
    {
        uiFolderSize += this->GetFileSizeFast(Ts.data());
    }

    return uiFolderSize;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::PrintDirContents(const char* pDir)
{
    if (!pDir || !*pDir)
        return;

    char* szDir = V_strdup(pDir);
    V_FixSlashes(szDir);

    Msg("---- Operation: Printing ----\n");
    Msg("Source: "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", szDir);
    const auto DirList = this->ScanDirectoryRecursive(szDir);

    if (DirList.empty())
    {
        Warning("There are no files to print!\n"
                "Skipping operation\n");
        m_iSkippedOperations++;
    }

    for (const auto& File : DirList)
    {
        Msg("File: "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", File.data());
        m_iCompletedOperations++;
    }

    delete[] szDir;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::GenerateGlobalOperationReport()
{
    Msg("\n-------------------------------------------------------------------------------------------\n");
    Msg("| ResourceCopy -> Done in %.2f | ",                                    Plat_FloatTime() - m_flTime);
    ColorSpewMessage(SPEW_MESSAGE, &ColorSucesfull,     "Completed: %i,     ",  m_iCompletedOperations);
    ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull,   "Error: %i,     ",      m_iFailedOperations);
    ColorSpewMessage(SPEW_MESSAGE, &ColorWarning,       "Skipped: %i         ", m_iSkippedOperations);
    Msg("\n-------------------------------------------------------------------------------------------\n");
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::GenerateHardwareReport(const char* pSrc, const char* pDst, const bool bIsPath)
{
    if ((!pSrc || !*pSrc) || (!pDst || !*pDst))
        return;

    char* pSrcCpy = V_strdup(pSrc);
    char* pDstCpy = V_strdup(pDst);

    V_FixSlashes(pSrcCpy);
    V_FixSlashes(pDstCpy);

    *(V_strchr(pSrcCpy, '\\') + 1) = '\0';
    *(V_strchr(pDstCpy, '\\') + 1) = '\0';

    Msg("---- Operation: Hardware report ----\n");
    Msg("Threads: %i\n", m_iThreads);
    Msg("Storing Unit (Source): %s\n", pSrcCpy);
    Msg("Storing Unit (Destination): %s\n", pDstCpy);

    if (bIsPath)
        Msg("Folder size (Source): %llu bytes\n", this->GetFolderSize(pSrc));
    else
        Msg("File size (Source): %llu bytes\n", this->GetFileSizeFast(pSrc));

    Msg("Storing Unit free memory (Destination): %llu bytes\n", this->GetDriveFreeSpace(pDst));
    Msg("\n");

    delete[] pSrcCpy;
    delete[] pDstCpy;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::GenerateErrorReport()
{
    DWORD err = GetLastError();
    char* msgBuf = nullptr;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&msgBuf, 0, NULL);

    ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "(Win32 API) Last Error code: %lu - %s\n", static_cast<unsigned long>(err), msgBuf ? msgBuf : "Unknown error");

    if (msgBuf)
        LocalFree(msgBuf);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::SetThreads(const int iThreads)
{
    // Sanity check!
    if (iThreads > m_iThreads)
        return;

    m_iThreads = iThreads;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::SetVerboseSpewMode()
{
    Msg("---- Spew Mode: Verbose ----\n");
    m_eSpewMode = SpewMode::k_Verbose;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::SetNormalSpewMode()
{
    Msg("---- Spew Mode: Normal ----\n");
    m_eSpewMode = SpewMode::k_Normal;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CResourceCopy::SetQuietSpewMode()
{
    Msg("---- Spew Mode: Quiet ----\n");
    m_eSpewMode = SpewMode::k_Quiet;
}

