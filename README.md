# D2StaticExtractor (DSE)
DSE is a static model extraction tool that converts Destiny 2 static models into fbx files. A static model is any model that does **not** move, is animated, or has a skeleton. The extractor has:
- Texture extraction (DDS, adding TGA & PNG support soon)
- Batch package extraction  

I will try to add support for these in the near future:
- Correct UV transforms
- Correct vertex normals
- Vertex color
- Full working LOD culling
- Seperating models by material used
- Other better fixes I guess  

Far-off goals:
- Loadzones  

There is no risk of a ban by using this tool.  
If you (for some reason) use DSE in your projects, it would be appreciated if you credited me and/or 42 

## Usage

The tool requires Windows 64 bit and a Destiny 2 installation.  
You also need to copy the "oo2core_8_win64.dll" from your Destiny 2 installation's bin/x64 folder.

The tool is used via the command line:

`D2StaticExtractor.exe -p [packages path] -o [output path] -i [singular hash] -t -b [package id]`

The required arguments are -p, -o and one of -i or -b.
- -p \[packages path]: package path for Destiny 2. An example might be C:/Program Files (x86)/Steam/steamapps/common/Destiny 2/packages
- -o \[output path]: the output path
- -i \[input hash]: the input hash. To get hashes to extract, either use the batch command or ask others i guess (*look for a google sheet posted in ripping sometime*)
- -t: enable texture extraction
- -b \[package ID]: will extract every static model given for a package ID. For the package "w64_venus_300_2", the package ID would be "0300"  

## Thanks
Getting to this point in datamining, and this project, wouldn't have been possible without the support from these (and many more) people's support:
- 42
- Monteven
- MrAir
- BIOS
- HighRTT
