# Assembles a release package for one build configuration.
#   package.ps1 <Debug|Release> <Game> <TpkMode>
#     TpkMode:  none | w | w2   (w/w2 = build the button texture TPK, needs xnfstpktool, w2 is the MW variant)
param(
    [Parameter(Mandatory)][string]$BuildType,
    [Parameter(Mandatory)][string]$GameName,
    [Parameter(Mandatory)][string]$TpkMode
)
$ErrorActionPreference = 'Stop'

# The post-build event runs this in Windows PowerShell, but MSBuild can be started from PowerShell 7 (CI), whose
# PSModulePath makes 5.1 pick PowerShell 7's incompatible built-in modules (Get-FileHash etc. not found)
if ($PSVersionTable.PSEdition -eq 'Desktop') { $env:PSModulePath = [Environment]::GetEnvironmentVariable('PSModulePath', 'Machine') }

# Run native tools and stop on a failing exit code
function Invoke-Native([scriptblock]$Command) {
    & $Command
    if ($LASTEXITCODE -ne 0) { throw "Command failed (exit $LASTEXITCODE): $Command" }
}

# Everything below is relative to the repository root
Set-Location (Join-Path $PSScriptRoot '..\..')
$ConfigName = "$BuildType-$GameName"
$PackageDir = "dist\$ConfigName"
$Scripts = "$PackageDir\scripts"
$ButtonsDir = 'data\XtendedInputNGButtons'

# Make sure PATH has the tools in it
$env:PATH += ";$(Resolve-Path $ButtonsDir)"

Write-Host 'Creating directories...'
if (Test-Path $PackageDir) { Remove-Item $PackageDir -Recurse -Force }
New-Item -ItemType Directory $Scripts | Out-Null

Write-Host 'Copying the binary'
Copy-Item "bin\$ConfigName\XtendedInputNG.asi" $Scripts

if ($TpkMode -ne 'none') {
    Write-Host 'Generating the TPK ini'
    New-Item -ItemType Directory "$PackageDir\GLOBAL" | Out-Null
    & "$PSScriptRoot\gen-tpk-ini.ps1" $ButtonsDir "$ButtonsDir\8A69054A.ini"
    # Downloaded and hash-checked, not stored in git
    & "$PSScriptRoot\fetch-xnfstpktool.ps1"
    Write-Host 'Building TPK with XNFSTPKTool'
    Invoke-Native { xnfstpktool "-$TpkMode" "$ButtonsDir\8A69054A.ini" "$PackageDir\GLOBAL\XtendedInputNGButtons.tpk" }
}

# SDL3 runtime for the optional SDL backend (downloaded and hash-checked, not stored in git)
& "$PSScriptRoot\fetch-sdl3.ps1"
Copy-Item 'data\SDL3\SDL3.dll' $Scripts

Write-Host 'Summoning text files'
Copy-Item 'LICENSE' $PackageDir
Copy-Item "data\Config-$GameName\XtendedInputNG.ini" $Scripts
Copy-Item "data\Config-$GameName\XtendedInputNG.default.ini" $Scripts

if ($TpkMode -ne 'none') {
    Write-Host 'Summoning additional resources'
    Copy-Item 'data\Mouse Cursor\nfs_cursor.cur' $Scripts
}

Write-Host 'Post build done!'
