# Generates the XNFSTPKTool ini for the button textures: one section per .dds found under the folder.
#   gen-tpk-ini.ps1 <TexturesFolder> <OutIni>
param(
    [Parameter(Mandatory)][string]$Folder,
    [Parameter(Mandatory)][string]$OutIni
)
$ErrorActionPreference = 'Stop'

# NFS string hash: uppercase the string, then h = c + 33 * h starting from -1 (32-bit)
function Get-NfsHash([string]$Text) {
    [uint32]$h = [uint32]::MaxValue
    foreach ($c in $Text.ToUpperInvariant().ToCharArray()) { $h = [uint32](([uint64]$c + 33 * [uint64]$h) % 4294967296) }
    '{0:X8}' -f $h
}

$Folder = (Resolve-Path $Folder).Path
$tpkName = 'XtendedInputNGButtons'
$lines = [Collections.Generic.List[string]]::new()
$lines.AddRange([string[]]@(
    '[TPK]',
    'Name = XTENDEDINPUTNGBUTTONS',
    'Version = 4',
    "Filename = $tpkName",
    "FilenameHash = 0x$(Get-NfsHash $tpkName)",
    'Animations = 0',
    ''))

# Textures are in subfolders (one per icon set), same order as a recursive directory walk
$dds = Get-ChildItem $Folder -Filter *.dds -Recurse -File | Sort-Object { $_.DirectoryName }, { $_.Name }
foreach ($file in $dds) {
    $name = $file.BaseName
    $lines.AddRange([string[]]@(
        "[$(Get-NfsHash $name)]",
        "File = $($file.FullName)",
        "Name = $name",
        'ClassNameHash = 0x1A93CF',
        'ImageCompressionType = 0x24',
        'PaletteCompressionType = 0x0',
        'NumPaletteEntries = 0',
        'TilableUV = 0',
        'BiasLevel = 0',
        'RenderingOrder = 5',
        'ScrollType = 0',
        'UsedFlag = 0',
        'ApplyAlphaSorting = 0',
        'AlphaUsageType = 2',
        'AlphaBlendType = 1',
        'Flags = 0x0',
        'ScrollTimeStep = 0',
        'ScrollSpeedS = 0',
        'ScrollSpeedT = 0',
        'OffsetS = 0',
        'OffsetT = 256',
        'ScaleS = 256',
        'ScaleT = 0',
        'PixelFormatUnk1 = 0x1',
        'PixelFormatUnk2 = 0x5',
        'PixelFormatUnk3 = 0x6',
        ''))
}
# Resolve against PowerShell's location, .NET's working directory can differ (e.g. under MSBuild)
$OutIni = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutIni)
[IO.File]::WriteAllText($OutIni, ($lines -join "`r`n") + "`r`n", [Text.Encoding]::ASCII)
