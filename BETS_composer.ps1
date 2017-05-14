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

$baseLocation = Get-Location

"Date: " + (Get-Date -format "yyyy-MM-dd
") | Out-File "BETS_composer.log" -Encoding ASCII

function Print {
  process {
    $input | % {
      Write-Host $_
      $_ | Out-File (Join-Path -Path $baseLocation -ChildPath "BETS_composer.log") -append -Encoding UTF8 | Out-Null
    }
  }
}
function Log {
  process {
    $input | Out-File (Join-Path -Path $baseLocation -ChildPath "BETS_composer.log") -append -Encoding UTF8 | Out-Null
  }
}

$betsArchiveName = Get-Date -format "BETS_yyyy-MM-dd.\zip"

"Composing: " + $betsArchiveName | Print
"" | Print

Remove-Item $betsArchiveName -force -ErrorAction SilentlyContinue
Remove-Item .\tmp_BETS\ -recurse -force -ErrorAction SilentlyContinue

if($springData -eq $NULL -or $springSource -eq $NULL) {
  "-- Locating SpringData and Spring source" | Print
  [xml]$propsDocument = Get-Content -Path $springPathsProps
  $pathsNode = $propsDocument.Project.PropertyGroup | Where { $_.Label -eq "UserMacros" }
  
  if($springData -eq $NULL) {
    $springData = $pathsNode.SPRINGDATA_DIR
  }
  if($springSource -eq $NULL) {
    $springSource = $pathsNode.SPRING_SOURCE_DIR
  }
}
"SpringData path: " + $springData | Print
"Spring source path: " + $springSource | Print

"" | Print



"-- Compiling BtEvaluator" | Print

Set-Location $springSource\build

mingw32-make -j4 BtEvaluator 2>&1 | Log
$version = cat .\AI\Skirmish\BtEvaluator\VERSION

Set-Location $baseLocation

"" | Print



"-- Creating a copy of BtEvaluator" | Print

New-Item .\tmp_BETS\SpringData\LuaUI\Widgets -type directory | Log
New-Item .\tmp_BETS\SpringData\AI\Skirmish\BtEvaluator\$version -type directory | Log
Copy-Item $springSource\build\AI\Skirmish\BtEvaluator\SkirmishAI.dll .\tmp_BETS\SpringData\AI\Skirmish\BtEvaluator\$version | Log
Copy-Item $springSource\AI\Skirmish\BtEvaluator\data\*.lua .\tmp_BETS\SpringData\AI\Skirmish\BtEvaluator\$version | Log

"" | Print



"-- Checking out the BETS repository:" | Print

Set-Location .\tmp_BETS\SpringData\LuaUI

git clone --recursive https://github.com/MartinFrancu/BETS.git Widgets\ 2>&1 | Log
Remove-Item .\Widgets\.git -recurse -force | Log
Remove-Item .\Widgets\.gitignore -force | Log


Set-Location $baseLocation

"" | Print


<# Excluding DLLs as they are no longer necessary
"-- (temporary) Adding dlls from MinGW" | Print
$engineVersion = (Get-ChildItem $springData\engines | Sort-Object -Property @{Expression={[double]$_.Name}; Ascending = $False} | Select -first 1).Name
$mingwPath = Split-Path -Path (Get-Command mingw32-make).Definition
"MinGW path: " + $mingwPath | Print
New-Item .\tmp_BETS\SpringData\engines\$engineVersion -type directory | Log
Copy-Item (Join-Path -Path $mingwPath -ChildPath *.dll) .\tmp_BETS\SpringData\engines\$engineVersion | Log
 
"" | Print
 #>



"-- Adding README:" | Print

dir .\BETS_readme.* | % {
  Copy-Item (Resolve-Path $_) (".\tmp_BETS\" + $_.Name) | Log
}

"" | Print



"-- Composing filelist:" | Print

Set-Location .\tmp_BETS

Get-ChildItem -Path . -Recurse | Where { !$_.PSIsContainer } | foreach { (($_.Fullname | Resolve-Path -Relative) -replace "\\", "/") -replace "^./", "" } | Out-File "BETS_filelist.txt" -Encoding ASCII

Set-Location $baseLocation

"" | Print


"-- Packing into archive" | Print

if( Get-Command jar -ErrorAction SilentlyContinue ) {
  Set-Location .\tmp_BETS
  jar -cMf ..\$betsArchiveName .\* 2>&1 | Log
} else { if( Get-Command zip -ErrorAction SilentlyContinue ) {
  Set-Location .\tmp_BETS
  zip -r ..\$betsArchiveName .\* 2>&1 | Log
} else {
  Add-Type -A System.IO.Compression.FileSystem
  [IO.Compression.ZipFile]::CreateFromDirectory('tmp_BETS', $betsArchiveName)
} }

Set-Location $baseLocation

"" | Print



"-- Cleaning up" | Print

Remove-Item .\tmp_BETS\ -recurse -force | Log