//=== AddonBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ====//
//
// Purpose: AddonBuilder, A ContentBuilder subsystem for organizing and 
//          packaging game content for Steam Workshop release (mp maps).
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
#ifndef ADDONBUILDER_HPP
#define ADDONBUILDER_HPP

#ifdef _WIN32
#pragma once
#endif // _WIN32

#pragma warning(disable : 4238)

#include <KeyValues.h>
#include <colorschemetools.h>
#include <filesystem_init.h>
#include <consolelogger.hpp>
#include "contentbuilder.hpp"
#include "corebuilder.hpp"
#include "assetbuilder_map.hpp"

// TODO FIX THIS!!

// This is how AddonBuilder KeyValue looks like.
/*
    // This will only work if -addonbuild is enabled in contentbuilder
    AddonBuilder
    {
        // relative to maps/ folder
        LevelToPack     " "     // (e.g: template.bsp)

        FoldersToPack   
        {
            materials; models; sounds; soundscripts
        }

        // Always relative to the game dir!
        SingleFilesToPack
        {
            
        }

        BspZip
        {
            BuildParams     " "
        }
    }
*/


//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
#define KV_BUILDER_ADDON            "AddonBuilder"
#define KV_ADDON_LEVEL              "LevelToPack"
#define KV_ADDON_BSPZIP             "BspZip"
#define KV_ADDON_FOLDERS_PACK       "FoldersToPack"
#define KV_ADDON_FILES_PACK         "SingleFileToPack"
#define DIR_ADDONBUILDER_TEMP       "_addonbuild"
#define BUILDER_BSPZIP_TEMPDIR      "_temp_bspzip"
#define FILE_LISTASSET_TO_PACK      "assetlisttopack.txt"


//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
class CAddonBuilder : public CCoreBuilder
{
private:
    bool SetUpEnviroment(char* pCommandLine);
    const FileList CreateListOfAssetToBePacked(const FileList& FolderList, const FileList& ListFile);

public:
	CAddonBuilder();
	~CAddonBuilder();

	void AssetBuilderCompile() override;
};


//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
CAddonBuilder::CAddonBuilder() :
    CCoreBuilder(KV_BUILDER_ADDON,
        true,
        g_iThreads,
        TOOL_MAP_BSPZIP
    )
{
}


//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
CAddonBuilder::~CAddonBuilder()
{
}


//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
const FileList CAddonBuilder::CreateListOfAssetToBePacked(const FileList& FolderList, const FileList& ListFile)
{
    FileList AssetsToPack;

    // Generate the asset list of the assets that are inside the folders.
    {
        const float start = Plat_FloatTime();
        Msg("AssetSytem%s -> Creating asset list to be packed inside the .bsp file 1/2...", m_szKeyValue);

        for (const FileString& folder : FolderList)
        {
            char szTemp[MAX_PATH];
            V_sprintf_safe(szTemp, "%s\\%s\\*", CCoreBuilder::m_szGameDirPath, folder.data());
            if (!g_pResourceCopy->DirExist(szTemp))
            {
                g_pConsoleLogger->Warning("AssetSystem%s -> Skipping %s! It does not exist!\n", m_szKeyValue, szTemp);
                continue;
            }

            FileList DirAssets = g_pResourceCopy->ScanDirectoryRecursive(szTemp);
            AssetsToPack.insert(AssetsToPack.end(), DirAssets.begin(), DirAssets.end());
        }
        Msg("done(%.2f)\n", Plat_FloatTime() - start);
    }

    // Generate the file list of the assets that are inside the folders.
    {
        const float start = Plat_FloatTime();
        Msg("AssetSytem%s -> Creating asset list to be packed inside the .bsp file 2/2...", m_szKeyValue);

        for (const FileString& File : ListFile)
        {
            char szTemp[MAX_PATH];
            V_sprintf_safe(szTemp, "%s\\%s\\*", CCoreBuilder::m_szGameDirPath, File.data());
            if (!g_pResourceCopy->FileExist(szTemp))
            {
                g_pConsoleLogger->Warning("AssetSystem%s -> Skipping %s! It does not exist!\n", m_szKeyValue, szTemp);
                continue;
            }
            FileList Temp;
            Temp.push_back(File);

            AssetsToPack.insert(AssetsToPack.end(), Temp.begin(), Temp.end());
        }
        Msg("done(%.2f)\n", Plat_FloatTime() - start);
    }

    return AssetsToPack;
}


//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
bool CAddonBuilder::SetUpEnviroment(char* pCommandLine)
{
    float start = Plat_FloatTime();
    char szListOfFilesToPack[MAX_PATH];

    if (m_eSpewMode == SpewMode::k_Verbose) { Msg("AssetSystem%sVerbose -> Loading Keyvalues from: ", m_szKeyValue); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s", g_szContentBuilderScriptFile); Msg("...\n"); }

    KeyValues* pContentBuilderRead = new KeyValues("");

    if (!pContentBuilderRead->LoadFromFile(g_pFullFileSystem, g_szContentBuilderScriptFile))
    {
        g_pConsoleLogger->Warning("\nAssetSystem%s -> Failed to read file: %s\n"
            "AssetSystem%s -> Using default values for asset compile, this might not be ideal!\n",
            m_szKeyValue, g_szContentBuilderScriptFile, m_szKeyValue);
        return false;
    }

    KeyValues* pKv = pContentBuilderRead->FindKey(m_szKeyValue, false);
    if (!pKv)
    {
        g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
            "AssetSystem%s -> Using default values for asset compile, this might not be ideal!\n",
            m_szKeyValue, g_szContentBuilderScriptFile, m_szKeyValue);
        return false;
    }

    // Get the setup the cmd so we can start bspzip
    {
        // Get the bsp name to be packed
        const char* pLevelToBePack = pKv->GetString(KV_ADDON_LEVEL, NULL);
        if (!pLevelToBePack)
        {
            g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
                "AssetSystem%s -> Skipping addonbuilder!\n",
                KV_ADDON_LEVEL, g_szContentBuilderScriptFile, m_szKeyValue);
            return false;
        }

        V_sprintf_safe(szListOfFilesToPack, "%s\\%s\\%s\\%s", m_szGameDirPath, BUILDER_OUTDIR, DIR_ADDONBUILDER_TEMP, FILE_LISTASSET_TO_PACK);

        const char* pBuildParms = pKv->GetString(KV_BUILDPARAMS, NULL);
        if (pBuildParms)
        {
            char szSourceBspFile[MAX_PATH];
            V_sprintf_safe(szSourceBspFile, "%s\\%s\\%s", m_szGameDirPath, DIR_MAPS, pLevelToBePack);

            char szPackedBspFile[MAX_PATH];
            V_sprintf_safe(szPackedBspFile, "%s\\%s\\%s\\%s", m_szGameDirPath, BUILDER_OUTDIR, DIR_ADDONBUILDER_TEMP, pLevelToBePack);

            // bspzip - addlist <bspfile> <listfile> <newbspfile>
            V_snprintf(pCommandLine, MAX_CMD_BUFFER_SIZE, "%s\\%s  %s -addfiles %s %s %s", m_szGameDirToolsPath, TOOL_MAP_BSPZIP, pBuildParms, szSourceBspFile, szListOfFilesToPack, szPackedBspFile);
        }
        else
        {
            g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
                "AssetSystem%s -> Using default values for asset compile, this might not be ideal!\n",
                m_szKeyValue, KV_BUILDPARAMS, g_szContentBuilderScriptFile, m_szKeyValue);
        }
    }

    // Make The list of things to pack
    {
        // Get the folder list to be added in the .bsp 
        FileList ListFoler;
        {
            KeyValues* pFolderKv = pKv->FindKey(KV_ADDON_FOLDERS_PACK);
            if (!pKv)
            {
                g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
                                          "AssetSystem%s -> Skipping addon packer!\n",
                    m_szKeyValue, KV_ADDON_FOLDERS_PACK, g_szContentBuilderScriptFile, m_szKeyValue);
                return false;
            }

            for (KeyValues* Kv = pFolderKv->GetFirstSubKey(); Kv; Kv = Kv->GetNextKey())
            {
                const char* pFolder = Kv->GetString();
                FileString Temp;
                V_strncpy(Temp.data(), pFolder, Temp.size());
                ListFoler.push_back(Temp);
            }
        }

        // Get the file list to be added in the .bsp 
        FileList ListFile;
        {
            KeyValues* pFileKv = pKv->FindKey(KV_ADDON_FILES_PACK);
            if (!pKv)
            {
                g_pConsoleLogger->Warning("\nAssetSystem%s -> Could not get \'%s\' KeyValues from \"%s\"!\n"
                    "AssetSystem%s -> Skipping addon packer!\n",
                    m_szKeyValue, KV_ADDON_FILES_PACK, g_szContentBuilderScriptFile, m_szKeyValue);
                return false;
            }

            for (KeyValues* Kv = pFileKv->GetFirstSubKey(); Kv; Kv = Kv->GetNextKey())
            {
                const char* pFile = Kv->GetString();
                FileString Temp;
                V_strncpy(Temp.data(), pFile, Temp.size());
                ListFile.push_back(Temp);
            }
        }

        const FileList TempList = CreateListOfAssetToBePacked(ListFoler, ListFile);

        // Generate the .txt file contain all of the asset to pack... I am tired boss...
        FILE* fp = fopen(szListOfFilesToPack, "w");
        if (!szListOfFilesToPack) 
        {
            g_pConsoleLogger->Warning("AssetSystem%s -> Could not create file at: %s!\n"
                                     "AssetSystem%s -> Skipping packing!!\n", 
                    m_szKeyValue, szListOfFilesToPack, m_szKeyValue);
            return false;
        }

        fprintf(fp, "// THIS FILE IS AUTO-GENERATED BY CONTENTBUILDER");
        for (const FileString& File : TempList)
        {
            fprintf(fp, "%s", File.data());
        }
        fclose(fp);
    }

    pContentBuilderRead->deleteThis();
    if (m_eSpewMode == SpewMode::k_Verbose) { Msg("done(%.2f)\n", Plat_FloatTime() - start); }

    return true;
}


// TODO: overwrite GenerateAssetReport!
//-----------------------------------------------------------------------------
// Purpose:	
//-----------------------------------------------------------------------------
void CAddonBuilder::AssetBuilderCompile()
{
    CCoreBuilder::PrintHeaderCompileType();

    if (!CCoreBuilder::m_bRunAssetCompile)
        return;

    char szAddonbuilderDir[MAX_PATH];
    V_sprintf_safe(szAddonbuilderDir, "%s\\%s\\%s", m_szGameDirPath, BUILDER_OUTDIR, DIR_ADDONBUILDER_TEMP);
    if (!g_pResourceCopy->CreateDir(szAddonbuilderDir))
    {
        g_pConsoleLogger->Warning("AssetSystem%s -> Could not create dir at: %s\n", m_szKeyValue, szAddonbuilderDir);
        return;
    }

    char szCommandLineBspZip[MAX_CMD_BUFFER_SIZE];
    if(!SetUpEnviroment(szCommandLineBspZip))
        return;
    
    CCoreBuilder::StartExe(szCommandLineBspZip);
}


#endif // ADDONBUILDER_HPP

