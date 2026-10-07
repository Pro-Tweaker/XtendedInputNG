# Makes sure data\XtendedInputNGButtons\XNFSTPKTool.exe is the official release, used to build the button TPK.
# https://github.com/xan1242/xnfstpktool
# When bumping: update $Version and $Sha256 (hash of the release zip) below.
param([string]$Dest = (Join-Path $PSScriptRoot '..\..\data\XtendedInputNGButtons'))

$Version = '3.05'
$Sha256  = '5d24761c8a8121ac5c1118eec2f7ec80a29b98c074a9ce975e7da3d176c004c4'
$Url     = "https://github.com/xan1242/xnfstpktool/releases/download/$Version/XNFSTPKTool-Release-XDKLibs.zip"

$ErrorActionPreference = 'Stop'
$Dest = [IO.Path]::GetFullPath($Dest)
$exe = Join-Path $Dest 'XNFSTPKTool.exe'
$marker = Join-Path $Dest 'XNFSTPKTool.version'
if ((Test-Path $exe) -and (Test-Path $marker) -and ((Get-Content $marker -Raw).Trim() -eq $Version)) {
    Write-Host "XNFSTPKTool $Version already present"
    return
}

$zip = Join-Path ([IO.Path]::GetTempPath()) "XNFSTPKTool-$Version.zip"
Invoke-WebRequest -Uri $Url -OutFile $zip
$hash = (Get-FileHash $zip -Algorithm SHA256).Hash
if ($hash -ne $Sha256.ToUpper()) { throw "XNFSTPKTool zip hash mismatch: got $hash, expected $Sha256" }

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($zip)
try {
    [IO.Compression.ZipFileExtensions]::ExtractToFile($archive.GetEntry('XNFSTPKTool.exe'), $exe, $true)
} finally { $archive.Dispose() }
Remove-Item $zip
Set-Content $marker $Version
Write-Host "XNFSTPKTool $Version fetched to $Dest"
