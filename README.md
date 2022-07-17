# D2StaticExtractor (DSE)
## [Charm](https://github.com/MontagueM/Charm) exists and is a better tool than this.
DSE is a static model extraction tool that converts Destiny 2 static models into fbx files. A static model is any model that **does not** move, is **not** animated, or **doesn't** have a skeleton.  

## [Download from here](https://github.com/nblockbuster/D2StaticExtractor/releases)


**DSE only supports Destiny 2 versions past 3.0.0.1 (Beyond Light) as of now.**   
**Textures only work past 4.0.0.1 (TWQ), unless you usea previous release.**

I will try to add support for these in the near future:
- Full working LOD culling
- Correct UV transforms
- Seperating models by material used
- Correct vertex normals
- Fix scaling
- Correct vertex position offsets n stuff
- Loadzones  

Far-off goals:
- Pre-BL


There is no risk of a ban by using this tool.  

## Usage

The tool requires Windows 64 bit and a Destiny 2 installation.  

The tool is used via the command line:

`D2StaticExtractor.exe -p [packages path] -o [output path] -i [singular hash] -b [package id] -t -l`

The required arguments are -p, -o and one of -i or -b.
- -p \[packages path]: package path for Destiny 2. An example might be "C:/Program Files (x86)/Steam/steamapps/common/Destiny 2/packages"
- -o \[output path]: the output path
- -i \[input hash]: the input hash. To get hashes to extract, either use the batch command or ask others i guess (*look for a google sheet posted in ripping sometime*)
- -t: enable texture extraction
- -b \[package ID]: will extract every static model given for a package ID. For the package "w64_venus_300_2", the package ID would be "0300"
- -l: Disables hacky LOD culling  

## Thanks
Getting to this point in datamining, and this project, wouldn't have been possible without the support from these (and many more) people's support:
- 42
- Monteven
- MrAir
- BIOS
- HighRTT
