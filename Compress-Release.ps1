$hwcRoot = $env:HWC_Root
$cetRoot = $env:CET_Root

$files = @(
    (Join-Path $hwcRoot "cataclysm_loader.exe"), 
    (Join-Path $hwcRoot "CataclysmEngineTweaks.dll"), 
    (Join-Path $cetRoot "CataclysmEngineTweaks.ini")
    (Join-Path $hwcRoot "rgl.dll"), 
    (Join-Path $hwcRoot "rgld3d.dll")
    (Join-Path $hwcRoot "update102.big")) 

$compress = @{
    LiteralPath      = $files
    CompressionLevel = "Optimal"
    DestinationPath  = (Join-Path $hwcRoot "cet.zip")
}

Compress-Archive @compress