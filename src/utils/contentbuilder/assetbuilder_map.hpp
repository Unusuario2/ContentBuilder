//===== MapBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ====//
//
// Purpose: MapBuilder, A ContentBuilder subsystem for map batch compiling.
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
#ifndef ASSETBUILDER_MAP_HPP
#define ASSETBUILDER_MAP_HPP

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
#define KV_BUILDER_MAP             "MapBuilder"
#define KV_MAP_GEOMETRY            "Vbsp"
#define KV_MAP_VISIBILITY          "Vvis"
#define KV_MAP_RADIOSITY           "Vrad"
#define KV_MAP_INFO                "VbspInfo"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CMapBuilder final : public CCoreBuilder
{
public:
	inline CMapBuilder();
	inline ~CMapBuilder();

	inline void AssetBuilderCompile() override;
	inline void DeleteCompiledContents() override;
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CMapBuilder::CMapBuilder() :
	CCoreBuilder(
				KV_BUILDER_MAP,
				DIR_MAPSRC,
				DIR_MAPS,
				true,
				g_bContentDstDelete,
				1, // For mapbuilder we dont use multithreading becouse the tools like, vvis and vrad uses multrithreading.
				TOOL_MAPBUILDER,
				FileList{ FileString { EXT_MAPSRC_VMN },
					      FileString { EXT_MAPSRC_VMF } },
				EXT_MAP
	)
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CMapBuilder::~CMapBuilder()
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CMapBuilder::AssetBuilderCompile()
{
	CCoreBuilder::PrintHeaderCompileType();

	if (!CCoreBuilder::m_bRunAssetCompile)
		return;

	CMapBuilder::DeleteCompiledContents();
	CCoreBuilder::AssetBuilderCompile();
	CCoreBuilder::GenerateGlobalOperationReport();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CMapBuilder::DeleteCompiledContents()
{
	CCoreBuilder::DeleteCompiledContents();
}


#endif // ASSETBUILDER_MAP_HPP
