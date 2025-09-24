//==== ModelBuilder -> Written by Unusuario2, https://github.com/Unusuario2  ===//
//
// Purpose: ModelBuilder, A ContentBuilder subsystem for model batch compiling.
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
#ifndef ASSETBUILDER_MODEL_HPP
#define ASSETBUILDER_MODEL_HPP

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
#define KV_BUILDER_MODEL	"ModelBuilder"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CModelBuilder : public CCoreBuilder
{
public:
	inline CModelBuilder();
	inline ~CModelBuilder();

	inline void AssetBuilderCompile() override;
	inline void DeleteCompiledContents() override;
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CModelBuilder::CModelBuilder() :
	CCoreBuilder(
				 KV_BUILDER_MODEL,
				 DIR_MODELSRC,
				 DIR_MODELS,
				 false,
		 		 g_bContentDstDelete,
				 g_iThreads, 
				 TOOL_MODEL,
		         FileList{ FileString { EXT_MODELSRC } },
				 EXT_MODEL
	)
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CModelBuilder::~CModelBuilder()
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CModelBuilder::AssetBuilderCompile()
{
	CCoreBuilder::PrintHeaderCompileType();

	if (!CCoreBuilder::m_bRunAssetCompile)
		return;

	CModelBuilder::DeleteCompiledContents();
	CCoreBuilder::AssetBuilderCompile();
	CCoreBuilder::GenerateGlobalOperationReport();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline void CModelBuilder::DeleteCompiledContents()
{
	CCoreBuilder::DeleteCompiledContents();
}

#endif // ASSETBUILDER_MODEL_HPP