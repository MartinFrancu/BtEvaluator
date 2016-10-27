param (
  $springPathsProps = ".\BtEvaluator\SpringPaths.props.user",
  $springData,
  $springSource
)
<# Purpose of this script is to create archive of all files in BETS project to 
distribute them to user. #>
<# The script assumes to be run in the home folder of NOTA. It creates new 
temporary folder structure which mimicks the nota structure and copy files of 
BtEvaluator to corresponding places and then checkout the BETS repositor,
again to corresponding place.#>

$betsArchiveName = Get-Date -format "BETS_yyyy-MM-dd.\zip"
$baseLocation = Get-Location

Write-Host $betsArchiveName

Remove-Item $betsArchiveName -force -ErrorAction SilentlyContinue
Remove-Item .\tmp_BETS\ -recurse -force -ErrorAction SilentlyContinue

if($springData -eq $NULL -or $springSource -eq $NULL) {
  Write-Host "-- Locating SpringData and Spring source"
  [xml]$propsDocument = Get-Content -Path $springPathsProps
  $pathsNode = $propsDocument.Project.PropertyGroup | Where { $_.Label -eq "UserMacros" }
  
  if($springData -eq $NULL) {
    $springData = $pathsNode.SPRINGDATA_DIR
  }
  if($springSource -eq $NULL) {
    $springSource = $pathsNode.SPRING_SOURCE_DIR
  }
}
Write-Host "SpringData path: " $springData
Write-Host "Spring source path: " $springSource




Write-Host "-- Compiling BtEvaluator"

Set-Location $springSource\build

mingw32-make -j4 BtEvaluator
$version = cat .\AI\Skirmish\BtEvaluator\VERSION

Set-Location $baseLocation





Write-Host "-- Creating a copy of BtEvaluator"

New-Item .\tmp_BETS\SpringData\LuaUI\Widgets -type directory
New-Item .\tmp_BETS\SpringData\AI\Skirmish\BtEvaluator\$version -type directory
Copy-Item $springSource\build\AI\Skirmish\BtEvaluator\SkirmishAI.dll .\tmp_BETS\SpringData\AI\Skirmish\BtEvaluator\$version
Copy-Item $springSource\AI\Skirmish\BtEvaluator\data\*.lua .\tmp_BETS\SpringData\AI\Skirmish\BtEvaluator\$version





Write-Host "-- Checking out the BETS repository:"

Set-Location .\tmp_BETS\SpringData\LuaUI

git clone --recursive https://github.com/MartinFrancu/BETS.git Widgets\
Remove-Item .\Widgets\.gitignore -force

Set-Location $baseLocation



Write-Host "-- (temporary) Adding dlls from MinGW"
$engineVersion = (Get-ChildItem $springData\engines | Sort-Object -Property @{Expression={[double]$_.Name}; Ascending = $False} | Select -first 1).Name
$mingwPath = Split-Path -Path (Get-Command mingw32-make).Definition
Write-Host "MinGW path: " $mingwPath
New-Item .\tmp_BETS\SpringData\engines\$engineVersion -type directory
Copy-Item (Join-Path -Path $mingwPath -ChildPath *.dll) .\tmp_BETS\SpringData\engines\$engineVersion



Write-Host "-- Packing into archive"

if( Get-Command jar -ErrorAction SilentlyContinue ) {
  Set-Location .\tmp_BETS
  jar -cMf ..\$betsArchiveName .\SpringData\*
} else { if( Get-Command zip -ErrorAction SilentlyContinue ) {
  Set-Location .\tmp_BETS
  zip -r ..\$betsArchiveName .\SpringData\*
} else {
  Add-Type -A System.IO.Compression.FileSystem
  [IO.Compression.ZipFile]::CreateFromDirectory('tmp_BETS', $betsArchiveName)
} }

Set-Location $baseLocation





Write-Host "-- Cleaning up"

Remove-Item .\tmp_BETS\ -recurse -force