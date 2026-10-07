# Makes sure data\SDL3\SDL3.dll is the official x86 runtime matching the third_party/SDL submodule.
# Fails if $Version doesn't match the submodule's SDL_version.h, so a submodule bump can't ship a stale DLL.
# When bumping SDL: move the submodule, then update $Version and $Sha256 (hash of the zip) below.
param([string]$Dest = (Join-Path $PSScriptRoot '..\..\data\SDL3'))

$Version = '3.4.18'
$Sha256  = '99f2ee6e126d828c5e5b3d20201930f3d16a8c6e147d1de90a59f4ae6df138c9'
$Url     = "https://github.com/libsdl-org/SDL/releases/download/release-$Version/SDL3-$Version-win32-x86.zip"

$ErrorActionPreference = 'Stop'

# The submodule's headers must be the version we are about to fetch the DLL for
$header = Join-Path $PSScriptRoot '..\..\third_party\SDL\include\SDL3\SDL_version.h'
if (-not (Test-Path $header)) { throw "SDL headers not found, run: git submodule update --init" }
$text = Get-Content $header -Raw
$parts = foreach ($n in 'MAJOR', 'MINOR', 'MICRO') {
    if ($text -notmatch "#define SDL_${n}_VERSION\s+(\d+)") { throw "SDL_${n}_VERSION not found in $header" }
    $Matches[1]
}
$headerVersion = $parts -join '.'
if ($headerVersion -ne $Version) {
    throw "third_party/SDL is $headerVersion but fetch-sdl3.ps1 fetches $Version, update `$Version and `$Sha256"
}

$Dest = [IO.Path]::GetFullPath($Dest)
$dll = Join-Path $Dest 'SDL3.dll'
$marker = Join-Path $Dest 'SDL3.version'
if ((Test-Path $dll) -and (Test-Path $marker) -and ((Get-Content $marker -Raw).Trim() -eq $Version)) {
    Write-Host "SDL3 $Version x86 already present"
    return
}

$zip = Join-Path ([IO.Path]::GetTempPath()) "SDL3-$Version-win32-x86.zip"
Invoke-WebRequest -Uri $Url -OutFile $zip
$hash = (Get-FileHash $zip -Algorithm SHA256).Hash
if ($hash -ne $Sha256.ToUpper()) { throw "SDL3 zip hash mismatch: got $hash, expected $Sha256" }

New-Item -ItemType Directory -Force $Dest | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($zip)
try {
    [IO.Compression.ZipFileExtensions]::ExtractToFile($archive.GetEntry('SDL3.dll'), $dll, $true)
} finally { $archive.Dispose() }
Remove-Item $zip
Set-Content $marker $Version
Write-Host "SDL3 $Version x86 fetched to $Dest"
