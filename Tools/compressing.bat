@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set toolpath=%~d0%~p0
set texPath=%~d0%~p0..\x64\Assets\Texture\
set srcPath=%texPath%Original\
set destPathLossy=%texPath%Lossy\
set destPathLossless=%texPath%Lossless\
set destPathConvert=%texPath%Convert\
set destPathConvertiOS=%destPathConvert%iOS\
set destPathConvertAndroid=%destPathConvert%Android\
set destPathConvertWindows=%destPathConvert%Windows\

if not exist %srcPath% (
md %srcPath%
)
if not exist %destPathLossy% (
md %destPathLossy%
)
if not exist %destPathLossless% (
md %destPathLossless%
)
if not exist %destPathConvert% (
md %destPathConvert%
)
if not exist %destPathConvertiOS% (
md %destPathConvertiOS%
)
if not exist %destPathConvertAndroid% (
md %destPathConvertAndroid%
)
if not exist %destPathConvertWindows% (
md %destPathConvertWindows%
)

set name="fileName"

for /f %%f in ('dir %srcPath%*.png /S /B') DO (

set name=%%~nxf

if not exist %destPathLossy%!name! (
%toolpath%pngquant.exe %srcPath%!name! --output "%destPathLossy%!name!"
echo [!name!] Lossy compressed to %destPathLossy%
)

if not exist %destPathLossless%!name! (
%toolpath%optipng.exe %srcPath%!name! -dir %destPathLossless%
echo [!name!] Lossless compressed to %destPathLossless%
)

if not exist %destPathConvertiOS%%%~nf.pvr (
%toolpath%PVRTexToolCLI.exe -i %srcPath%!name! -o %destPathConvertiOS%%%~nf.pvr -f PVRTCI_4BPP_RGB -q pvrtcfast
echo [!name!] converted to %destPathConvertiOS%

)

if not exist %destPathConvertAndroid%%%~nf.ktx (
%toolpath%PVRTexToolCLI.exe -i %srcPath%!name! -o %destPathConvertAndroid%%%~nf.ktx -f ETC2_rgb -q etcfast
echo [!name!] converted to %destPathConvertAndroid%
)

if not exist %destPathConvertWindows%%%~nf.dds (
%toolpath%PVRTexToolCLI.exe -i %srcPath%!name! -o %destPathConvertWindows%%%~nf.dds -f BC3
echo [!name!] converted to %destPathConvertWindows%
)

)

exit 0