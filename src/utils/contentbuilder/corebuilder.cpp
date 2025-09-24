//==== CCoreBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ===//
//
// Purpose: Core of contentbuilder buildsystem.
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
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <vector>
#include <array>
#include <future>
#include <mutex>

#include <filesystem_init.h>
#include <KeyValues.h>
#include <cmdlib.h>

#include <colorschemetools.h>
#include <pipeline_shareddefs.h>
#include <resourcecopy/cresourcecopy.hpp>
#include "corebuilder.hpp"
#include "contentbuilder.hpp"

#pragma warning(disable : 4238)


//-----------------------------------------------------------------------------
// Purpose:   
//-----------------------------------------------------------------------------
CCoreBuilder::CCoreBuilder(const char* pKeyValue, const char* pFolderNameSrc,
    const char* pFolderNameDst, const bool bPrintAppToTheConsole, const bool bDeleteCompiledAssets,
    uint uiThreads, const char* pToolNames, FileList pSrcExtensions, const char* pCompiledExtension)
{
    m_flStartTime           = Plat_FloatTime();

    m_rgpVSrcExtensions     = new FileList();
    m_pVAssetCompileList    = new FileList();
    
    m_szKeyValue            = V_strdup(pKeyValue);
    m_szGameDirPath         = V_strdup(gamedir);
    m_szGameDirToolsPath    = V_strdup(g_szGameBin);
    m_szCompiledExtension   = V_strdup(pCompiledExtension);
    m_szToolName            = V_strdup(pToolNames);
    m_szFolderSrc           = V_strdup(pFolderNameSrc);
    m_szFolderDst           = V_strdup(pFolderNameDst);
    m_uiThreads             = uiThreads;
    m_bDeleteCompiledAssets = bDeleteCompiledAssets;
    m_bPrintAppToTheConsole = bPrintAppToTheConsole;
    m_bForceBuildContent    = g_bForceBuildContent;
    m_eSpewMode             = g_eSpewMode;
        
    for (FileString& pSc : pSrcExtensions)
        m_rgpVSrcExtensions->push_back(pSc);

    {
        char szTemp1[MAX_PATH];
        V_sprintf_safe(szTemp1, "%s\\%s", m_szGameDirPath, m_szFolderSrc);
        m_szGameAssetSrcPath = V_strdup(szTemp1);
    }
 
    {
        char szTemp2[MAX_PATH];
        V_sprintf_safe(szTemp2, "%s\\%s", m_szGameDirPath, m_szFolderDst);
        m_szGameAssetDstPath = V_strdup(szTemp2);
    }
}


//-----------------------------------------------------------------------------
// Purpose:   
//-----------------------------------------------------------------------------
CCoreBuilder::CCoreBuilder(const char* pKeyValue, const bool bPrintAppToTheConsole, const bool bDeleteCompiledAssets, uint uiThreads, const char* pToolNames)
{
    m_flStartTime           = Plat_FloatTime();
    m_rgpVSrcExtensions     = new FileList();
    m_pVAssetCompileList    = new FileList();

    m_szKeyValue            = V_strdup(pKeyValue);
    m_szGameDirPath         = V_strdup(gamedir);
    m_szGameDirToolsPath    = V_strdup(g_szGameBin);
    m_szToolName            = V_strdup(pToolNames);
    m_uiThreads             = uiThreads;
    m_bPrintAppToTheConsole = bPrintAppToTheConsole;
    m_bDeleteCompiledAssets = bDeleteCompiledAssets;
    m_bForceBuildContent    = g_bForceBuildContent;
    m_eSpewMode             = g_eSpewMode;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CCoreBuilder::~CCoreBuilder()
{
    m_rgpVSrcExtensions->clear();
    m_pVAssetCompileList->clear();

    delete[] m_szKeyValue;
    delete[] m_szGameDirToolsPath;
    delete[] m_szGameDirPath;
    delete[] m_szToolName;

    if (m_szCompiledExtension)
        delete[] m_szCompiledExtension;
    if(m_szFolderSrc)
        delete[] m_szFolderSrc;
    if (m_szFolderDst)
        delete[] m_szFolderDst;
    if (m_szGameAssetSrcPath)
        delete[] m_szGameAssetSrcPath;
    if (m_szGameAssetDstPath)
        delete[] m_szGameAssetDstPath;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCoreBuilder::GenerateGlobalOperationReport()
{
    Msg("\n");
    Msg("All operations of %s finished!\n", m_szKeyValue);
    Msg("Generating Global Operation Report:");
    Msg("\n");

    Msg("-------------------------------------------------------------------------------------------\n");
    Msg("| %s -> Done in %s | ", m_szKeyValue, TimeStamp());
    ColorSpewMessage(SPEW_MESSAGE, &ColorSucesfull,     "Completed: %i,     ", m_uiCompletedProcess);
    ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull,   "Error: %i,     ", m_uiFailedProcess);
    ColorSpewMessage(SPEW_MESSAGE, &ColorWarning,       "Skipped: %i         ", m_uiSkippedProcess);
    Msg("\n");
    Msg("-------------------------------------------------------------------------------------------\n");
    Msg("\n");
}


//-----------------------------------------------------------------------------
// Purpose:   Check if the tool exists
//-----------------------------------------------------------------------------
void CCoreBuilder::AssetToolCheck()
{
    char szTemp[MAX_PATH];
    V_sprintf_safe(szTemp, "%s\\%s", m_szGameDirToolsPath, m_szToolName);

    if (!g_pResourceCopy->FileExist(szTemp))
    {
        g_pConsoleLogger->Warning("AssetSystem%s -> Skipping compile! We could not find %s!\n", m_szKeyValue, m_szToolName);
        m_bRunAssetCompile = false;
    }
}


//-----------------------------------------------------------------------------
// Purpose: Loads specific tools KeyValues, we asume that we are inside ContentBuilder KV
//-----------------------------------------------------------------------------
void CCoreBuilder::LoadGameInfoKv(char* pCommandLine)
{
    const float start = Plat_FloatTime();
    char szArgv[MAX_CMD_BUFFER_SIZE];

    if (m_eSpewMode == SpewMode::k_Verbose) { Msg("AssetSystem%sVerbose -> Loading Keyvalues from: ", m_szKeyValue); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s", g_szContentBuilderScriptFile); Msg("...\n"); }

    KeyValues* pContentBuilderRead = new KeyValues("");

    // TODO: MOVE ALL THE PATHS TO BE INSIDE THE CLASS!!
    if (!pContentBuilderRead->LoadFromFile(g_pFullFileSystem, g_szContentBuilderScriptFile))
    {
        g_pConsoleLogger->Warning("\nAssetSystem%s -> Failed to read file: %s\n"
            "AssetSystem%s -> Using default values for asset compile, this might not be ideal!\n",
            m_szKeyValue, g_szContentBuilderScriptFile, m_szKeyValue);
        return;
    }

    KeyValues* pKv = pContentBuilderRead->FindKey(m_szKeyValue, false);
    if (!pKv)
    {
        g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
            "AssetSystem%s -> Using default values for asset compile, this might not be ideal!\n",
            m_szKeyValue, g_szContentBuilderScriptFile, m_szKeyValue);
        return;
    }

    const char* pArgvTool = pKv->GetString(KV_BUILDPARAMS, NULL);
    if (pArgvTool)
    {
        V_sprintf_safe(szArgv, " %s ", pArgvTool);
    }
    else
    {
        g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
            "AssetSystem%s -> Using default values for asset compile, this might not be ideal!\n",
            m_szKeyValue, KV_BUILDPARAMS, m_szKeyValue, m_szKeyValue);
        return;
    }
    
    V_strncpy(pCommandLine, szArgv, MAX_CMD_BUFFER_SIZE);
    if (m_eSpewMode == SpewMode::k_Verbose) { Msg("done(%.2f)\n", Plat_FloatTime() - start); }
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
FileList CCoreBuilder::GenerateBuildingListAssets(const char* pForceExtension)
{
    // Scan the dir and generate a list with all the files.
    FileList VFileList;
    {
        const float start = Plat_FloatTime();
        if (m_eSpewMode != SpewMode::k_Quiet) { Msg("AssetSystem%s -> Creating building list... ", m_szKeyValue); }

        int len;
        if (pForceExtension)
            len = 1;
        else
            len = m_rgpVSrcExtensions->size();

        for (int i = 0; i < len; ++i)
        {
            char szTempExt[MAX_PATH];
            V_sprintf_safe(szTempExt, "%s\\%s\\*%s", m_szGameDirPath, m_szFolderSrc, 
                [&pForceExtension, this, i]() -> const char*
                {
                    if (pForceExtension)
                        return pForceExtension;
                    else
                        return (*m_rgpVSrcExtensions)[i].data();
                }()
            );

            FileList Temp = g_pResourceCopy->ScanDirectoryRecursive(szTempExt);
            VFileList.insert(VFileList.end(), Temp.begin(), Temp.end());
        }

        if (m_eSpewMode != SpewMode::k_Quiet) { Msg("done(%.2f)\n", Plat_FloatTime() - start); }
    }

    // We check what NOT to build, these are found inside Exclude { ExcludeFileOrDir "File" } KV int contentbuilder.txt
    {
        const float start = Plat_FloatTime();
        if (m_eSpewMode != SpewMode::k_Quiet) { Msg("AssetSystem%s -> Excluding files from building list 1/2...", m_szKeyValue); }

        KeyValues* pContenBuilder = new KeyValues("");
        if (!pContenBuilder->LoadFromFile(g_pFileSystem, g_szContentBuilderScriptFile))
        {
            g_pConsoleLogger->Warning("\nAssetSystem%s -> Failed to read file: %s\n"
                "AssetSystem%s -> We cannot locate what files to skip!\n",
                m_szKeyValue, g_szContentBuilderScriptFile, m_szKeyValue);
        }
        else
        {
            KeyValues* pKvExclude = pContenBuilder->FindKey(KV_EXCLUDE, NULL);
            int iGameDirlen = V_strlen(m_szGameDirPath);

            for (KeyValues* pKey = pKvExclude->GetFirstSubKey(); pKey; pKey = pKey->GetNextValue())
            {
                const char* pString = pKey->GetString();
                if (pString)
                {
                    for (int i = 0; i < VFileList.size(); i++)
                    {
                        if (V_strstr(&VFileList[i].data()[iGameDirlen + 1], pString))
                        {
                            VFileList.erase(VFileList.begin() + i);
                            m_uiSkippedProcess++;
                            i--;
                        }
                    }
                }
            }
            if (m_eSpewMode != SpewMode::k_Quiet) { Msg(" done(%.2f)\n", Plat_FloatTime() - start); }
        }
    }

    // Know compare remove all the assets that are excluded to be build & the time stamp!!
    if (!m_bForceBuildContent)
    {
        const float start = Plat_FloatTime();
        if (m_eSpewMode != SpewMode::k_Quiet) { Msg("AssetSystem%s -> Excluding files from building list 2/2...", m_szKeyValue); }

        {
            int iStrlenSrc = V_strlen(m_szGameAssetSrcPath) + 1;
            for (int i = 0; i < VFileList.size(); i++)
            {
                char szDstFile[MAX_PATH];
                {
                    char szRelativeFile[MAX_PATH];
                    V_strcpy_safe(szRelativeFile, &VFileList[i].data()[iStrlenSrc]);
                    *(V_strrchr(szRelativeFile, '.')) = '\0';
                    V_sprintf_safe(szDstFile, "%s\\%s%s", m_szGameAssetDstPath, szRelativeFile, m_szCompiledExtension);
                }

                if (g_pResourceCopy->FileExist(szDstFile))
                {
                    if ((g_pFullFileSystem->GetFileTime(szDstFile) - g_pFullFileSystem->GetFileTime(VFileList[i].data())) <= 0)
                    {
                        VFileList.erase(VFileList.begin() + i);
                        m_uiSkippedProcess++;
                        i--;
                    }
                }
            }
        }
        if (m_eSpewMode != SpewMode::k_Quiet) { Msg(" done(%.2f)\n", Plat_FloatTime() - start); }
    }

    return VFileList;
}


//-----------------------------------------------------------------------------
// Purpose: Given a dir and a extension it will generate a list of all the assets
//          found in the dir. (e,g: c:\testfile.vmf)
//-----------------------------------------------------------------------------
FileListExtended CCoreBuilder::GenerateFullBuildingList(FileList* pVListFiles)
{
    // TODO: if the user has a lot of files to exclude it may be better to change the use of std::vector for std::deque!
    float _start = Plat_FloatTime();

    FileList VFileList = GenerateBuildingListAssets();

    // Pass the raw asset list (only paths to assets without the commnad line)
    if (pVListFiles)
        pVListFiles->insert(pVListFiles->begin(), VFileList.begin(), VFileList.end());

    // We have know all the files to be build, we want to add the respective command line to the files
    FileListExtended VCommandsToExec;
    {
        const float start = Plat_FloatTime();
        if (m_eSpewMode != SpewMode::k_Quiet) { Msg("AssetSystem%s -> Adding additional command lines to the building list... ", m_szKeyValue); }

        char szCommandLine[MAX_CMD_BUFFER_SIZE];
        char szFullCommandLine[MAX_CMD_BUFFER_SIZE];
        LoadGameInfoKv(szCommandLine);
        for (FileString& fString : VFileList)
        {
            // Format of the string: (executate name, command line params, file) (e.g: "C:/test/vbsp.exe -noprude -game C:/test C:/test/mapsrc/.vmf)
            V_sprintf_safe(szFullCommandLine, "%s\\%s %s -game \"%s\" \"%s\"", m_szGameDirToolsPath, m_szToolName, szCommandLine, m_szGameDirPath, fString.data());

            FileStringExtended Temp;
            V_strcpy(Temp.data(), szFullCommandLine);
            VCommandsToExec.push_back(Temp);
        }
        if (m_eSpewMode != SpewMode::k_Quiet) { Msg("done(%.2f)\n", Plat_FloatTime() - start); }
    }

    Msg("AssetSystem%s -> Finalizing build list... done(%.2f)\n", m_szKeyValue, Plat_FloatTime() - _start);
    Msg("AssetSystem%s -> Tasks to execute: %llu\n", m_szKeyValue, VCommandsToExec.size());

    return VCommandsToExec;
}


//----------------------------------------------------------------------------
// Purpose: Starts .exe tools
//----------------------------------------------------------------------------
void CCoreBuilder::StartExe(char* pFullCommand, const char* pFileName)
{
    const float start = Plat_FloatTime();

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    DWORD dwFlags = 0;
    HANDLE hNull = NULL;

    // TODO: UNDO this, this right now is shitty af.
    if (!m_bPrintAppToTheConsole)
    {
        // Tell Windows we are overriding handles + window behavior
        si.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.wShowWindow = SW_HIDE;

        // Redirect to NULL
        hNull = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hNull != INVALID_HANDLE_VALUE)
        {
            si.hStdOutput = hNull;
            si.hStdError = hNull;
        }
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

        // Dont create a console window
        dwFlags |= CREATE_NO_WINDOW;
    }

    // Create process
    if (!CreateProcessA(NULL, pFullCommand, NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi))
    {
        g_pConsoleLogger->Warning("AssetTools%s -> %s could not start!\n", m_szKeyValue, pFullCommand);
        m_uiFailedProcess++;
        if (hNull) 
            CloseHandle(hNull);
        return;
    }

    // Wait until child process exits
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Retrieve exit code
    DWORD exitCode = 0;
    if (GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        if (exitCode != 0)
        {
            std::lock_guard<std::mutex> lock(m_MsgMtxLock);

            if(pFileName)
                ColorSpewMessage(SPEW_MESSAGE, &ColorUnSucesfull, "FAILED - %s - %s\n", TimeStamp(), pFileName);
            
            g_pConsoleLogger->Warning("AssetTools -> %s compile failed: %d!\n", pFullCommand, exitCode);
            m_uiFailedProcess++;
        }
        else
        {
            std::lock_guard<std::mutex> lock(m_MsgMtxLock);
            if (pFileName) 
            {
                ColorSpewMessage(SPEW_MESSAGE, &ColorSucesfull, "OK");
                Msg(" - %s - ", TimeStamp());
                ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", pFileName);
            }
            m_uiCompletedProcess++;
        }
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_MsgMtxLock);
        g_pConsoleLogger->Warning("AssetTools%s -> GetExitCodeProcess() failed!\n", m_szKeyValue);
        m_uiFailedProcess++;
    }

    // Close process handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (hNull) 
        CloseHandle(hNull);
}


//-----------------------------------------------------------------------------
// Purpose: Process header ident
//-----------------------------------------------------------------------------
void CCoreBuilder::PrintHeaderCompileType()
{
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "\n==== %s ====\n", m_szKeyValue);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char* CCoreBuilder::TimeStamp()
{
    float actualTime = Plat_FloatTime();
    float deltaTime = actualTime - m_flStartTime;

    if (deltaTime < 0) deltaTime = 0;

    std::size_t totalSecs = static_cast<std::size_t>(deltaTime);
    std::size_t hours = totalSecs / 3600;
    std::size_t mins = (totalSecs % 3600) / 60;
    std::size_t secs = totalSecs % 60;

    static char s_szTime[128];
    V_sprintf_safe(s_szTime, "%02lluh:%02llum:%02llus", static_cast<unsigned long long>(hours), static_cast<unsigned long long>(mins), static_cast<unsigned long long>(secs));

    return s_szTime;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCoreBuilder::AssetBuilderCompile()
{
    const float start = Plat_FloatTime();

    Msg("AssetSystem%s -> Starting preparation...\n", m_szKeyValue);
    if (m_szFolderDst)
    {
        char szGameDstPath[MAX_PATH];
        V_sprintf_safe(szGameDstPath, "%s\\%s", m_szGameDirPath, m_szFolderDst);
        if (!g_pResourceCopy->CreateDir(szGameDstPath))
        {
            g_pConsoleLogger->Warning("AssetSystem%s -> Failed to creating folder at: %s", m_szKeyValue, szGameDstPath);
        }
        else if (m_eSpewMode == SpewMode::k_Verbose)
        {
            Msg("AssetSystem%s -> Creating folder at: ", m_szKeyValue); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s", szGameDstPath);
        }
    }

    FileListExtended VCommandsToExec = GenerateFullBuildingList(m_pVAssetCompileList);
    Msg("AssetSystem%s -> Preparation done in %.2f seconds.\n",m_szKeyValue, Plat_FloatTime() - start);
    Msg("AssetSystem%s -> Starting compile with %i threads.\n", m_szKeyValue, m_uiThreads);
    Msg("\n");

    // Now that we have the list to build, we execute the start the process
    std::vector<std::future<void>> tasks;
    int len = VCommandsToExec.size();
    for (int i = 0; i < len; i++)
    {
        tasks.emplace_back(std::async(std::launch::async,
            [this, &i, VCommandsToExec]() -> void
            {
                // TODO, change this!! this is so fucking retared! All becouse of stupid win32 API
                char* pTemp = V_strdup(VCommandsToExec[i].data());
                StartExe(pTemp,
                    [this, i]() -> const char*
                    {
                        if (m_bPrintAppToTheConsole)
                            return nullptr;
                        else
                            return (*m_pVAssetCompileList)[i].data();
                    }()
                        );
                delete[] pTemp;
            }
        ));

        // If we have reached the limit of concurrent threads, wait for the first to finish
        if (tasks.size() >= m_uiThreads)
        {
            tasks.front().get();        // wait
            tasks.erase(tasks.begin()); // remove finished task
        }
    }

    // Wait until all the process are done
    for (std::future<void>& f : tasks)
        f.get();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCoreBuilder::GenerateAssetReport()
{
    // TODO: Change this! we run this two times!
    FileList VFileList = GenerateBuildingListAssets();

    char szBuilderSubpath[MAX_PATH];
    {
        char* pTemp = V_strdup(m_szKeyValue);
        V_sprintf_safe(szBuilderSubpath, "%s\\%s\\assetreport%s.txt", m_szGameDirPath, BUILDER_OUTDIR, strlwr(pTemp));
        delete[] pTemp;
    }

    FILE* fp = fopen(szBuilderSubpath, "w");
    if (!fp)
    {
        g_pConsoleLogger->Warning("AssetSystem%s -> Could not create: %s!\n"
            "AssetSystem%s -> Skipping saving the output of the console!\n",
            m_szKeyValue, szBuilderSubpath, m_szKeyValue);
        return;
    }

    for (auto File : VFileList)
        fprintf(fp, "%s\n", File.data());

    fclose(fp);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCoreBuilder::DeleteCompiledContents()
{
    if (m_bDeleteCompiledAssets)
        return;

    Msg("AssetSystem%s -> Deleting compiled files\n", m_szKeyValue);

    char szWildCard[MAX_PATH];
    V_sprintf_safe(szWildCard, "%s\\*%s", CCoreBuilder::m_szGameAssetDstPath, CCoreBuilder::m_szCompiledExtension);
    g_pResourceCopy->DeleteDirRecursive(szWildCard);
    Msg("\n");
}

