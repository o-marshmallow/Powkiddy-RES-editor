# RES editor for PowKiddy firmware

## What is the purpose of this project ?

RES files are used in Powkiddy consoles running µCO/S-II (mainly X16).
These files are used to store images that will be used by the applications, like the launcher for example.
The main purpose of this project is to be able to view and replace the images located in these RES files.

Please keep in mind this is an **alpha** version. It only supports viewing file at the moment. It **DOES NOT** support export/import functionnality yet.

## How to use it ?

It has been developed using C++ and Qt framework and Qt Creator.
Simply import .pro file in Qt Creator to be able to compile and execute the whole project.
It has **only** been tested on Linux (Ubuntu 14.04).

## Any dependency ?

**YES**, zlib is necessary to DEFLATE some parts of the RES file.

## Changelog

v0.25:
- Images can now be exported as PNG

v0.2:
- Unknown field is now known (alpha or not alpha image)
- Add support to view opaque images (no alpha)
- After loading a RES file, the first picture will be shown
- Factorize code

v0.1:
- Initial version: the program can open RES file and show some images of the file.

## Thanks

Big thanks to **KikiOnE** from xda-developers forum who worked with me on reverse engineering the RES file structure.

## Screenshot

![alt Main screen](https://raw.githubusercontent.com/o-marshmallow/Powkiddy-RES-editor/master/res/res_editor.png)
