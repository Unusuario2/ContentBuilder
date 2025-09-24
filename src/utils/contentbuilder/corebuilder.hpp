//==== CCoreBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ===//
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
#ifndef CCOREBUILDER_HPP
#define CCOREBUILDER_HPP

#ifdef _WIN32
#pragma once
#endif // _WIN32

#pragma warning(disable : 4238)

#include <platform.h>
#include <array>
#include <vector>
#include <mutex>
#include <resourcecopy/cresourcecopy.hpp>
#include <consolelogger.hpp>


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CCoreBuilder
{
private:
	uint			m_uiThreads				= 1;
	bool			m_bForceBuildContent	= true;
	bool			m_bPrintAppToTheConsole = false;

protected:
	float			m_flStartTime			= 0;
	uint			m_uiCompletedProcess	= 0;
	uint			m_uiSkippedProcess		= 0;
	uint			m_uiFailedProcess		= 0;
	ContainerList*	m_pVAssetCompileList	= nullptr;	// Do we really use this??
	ContainerList*	m_rgpVSrcExtensions		= nullptr;	// List of all of 
	bool			m_bRunAssetCompile		= true;		// Do we run the builder?
	bool			m_bDeleteCompiledAssets	= false;	
	std::mutex		m_MsgMtxLock;

public:
	SpewMode		m_eSpewMode				= SpewMode::k_Normal;
	const char*		m_szToolName			= nullptr;	// (e.g: vtex.exe)
	const char*		m_szGameAssetSrcPath	= nullptr;  // Source of the folder  (e.g: $SteamDir/Half-Life 2/hl2/materialsrc)
	const char*		m_szGameAssetDstPath	= nullptr;  // Destiantion of the folder (e.g: $SteamDir/Half-Life 2/hl2/materials)
	const char*		m_szFolderSrc			= nullptr;	// Source of the folder inside m_szGameDirPath (e.g: materialsrc)
	const char*		m_szFolderDst			= nullptr;  // Destiantion of the folder inside m_szGameDirPath (e.g: materials)
	const char*		m_szKeyValue			= nullptr;	// Name of the KeyValue of the subcompilesystem (e.g: Kv-> MaterialBuilder)
	const char*		m_szGameDirPath			= nullptr;	// (e.g: $SteamDir/Half-Life 2/hl2)
	const char*		m_szGameDirToolsPath	= nullptr;	// (e.g: $SteamDir/Half-Life 2/bin)
	const char*		m_szCompiledExtension	= nullptr;	// (e.g: .vtf)

protected:
	void StartExe(char* pFullCommand, const char* pFileName = nullptr);
	void PrintHeaderCompileType();
	void LoadGameInfoKv(char* pCommandLine);
	void GenerateGlobalOperationReport();
	const char* TimeStamp();
	FileList GenerateBuildingListAssets(const char* pForceExtension = nullptr);
	FileListExtended GenerateFullBuildingList(FileList* pVListFiles = nullptr);

public:
	CCoreBuilder(const char* pKeyValue, const char* pFolderNameSrc, const char* pFolderNameDst, const bool bPrintAppToTheConsole, const bool bDeleteCompiledAssets, uint uiThreads, const char* pToolNames, FileList pExtensions, const char* pCompiledExtension);
	CCoreBuilder(const char* pKeyValue, const bool bPrintAppToTheConsole, const bool bDeleteCompiledAssets, uint uiThreads, const char* pToolNames);
	virtual ~CCoreBuilder();

	void AssetToolCheck();

	virtual void AssetBuilderCompile();
	virtual void GenerateAssetReport();
	
	virtual void DeleteCompiledContents();
};


#endif // CCOREBUILDER_HPP

