# Builds every configuration (or the ones given as arguments) and prints a one-line result for each.
#   build-all.ps1                      all Debug/Release configs
#   build-all.ps1 Release-MW Debug-UC  just these
# Set $env:MSBUILD to override the MSBuild.exe found through vswhere.
param([Parameter(ValueFromRemainingArguments)][string[]]$Configs)

if (-not $env:MSBUILD) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    $env:MSBUILD = & $vswhere -latest -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
}
if (-not $env:MSBUILD) {
    Write-Host 'MSBuild.exe not found, set the MSBUILD environment variable'
    exit 2
}

$project = Join-Path $PSScriptRoot '..\XtendedInputNG.vcxproj'
if (-not $Configs) {
    $Configs = foreach ($type in 'Debug', 'Release') {
        foreach ($game in 'MW', 'Carbon', 'ProStreet', 'UC', 'World', 'UG', 'UG2') { "$type-$game" }
    }
}

$failed = 0
foreach ($cfg in $Configs) {
    $log = & $env:MSBUILD $project "-p:Configuration=$cfg" -p:Platform=Win32 -v:m -nologo 2>&1 | Out-String -Stream
    if ($LASTEXITCODE -ne 0) {
        Write-Host "FAIL  $cfg"
        $log | Select-String ' error ' | Select-Object -Unique -First 15 | ForEach-Object { $_.Line }
        $failed = 1
    } else {
        $warnings = @($log | Select-String 'warning C').Count
        Write-Host "OK    $cfg ($warnings warning lines)"
    }
}
exit $failed
