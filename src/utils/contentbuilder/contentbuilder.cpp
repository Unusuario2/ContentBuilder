//=== Contenbuilder -> Written by Unusuario2, https://github.com/Unusuario2  ===//
//
// Purpose: ContentBuilder - Tool for compiling, processing, and packaging game assets 
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
#include <tier1/strtools.h>
#include <tier0/icommandline.h>
#include <tools_minidump.h>
#include <loadcmdline.h>
#include <cmdlib.h>
#include <filesystem_init.h>
#include <filesystem_tools.h>
#include <colorschemetools.h>
#include <pipeline_shareddefs.h>
#include <consolelogger.hpp>

#include "contentbuilder.hpp"
#include "assetbuilder_material.hpp"
#include "assetbuilder_model.hpp"
#include "assetbuilder_scene.hpp"
#include "assetbuilder_caption.hpp"
#include "assetbuilder_map.hpp"
#include "assetbuilder_vpk.hpp"

// Note: this is disabled in SP-branch
#ifdef MP_ADDON_SUPPORT
#include "assetbuilder_addon.hpp"
#endif // MP_ADDON_SUPPORT

#pragma warning(disable : 4238)

// TODO: Document the code!!


//-----------------------------------------------------------------------------
// Purpose: Global vars 
//-----------------------------------------------------------------------------
float               g_flStartTime           = 0;
int                 g_iThreads              = -1; 
bool                g_bForce32bits          = !IsPlatform64Bits();
bool                g_bForce64bits          = IsPlatform64Bits();
bool                g_bGenerateInfoBuild    = false;
bool                g_bAddonBuilder         = false;
bool                g_bForceBuildContent    = false;
bool                g_bBuildMaterials       = true;
bool                g_bBuildModels          = true;
bool                g_bBuildScene           = true;
bool                g_bBuildCaption         = true;
bool                g_bBuildMap             = true;
bool                g_bBuildVpk             = false;
bool                g_bContentDstDelete     = false;
bool                g_bQuiet                = false;
bool                g_bSteamBuild           = false;
char                g_szGameBin[MAX_PATH];                  //  $SteamDir/Half-Life 2/bin or $SteamDir/Half-Life 2/bin/x64
char                g_szSteamDir[MAX_PATH];                 //  $SteamDir/Half-Life 2
char                g_szGameInfoFile[MAX_PATH];             //  $SteamDir/Half-Life 2/hl2/gameinfo.txt
char                g_szContentBuilderScriptFile[MAX_PATH]; //  $SteamDir/Half-Life 2/hl2/scripts/tools/contentbuilder_settings.txt
char                g_szContentBuilderOutPath[MAX_PATH];    //  $SteamDir/Half-Life 2/hl2/_build
CConsoleLogger*     g_pConsoleLogger        = nullptr;
CResourceCopy*      g_pResourceCopy         = nullptr;
CMaterialBuilder*   g_pMaterialBuilder      = nullptr;
CModelBuilder*      g_pModelBuilder         = nullptr;
CSceneBuilder*      g_pSceneBuilder         = nullptr;
CCaptionBuilder*    g_pCaptionBuilder       = nullptr;
CMapBuilder*        g_pMapBuilder           = nullptr;
CVpkBuilder*        g_pVpkBuilder           = nullptr;
#ifdef MP_ADDON_SUPPORT
CAddonBuilder*      g_pAddonBuilder         = nullptr;
#endif // MP_ADDON_SUPPORT
SpewMode            g_eSpewMode             = SpewMode::k_Normal;


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static void Init_AssetTools()
{
    {
        const float start = Plat_FloatTime();
        Msg("Initializing Sub-AssetSystem... ");
        g_pMaterialBuilder =    new CMaterialBuilder();
        g_pModelBuilder =       new CModelBuilder();
        g_pSceneBuilder =       new CSceneBuilder();
        g_pCaptionBuilder =     new CCaptionBuilder();
        g_pMapBuilder =         new CMapBuilder();
        g_pVpkBuilder =         new CVpkBuilder();
#ifdef MP_ADDON_SUPPORT
        g_pAddonBuilder =       new CAddonBuilder();
#endif // MP_ADDON_SUPPORT
        g_pResourceCopy->SetThreads(g_iThreads);
        Msg("done(%.2f)\n", Plat_FloatTime() - start);
    }

    // Basic info
    Msg("Working Directory:   ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "\"%s\"\n", g_szSteamDir);
    Msg("Mod:                 ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "\"%s\"\n", gamedir);
    Msg("Operation:           %s\n",
        []() -> const char*
        {
            if (g_bForceBuildContent)
                return "Force-building content";
            else
                return "Partial-building content";
        }()
            );
    Msg("Verbosity:           %s\n",
        []()-> const char*
        {
            if (verbose)
                return "High";
            else if (g_bQuiet)
                return "Quiet";
            else
                return "Standard";
        }()
            );
    Msg("Building:            %s\n",
        []() -> const char*
        {
            if (g_bAddonBuilder)
                return "Addon";
            else
                return "Game";
        }()
            );
    Msg("Exclude:             %s\n",
        []() -> const char*
        {
            char szExclude[MAX_PATH] = {'\0'};
            if (!g_bBuildMaterials)   V_strcat_safe(szExclude, "(materials) ");
            if (!g_bBuildModels)      V_strcat_safe(szExclude, "(models) ");
            if (!g_bBuildScene)       V_strcat_safe(szExclude, "(scene) ");
            if (!g_bBuildCaption)     V_strcat_safe(szExclude, "(caption) ");
            if (!g_bBuildMap)         V_strcat_safe(szExclude, "(maps) ");
            if (!g_bBuildVpk)         V_strcat_safe(szExclude, "(vpk) ");
            else                      V_strcat_safe(szExclude, "None ");

            return szExclude;
        }()
            );

     ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "\nGeneral paths:\n");
     Msg("  Path - gameinfo.txt:         ");      ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "\"%s\"\n", g_szGameInfoFile);
     Msg("  Path - Game binary (tools):  ");      ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "\"%s\"\n", g_szGameBin);

     ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "%s Source/Compiled assets paths:\n", 
         []() -> const char*
         {
             if (g_bAddonBuilder)
                 return "Addon";
             else
                 return "Game";
         }()
             );
    if (g_bBuildMaterials)   { Msg("  Content Source - Materials:  ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pMaterialBuilder->m_szGameAssetSrcPath); }
    if (g_bBuildModels)      { Msg("  Content Source - Models:     ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pModelBuilder->m_szGameAssetSrcPath); }
    if (g_bBuildScene)       { Msg("  Content Source - Scene:      ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pSceneBuilder->m_szGameAssetSrcPath); }
    if (g_bBuildCaption)     { Msg("  Content Source - Caption:    ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pCaptionBuilder->m_szGameAssetSrcPath); }
    if (g_bBuildMap)         { Msg("  Content Source - Maps:       ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pMapBuilder->m_szGameAssetSrcPath); }
    if (g_bBuildVpk)         { Msg("  Content Source - VPK:        ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pVpkBuilder->m_szGameDirPath); }
    if (g_bBuildMaterials)   { Msg("  Content Compiled - Materials:");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pMaterialBuilder->m_szGameAssetDstPath); }
    if (g_bBuildModels)      { Msg("  Content Compiled - Models:   ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pModelBuilder->m_szGameAssetDstPath); }
    if (g_bBuildScene)       { Msg("  Content Compiled - Scene:    ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pSceneBuilder->m_szGameAssetDstPath); }
    if (g_bBuildCaption)     { Msg("  Content Compiled - Caption:  ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pCaptionBuilder->m_szGameAssetDstPath); }
    if (g_bBuildMap)         { Msg("  Content Compiled - Maps:     ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pMapBuilder->m_szGameAssetDstPath); }
    if (g_bBuildVpk)         { Msg("  Content Compiled - VPK:      ");   ColorSpewMessage(SPEW_MESSAGE, &ColorPath, "%s\n", g_pVpkBuilder->m_szGameDirPath); }
    Msg("\n");

    {
        const float start = Plat_FloatTime();
        Msg("Initializing AssetTools systems (%s bits)... ",
            []() -> const char*
            {
                if (g_bForce32bits)
                    return "32";
                else
                    return "64";
            }()
        );
        if (g_bBuildMaterials)   g_pMaterialBuilder->AssetToolCheck();
        if (g_bBuildModels)      g_pModelBuilder->AssetToolCheck();
        if (g_bBuildScene)       g_pSceneBuilder->AssetToolCheck();
        if (g_bBuildCaption)     g_pCaptionBuilder->AssetToolCheck();
        if (g_bBuildMap)         g_pMapBuilder->AssetToolCheck();
        if (g_bBuildVpk)         g_pVpkBuilder->AssetToolCheck();
#ifdef MP_ADDON_SUPPORT
        if (g_bAddonBuilder)     g_pAddonBuilder->AssetToolCheck();
#endif // MP_ADDON_SUPPORT
        Msg("done(%.2f)\n", Plat_FloatTime() - start);
    }

    if (!g_pResourceCopy->CreateDir(g_szContentBuilderOutPath))
        g_pConsoleLogger->Error("AssetSystem -> Could not create contentbuilder build path: %s\n", g_szContentBuilderOutPath);

    {
        char szLogFile[MAX_PATH];
        V_sprintf_safe(szLogFile, "%s\\contentbuilder.log", g_szContentBuilderOutPath);
        SetSpewFunctionLogFile(szLogFile);
    }
}


//-----------------------------------------------------------------------------
// Purpose:   Print contentbuilder usage
//-----------------------------------------------------------------------------
static void PrintUsage(int argc, char* argv[])
{
    Msg("\nUsage: contentbuilder.exe [options] -game <path>\n\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, " General Options:\n");
    Msg("   -fb:                   Force build even if the there is not out-of-date content.\n"
        "   -contentdstdelete:     Deletes all compiled assets before starting contentbuilder compile. (USE WITH EXTREME CAUTION!!).\n"
        "   -info:                 Generate filelist of the assets that will be build (This mode disables building). (Note: This will be generated at '%s' folder).\n"
        "   -game <path>:          Specify the folder of the gameinfo.txt file.\n"
        "   -vproject <path>:      Same as \'-game\'.\n"
        "\n", BUILDER_OUTDIR);
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, " Building Options:\n");
    Msg(
#ifdef MP_ADDON_SUPPORT
        "   -addonbuild:           If enabled, it will compile the addon and pack all the contents into the defined .bsp file\n"
        "                          Useful for addons intended for release on the Workshop. (\'-addonbuild\' needs to be enabled to work)\n"
#endif // MP_ADDON_SUPPORT
        "   -skipmaterial:         Skips texture compile.\n"
        "   -skipmodel:            Skips model compile.\n"
        "   -skipscene:            Skips scene compile.\n"
        "   -skipcaption:          Skips caption compile.\n"
        "   -skipmap:              Skips maps compile.\n"
        "   -vpk:                  Generate vpk files. (Not avaible for \'-addonbuild\').\n"
        "   -steambuild:           Generates a build for steam release content, (Not avaible for addon build, only for game builds)\n"
        "                          The -steambuild command enables VPK generation and sanitizes the game directory.\n"    
        "\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, " Spew Options:\n");
    Msg("   -v or -verbose:        Enables verbose.\n"
        "   -q or -quiet:          Prints minimal text. (Note: Disables \'-verbose\' and \'-spewallcommands\')\n"   // TODO!
//      "   -spewallcommands:                     \n"                                                               // TODO!
//      "   -compileverbose:       Enable verbose for tools. (Prints a LOT of text)\n"                              // TODO!
//      "   -spewallverbose:       Same as \'-v -spewallcommands -compileverbose\'\n"      
        "\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, " Advanced Build Options:\n");
    Msg("   -toolsforce32bits:     Force contentbuilder to use 32 bits tools.\n"
        "   -toolsforce64bits:     Force contentbuilder to use 64 bits tools.\n"
        "   -proc n or -threads n: Max simultaneous compile subsystems. (Not avaible for MapBuilder, SceneBuilder, AddonBuilder, VpkBuilder).\n"
        "\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, " Other Options:\n");
    Msg("   -FullMinidumps:        Write large minidumps on crash.\n"
        "\n");

    DeleteCmdLine(argc, argv);
    CmdLib_Cleanup();
    CmdLib_Exit(-1);
}


//-----------------------------------------------------------------------------
// Purpose:   Parse command line
//-----------------------------------------------------------------------------
static void ParseCommandline(int argc, char* argv[])
{
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "Command Line:\n\t");
    for (std::size_t i = 1; i < argc; ++i)
    {
        Msg("%s ", argv[i]);
    }
    Msg("\n");

    if(argc == 1 || argc == 2)
    {
        PrintUsage(argc, argv);
    }

    for (int i = 1; i < argc; ++i)
    {
        if (!V_stricmp(argv[i], "-?") || !V_stricmp(argv[i], "-help") || argc == 1)
        {
            PrintUsage(argc, argv);
        }
        else if (!V_stricmp(argv[i], "-v") || !V_stricmp(argv[i], "-verbose"))
        {
            verbose = true;
            g_bQuiet = false;
        }       
        else if (!V_stricmp(argv[i], "-quiet"))
        {
            g_bQuiet = true;
            verbose = false;
        }
        else if (!V_stricmp(argv[i], "-fb"))
        {
            g_bForceBuildContent = true;
        }
        else if (!V_stricmp(argv[i], "-info"))
        {
            g_bGenerateInfoBuild = true;
        }        
        else if (!V_stricmp(argv[i], "-contentdstdelete"))
        {
            g_bContentDstDelete = true;
        }       
        else if (!V_stricmp(argv[i], "-FullMinidumps"))
        {
            EnableFullMinidumps(true);
        }
        else if (!V_stricmp(argv[i], "-toolsforce32bits"))
        {
            g_bForce32bits = true;
            g_bForce64bits = false;
        }
        else if (!V_stricmp(argv[i], "-toolsforce64bits"))
        {
            g_bForce32bits = false;
            g_bForce64bits = true;
        }        
#ifdef MP_ADDON_SUPPORT
        else if (!V_stricmp(argv[i], "-addonbuild"))
        {
            g_bAddonBuilder = true;
            g_bBuildVpk = false;
        }      
#endif // MP_ADDON_SUPPORT
        else if (!V_stricmp(argv[i], "-skipmaterial"))
        {
            g_bBuildMaterials = false;
        }
        else if (!V_stricmp(argv[i], "-skipmodel"))
        {
            g_bBuildModels = false;
        }
        else if (!V_stricmp(argv[i], "-skipscene"))
        {
            g_bBuildScene = false;
        }
        else if (!V_stricmp(argv[i], "-skipcaption"))
        {
            g_bBuildCaption = false;
        }
        else if (!V_stricmp(argv[i], "-skipmap"))
        {
            g_bBuildMap = false;
        }
        else if (!V_stricmp(argv[i], "-vpk"))
        {
            g_bBuildVpk = true;
        }
        else if (!V_stricmp(argv[i], "-steambuild"))
        {
            g_bSteamBuild = true;
            g_bBuildVpk = true;
        }
        else if (!V_stricmp(argv[i], "-threads") || !V_stricmp(argv[i], "-proc"))
        {
            if (++i < argc && argv[i] != '\0')
            {
                int iTemp = V_atoi(argv[i]);
                if (iTemp < 1)
                {
                    Warning("Expected value greater or equal that 1!\n");
                    PrintUsage(argc, argv);
                }
                g_iThreads = iTemp;
            }
            else
            {
                Warning("Expected value after '-threads'\n");
                PrintUsage(argc, argv);
            }
        }
        else if (!V_stricmp(argv[i], "-game") || !V_stricmp(argv[i], "-vproject"))
        {
            if (++i < argc && argv[i][0] != '-')
            {
                char* gamePath = argv[i];
                if (!gamePath)
                {
                    Error("\nError: \'-game\' requires a valid path argument. NULL path\n");
                }
                V_strcpy_safe(gamedir, gamePath);
            }
            else
            {
                Error("\nError: \'-game\' requires a valid path argument.\n");
            }
        }
        else
        {
            Warning("\nWarning Unknown option \'%s\'\n", argv[i]);
            PrintUsage(argc, argv);
        }
    }
    Msg("\n");
}


//-----------------------------------------------------------------------------
// Purpose:   Prints the header
//-----------------------------------------------------------------------------
static void PrintHeader()
{
    Msg("\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "//------------------------------------------------------------\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "// "); Msg("Content builder (Build: %s %s)\n", __DATE__, __TIME__);
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "// "); Msg("Written by Unusuario 2 https://github.com/Unusuario2\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "//------------------------------------------------------------\n");
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static void Init(int argc, char* argv[])
{
    g_flStartTime = Plat_FloatTime();

    SetupDefaultToolsMinidumpHandler();
    CommandLine()->CreateCmdLine(argc, argv);
    InstallSpewFunction();
    PrintHeader();
    ParseCommandline(argc, argv);

    // Note: Even though it's standard to add a '\' at the end of the string,
    // we remove it here because it makes managing paths much easier across the tool.
    CmdLib_InitFileSystem(gamedir);
    V_StripTrailingSlash(gamedir);

    V_sprintf_safe(g_szGameInfoFile, "%s\\%s", gamedir, "gameinfo.txt");
    V_sprintf_safe(g_szContentBuilderScriptFile, "%s\\%s", gamedir, BUILDER_CONFIG_FILE);
    V_sprintf_safe(g_szContentBuilderOutPath, "%s\\%s", gamedir, BUILDER_OUTDIR);

    g_pConsoleLogger = new CConsoleLogger(g_szContentBuilderOutPath, BUILDER_SPECIFIC_LOG, BUILDER_WARNING_LOG, BUILDER_ERROR_LOG);
    g_pResourceCopy = new CResourceCopy();

    if (verbose)
        g_eSpewMode = SpewMode::k_Verbose;
    else if (g_bQuiet)
        g_eSpewMode = SpewMode::k_Quiet;
    else
        g_eSpewMode = SpewMode::k_Normal;

    {
        g_pFullFileSystem->GetSearchPath_safe("BASE_PATH", false, g_szSteamDir);
        char* pChar = V_strrchr(g_szSteamDir, ';');
        if (pChar)
            pChar = '\0';

        V_StripTrailingSlash(g_szSteamDir);
    }

    // Setup the gamebin dir!
    V_sprintf_safe(g_szGameBin, "%s\\%s", g_szSteamDir,
        []() -> const char*
        {
            if (g_bForce32bits)
                return DIR_TOOLS_X86;
            else
                return DIR_TOOLS_X64;
        }()
            );

    // Set the Threads number
    if (g_iThreads == -1)
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        g_iThreads = sysInfo.dwNumberOfProcessors;
    }

    Init_AssetTools();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static void Destroy(int argc, char* argv[])
{
    g_pResourceCopy->GenerateGlobalOperationReport();
    ColorSpewMessage(SPEW_MESSAGE, &ColorSucesfull, "-------------------------------------------------------------------------------------------\n");
    ColorSpewMessage(SPEW_MESSAGE, &ColorSucesfull, "| ContentBuilder -> Build Done in %.2f seconds.\n", Plat_FloatTime() - g_flStartTime);
    ColorSpewMessage(SPEW_MESSAGE, &ColorSucesfull, "-------------------------------------------------------------------------------------------\n");

    delete g_pMaterialBuilder;
    delete g_pModelBuilder;
    delete g_pSceneBuilder;
    delete g_pCaptionBuilder;
    delete g_pMapBuilder;
    delete g_pVpkBuilder;
#ifdef MP_ADDON_SUPPORT
    delete g_pAddonBuilder;
#endif // MP_ADDON_SUPPORT

    delete g_pResourceCopy;
    delete g_pConsoleLogger;

    DeleteCmdLine(argc, argv);
    CmdLib_Cleanup();
    CmdLib_Exit(0);
}


//-----------------------------------------------------------------------------
// Purpose:   Main funtion
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Init(argc, argv);

    // MOVE THIS TO ANOTHER funtion!!!!
    if (g_bBuildMaterials)
    {
        if (g_bGenerateInfoBuild)
            g_pMaterialBuilder->GenerateAssetReport();
        else
        {
            g_pMaterialBuilder->AssetBuilderCompile();
        }
    }
    if (g_bBuildModels) 
    {
        if (g_bGenerateInfoBuild)
            g_pModelBuilder->GenerateAssetReport();
        else
            g_pModelBuilder->AssetBuilderCompile();
    }
    if (g_bBuildCaption) 
    {
        if (g_bGenerateInfoBuild)
            g_pCaptionBuilder->GenerateAssetReport();
        else
            g_pCaptionBuilder->AssetBuilderCompile();
    }
    if (g_bBuildScene) 
    {
        if (g_bGenerateInfoBuild)
            g_pSceneBuilder->GenerateAssetReport();
        else
            g_pSceneBuilder->AssetBuilderCompile();
    }
    if (g_bBuildMap) 
    {
        if (g_bGenerateInfoBuild)
            g_pMapBuilder->GenerateAssetReport();
        else
            g_pMapBuilder->AssetBuilderCompile();
    }
    if (g_bSteamBuild)
    {
        ColorSpewMessage(SPEW_MESSAGE, &ColorHeader, "\n==== Sanitize Game Diretory ====\n");
        {
            char szWildCard[MAX_PATH];
            const char* rgpExt[] = { ".log", ".ptr", ".lin" };
            for (int i = 0; i < 3; i++) 
            {
                V_sprintf_safe(szWildCard, "%s\\*%s", gamedir, rgpExt[i]);
                g_pResourceCopy->DeleteDirRecursive(szWildCard);
            }
        }
    }
    if (g_bBuildVpk)
    {
        if (g_bGenerateInfoBuild)
            g_pVpkBuilder->GenerateAssetReport();
        else
            g_pVpkBuilder->AssetBuilderCompile();
    };
#ifdef MP_ADDON_SUPPORT
    if (g_bAddonBuilder) 
    {
        if (g_bGenerateInfoBuild)
            g_pAddonBuilder->GenerateAssetReport();
        else
            g_pAddonBuilder->AssetBuilderCompile();
    }
#endif // MP_ADDON_SUPPORT
    Destroy(argc, argv);

    return 0;
}

