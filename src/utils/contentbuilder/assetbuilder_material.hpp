//=== MaterialBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ==//
//
// Purpose: MaterialBuilder, A ContentBuilder subsystem for texture batch compiling.
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
#ifndef ASSETBUILDER_MATERIAL_HPP
#define ASSETBUILDER_MATERIAL_HPP

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
#define KV_BUILDER_MATERIAL        "MaterialBuilder"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CMaterialBuilder : public CCoreBuilder
{
private:
	inline void CopySrcVmtToGameDir();

public:
	inline CMaterialBuilder();
	inline ~CMaterialBuilder();

	inline void AssetBuilderCompile() override;
	inline void DeleteCompiledContents() override;
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CMaterialBuilder::CMaterialBuilder() :
	CCoreBuilder(
				KV_BUILDER_MATERIAL,
				DIR_MATERIALSRC,
				DIR_MATERIALS,
				false,
				g_bContentDstDelete,
				g_iThreads,
				TOOL_MATERIAL,
				FileList{ FileString { EXT_TEXTURESRC_PFM },
						  FileString { EXT_TEXTURESRC_PSD },
						  FileString { EXT_TEXTURESRC_TGA }},
				EXT_TEXTURE 
	)
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CMaterialBuilder::~CMaterialBuilder()
{
}


//-----------------------------------------------------------------------------
// Purpose: Copy all the .vmt of game/mod/materialsrc to game/mod/materials folder
//-----------------------------------------------------------------------------
inline void CMaterialBuilder::CopySrcVmtToGameDir()
{
	Msg("AssetSystemMaterialBuilder -> Copying .vmt files from "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s", CCoreBuilder::m_szGameAssetSrcPath); Msg(" to "); ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", CCoreBuilder::m_szGameAssetDstPath);

	char szMaterialSrcDir[MAX_PATH];
	V_sprintf_safe(szMaterialSrcDir, "%s\\*%s", CCoreBuilder::m_szGameAssetSrcPath, EXT_MATERIAL);

	g_pResourceCopy->CopyDirTo(szMaterialSrcDir , CCoreBuilder::m_szGameAssetDstPath, true, true, &CCoreBuilder::GenerateBuildingListAssets(EXT_MATERIAL));
	Msg("\n");
}


//-----------------------------------------------------------------------------
// Purpose:	Generate & add a list of all the textures that need to be compiled.
//-----------------------------------------------------------------------------
inline void CMaterialBuilder::AssetBuilderCompile()
{
	CCoreBuilder::PrintHeaderCompileType();

	if (!CCoreBuilder::m_bRunAssetCompile)
		return;

	CMaterialBuilder::DeleteCompiledContents();
	CMaterialBuilder::CopySrcVmtToGameDir();
	
	CCoreBuilder::AssetBuilderCompile();
	CCoreBuilder::GenerateGlobalOperationReport();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CMaterialBuilder::DeleteCompiledContents()
{
	CCoreBuilder::DeleteCompiledContents();

	{
		char szWildCard[MAX_PATH];
		V_sprintf_safe(szWildCard, "%s\\*%s", CCoreBuilder::m_szGameAssetDstPath, EXT_MATERIAL);
		g_pResourceCopy->DeleteDirRecursive(szWildCard, true, false);
	}
}


#endif // ASSETBUILDER_MATERIAL_HPP

