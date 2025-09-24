//=== CaptionBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ==//
//
// Purpose: CaptionBuilder, A ContentBuilder subsystem for batch compiling 
//          and processing closed captions.
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
#ifndef ASSETBUILDER_CAPTION_HPP
#define ASSETBUILDER_CAPTION_HPP

#ifdef _WIN32
#pragma once
#endif // _WIN32

#pragma warning(disable : 4238)

#include <filesystem_init.h>
#include <colorschemetools.h>
#include <resourcecopy/cresourcecopy.hpp>
#include "corebuilder.hpp"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
#define KV_BUILDER_CAPTION         "CaptionBuilder"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CCaptionBuilder : public CCoreBuilder
{
public:
	inline CCaptionBuilder();
	inline ~CCaptionBuilder();

	inline void AssetBuilderCompile() override;
	inline void DeleteCompiledContents();
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CCaptionBuilder::CCaptionBuilder() :
	CCoreBuilder(
				KV_BUILDER_CAPTION,
				DIR_CAPTIONSRC,
				DIR_CAPTIONS,
				false,
				g_bContentDstDelete,
				g_iThreads,
				TOOL_CAPTION,
				FileList{ FileString { EXT_CAPTIONSRC }},
				EXT_CAPTION
	)
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CCaptionBuilder::~CCaptionBuilder()
{
}


//-----------------------------------------------------------------------------
// Purpose:	Setup the enviroment for captioncompiler.exe to start & compile
//-----------------------------------------------------------------------------
inline void CCaptionBuilder::AssetBuilderCompile()
{
	CCoreBuilder::PrintHeaderCompileType();

	if (!CCoreBuilder::m_bRunAssetCompile)
		return;

	CCaptionBuilder::DeleteCompiledContents();
	CCoreBuilder::AssetBuilderCompile();

	// Copy all the .dat files to /resources
	{
		char szWildCard[MAX_PATH];
		V_sprintf_safe(szWildCard, "%s\\*%s", m_szGameAssetSrcPath, EXT_CAPTION);
		g_pResourceCopy->TransferDirTo(szWildCard, m_szGameAssetDstPath, false);
	}

	CCoreBuilder::GenerateGlobalOperationReport();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCaptionBuilder::DeleteCompiledContents()
{
	CCoreBuilder::DeleteCompiledContents();
}


#endif // ASSETBUILDER_CAPTION_HPP

