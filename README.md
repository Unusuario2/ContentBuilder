# ContentBuilder  
**ContentBuilder** is a flexible command-line utility designed to automate the **asset compilation pipeline** for Source Engine projects.  

This makes **ContentBuilder** especially useful for:  
- Teams or individuals who want to include **automated map builds in larger workflows** (e.g., continuous integration, automated testing, or packaging pipelines).  
- Anyone seeking a **Source 2–style workflow** within Source 1.  

By streamlining repetitive compile tasks, **ContentBuilder** helps keep your workflow **fast, consistent, and efficient**.  

ContentBuilder allows developers to:
- Compile and package multiple asset types.  
- Control exactly what gets built and how.  
- Generate reports instead of compiling.  
- Separate warnings and errors into dedicated logs.  
- Manage performance with multi-threading and 32/64-bit tools.  
- Prepare builds ready for Steam or Workshop release.  

**ContentBuilder** uses external tools, such as [SceneImageBuilder](https://github.com/Unusuario2/SceneImageBuilder), [ResourceCopy](https://github.com/Unusuario2/ResourceCopy), [MapBuilder](https://github.com/Unusuario2/MapBuilder), you need to donwload them in order for **ContentBuilder** to work. Additionally, **ContentBuilder** is designed to integrate with the Source 1 ecosystem, which means it also utilizes tools such as `vtex.exe`, `studiomdl.exe`, `captioncompiler.exe`, and `vpk.exe`.

<img width="1301" height="687" alt="image" src="https://github.com/user-attachments/assets/7ef1edaf-8bae-44e3-a665-6d8db224d33a" />

**ContentBuilder in action:**

https://github.com/user-attachments/assets/62d3dc90-3069-4f7d-a5cd-7eb1ce926feb


---
## Installation  
Drag the executable in the `bin\x64`or `bin` if on 32 bits games. 

---
## Build
To build **ContentBuilder**, follow these steps:  

1. Start with a clean copy of the **Source SDK 2013** source code (works with both SP & MP).  
2. Drag and drop the contents of this repository into your SDK source directory.  
3. Overwrite any existing files when prompted.  
4. Regenerate the Visual Studio solution files (e.g., by running `createallprojects.bat`).  
5. Open the solution in Visual Studio and compile it. The `contentbuilder.exe` executable will then be ready to use.  

---
## Features  
### Supported Asset Types
- Materials (textures)  
- Models  
- Scenes  
- Captions (subtitles)  
- Maps  
- VPK packages  
- Addons (if compiled with `MP_ADDON_SUPPORT`) **(NOT WORKING AT THE MOMENT)**

### Build Modes
- Force Build (`-fb`): Rebuilds all assets, even if they are up to date.  
- Partial Build (default): Only builds assets that are out of date. **(NOT WORKING AT THE MOMENT)**
- Info Build (`-info`): Generates a detailed report of which assets would be built without compiling them.  
- Steam Build (`-steambuild`): Prepares assets for a Steam release.  
  - Automatically enables VPK generation.  
  - Cleans the game directory from residual files (.log, .ptr, .lin).  
- Addon Build (`-addonbuild`): Packages addons into a .bsp for Workshop release. **(NOT WORKING AT THE MOMENT)**
- `-skipmaterial`: Skip texture compilation.  
- `-skipmodel`: Skip model compilation.  
- `-skipscene`: Skip scene compilation.  
- `-skipcaption`: Skip caption compilation.  
- `-skipmap`: Skip map compilation.  
- `-vpk`: Generate VPK packages.  

### Performance and System Options
- `-threads N` or `-proc N`: Set maximum simultaneous compile threads. Avaivle for **MaterialBuilder, ModelBuilder, CaptionBuilder** 
- `-toolsforce32bits`: Force usage of 32-bit tools.  
- `-toolsforce64bits`: Force usage of 64-bit tools.  

### Logging and Reports
- A main log is generated at:  
<game>_build\contentbuilder.log
- The logger separates output into multiple files:  
- General log  
- Warnings log  
- Errors log  
- Logs help distinguish important issues from normal messages.  


---
## File Structure  

Since **Source 1** lacks a built-in system to sync source content with game assets, **ContentBuilder** enforces its own structure. Everything is relative to the `-game <gamedir>`.  

| Builder          | Source Extensions                | Final Extensions         | Description                          | Source Folder                    | Destination Folder |
|------------------|---------------------------------|-------------------------|--------------------------------------|----------------------------------|--------------------|
| **MaterialBuilder** | `.tga`, `.png`, `.psd`, `.vmt` | `.vtf`, `.vmt`          | Textures and materials               | `\materialsrc`                   | `\materials`       |
| **ModelBuilder**    | `.qc`, `.smd`, `.dmx`          | `.mdl`, `.vtx`, `.phy`, `.vvd`, `.ani` | 3D models             | `\modelsrc`                      | `\models`          |
| **CaptionBuilder**  | `.txt`                         | `.dat`                  | Close captions                       | `\resource\closecaptionsrc`      | `\resource`        |
| **SceneBuilder**    | `.vcd`                         | `.image`                  | Valve Choreography Data (scenes)   | `\scene`                         | `\scene`           |
| **MapBuilder**      | `.vmf`                         | `.bsp`                  | Source maps                          | `\mapsrc`                        | `\maps`            |
| **VpkBuilder**      | All compiled game content      | `.vpk`                  | Valve Pack files (VPK archives)      | `<gamedir>`                      | `<gamedir>`        |


---
## Supported Builders  

Each builder corresponds to a compile tool. By default, these tools are assumed to exist in the **`bin` or `bin\x64` directory** (the same location as `contentbuilder.exe`).  

You can modify the command line of the builders. To add a command, specify it inside the `BuildParam` KeyValue.  

### Example  
```txt
    CaptionBuilder
    {
        BuildParams     "-v -l"
    }
```

Only include the command-specific arguments in `BuildParam`, as ContentBuilder will automatically append:  
`-game <gamedir> <filename>`  

### Command Line Expansion  

The full format is:  
```
<Tool Path> <BuildParams> -game <gamedir> <filename>
```  

For **SceneBuilder** and **VpkBuilder**, the format is:  
```
<Tool Path> <BuildParams> -game <gamedir>
```  

Example:  
```txt
"C:\SteamLibrary\steamapps\common\Source SDK Base 2013 Multiplayer\bin\x64\captioncompiler.exe" -v -l -game "D:\generic_test" "D:\generic_test\resource\closecaptionsrc\closecaption_brazilian.txt"
```


---
## Execution Order  

Unless a compile type is skipped, the build order is as follows:  
1. **MaterialBuilder** → runs `vtex.exe` to compile all textures.  
2. **ModelBuilder** → runs `studiomdl.exe` to compile all models.  
3. **CaptionBuilder** → runs `captioncompiler.exe` to compile all closecaption files.  
4. **SceneBuilder** → runs `sceneimagebuilder.exe` to compile all Valve Choreography Data files.  
5. **MapBuilder** → runs `mapbuilder.exe` to compile all source map files.  
6. **VpkBuilder** → runs `vpk.exe` to create Valve Pack files.  


---
## Script file (`scripts/tools/contentbuilder_settings.txt`)  
**ContentBuilder** loads its configuration from `scripts/tools/contentbuilder_settings.txt`  
This file defines all the settings required for compilation.  
Take the example bellow as the base of your settings file.  
```txt
ContentBuilder
{
    // For any builder (except VpkBuilder) exclude these files, folders, or extensions
    Exclude
    {
        // Folders
        ExcludeFileOrFolder     "mapsrc\dev"
        ExcludeFileOrFolder     "mapsrc\prefab"
        ExcludeFileOrFolder     "mapsrc\instances"  

        // Files
        ExcludeFileOrFolder     "materialsrc\dev\test.psd"
        ExcludeFileOrFolder     "modelsrc\template.qc"
    }

    // Do not remove these commands for vtex.exe — ContentBuilder behavior depends on them
    MaterialBuilder 
    {
        BuildParams     "-nopause -deducepath -nop4"
    }

    ModelBuilder    
    {
        BuildParams     "-nop4"
    }

    SceneBuilder
    {
        BuildParams     ""
    }

    CaptionBuilder
    {
        BuildParams     "" 
    }

    MapBuilder    
    {
        BuildParams     "-preset Final"
    }

    VpkBuilder
    {
        BuildParams     "-M"

        // Exclusions specific to VPK builds
        Exclude
        {
            // Extensions
            ExcludeFileOrFolder ".vcd" 
            ExcludeFileOrFolder ".ptr" 
            ExcludeFileOrFolder ".lin" 
            ExcludeFileOrFolder ".vpk" 

            // Files
            ExcludeFileOrFolder "gameinfo.txt" 
            ExcludeFileOrFolder "contentbuilder_settings.txt" 

            // Folders
            ExcludeFileOrFolder "_build" 
            ExcludeFileOrFolder "mapsrc" 
            ExcludeFileOrFolder "materialsrc" 
            ExcludeFileOrFolder "modelsrc" 
            ExcludeFileOrFolder "resource/closecaptionsrc" 
        }
    }
}
```

---
## Important!
 - Your `gameinfo.txt` must be valid for the compiler to run, if not, the compile will fail. Which can lead to UNDEFINED behaviour.

---
## Contact  

- **Steam**: [Profile Link](https://steamcommunity.com/profiles/76561199073832016/)  
- **Twitter/X**: [@47Z14](https://x.com/47Z14)  
- **Discord**: `carlossuarez7285`  
---

