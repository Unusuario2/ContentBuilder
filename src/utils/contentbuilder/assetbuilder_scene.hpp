//==== SceneBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ===//
//
// Purpose: SceneBuilder. A ContentBuilder subsystem for scene compile.
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
#ifndef ASSETBUILDER_SCENE_HPP
#define ASSETBUILDER_SCENE_HPP

#ifdef _WIN32
#pragma once
#endif // _WIN32

#pragma warning(disable : 4238)

#include <filesystem_init.h>
#include <colorschemetools.h>
#include <pipeline_shareddefs.h>
#include "corebuilder.hpp"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
#define KV_BUILDER_SCENE           "SceneBuilder"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CSceneBuilder : public CCoreBuilder
{
private:
	bool m_bRunAssetCompile = true;

public:
	inline CSceneBuilder();
	inline ~CSceneBuilder();

	inline void AssetBuilderCompile() override;
	inline void DeleteCompiledContents() override;
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CSceneBuilder::CSceneBuilder() :
	CCoreBuilder(
				KV_BUILDER_SCENE,
				DIR_SCENESRC,
				DIR_SCENES,
				true,
			    g_bContentDstDelete,
				g_iThreads,
				TOOL_SCENE,
				FileList{ FileString { EXT_SCENESRC } },
				EXT_SCENE
	)
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CSceneBuilder::~CSceneBuilder()
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CSceneBuilder::AssetBuilderCompile()
{
	bool bBuild = false;
	CCoreBuilder::PrintHeaderCompileType();

	if (!CCoreBuilder::m_bRunAssetCompile)
		return;

	CSceneBuilder::DeleteCompiledContents();

	if (!g_pResourceCopy->CreateDir(CCoreBuilder::m_szGameAssetDstPath))
		g_pConsoleLogger->Warning("AssetSystem%s -> Failed to creating folder at: %s", CCoreBuilder::m_szKeyValue, CCoreBuilder::m_szGameAssetDstPath);

	// Check if need to build any files
	char szSceneCompiledFile[MAX_PATH];
	V_sprintf_safe(szSceneCompiledFile, "%s\\%s", CCoreBuilder::m_szGameAssetSrcPath, FILE_SCENE);

	char szSceneWildCard[MAX_PATH];
	V_sprintf_safe(szSceneWildCard, "%s\\*%s", CCoreBuilder::m_szGameAssetSrcPath, EXT_SCENESRC);

	FileList SceneFiles = g_pResourceCopy->ScanDirectoryRecursive(szSceneWildCard);
	if (!SceneFiles.empty())
	{
		if (g_pResourceCopy->FileExist(szSceneCompiledFile))
		{
			for (const auto& File : SceneFiles)
			{
				if (g_pFullFileSystem->GetFileTime(szSceneCompiledFile) < g_pFullFileSystem->GetFileTime(File.data()))
				{
					bBuild = true;
					break;
				}
			}
		}
		else
		{
			bBuild = true;
		}
	}
	else
	{
		bBuild = false;
	}

	if (bBuild)
	{
		char szCommandLine[MAX_CMD_BUFFER_SIZE];
		CCoreBuilder::LoadGameInfoKv(szCommandLine);

		char szFullCommandLine[MAX_CMD_BUFFER_SIZE];
		V_sprintf_safe(szFullCommandLine, "%s\\%s %s -game \"%s\"", CCoreBuilder::m_szGameDirToolsPath, TOOL_SCENE, szCommandLine, CCoreBuilder::m_szGameDirPath);
		CCoreBuilder::StartExe(szFullCommandLine, nullptr);
	}
	else
	{
		Msg("AssetSystem%s-> No Files to build!\n", CCoreBuilder::m_szKeyValue);
	}

	CCoreBuilder::GenerateGlobalOperationReport();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CSceneBuilder::DeleteCompiledContents()
{
	CCoreBuilder::DeleteCompiledContents();
}


#endif // ASSETBUILDER_SCENE_HPP

