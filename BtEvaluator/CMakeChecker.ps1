$a = Get-Item ../CMakeLists.txt
$b = Get-Item ../src/*.cpp | sort {$_.CreationTime} -desc | select -first 1
if( $($a.LastWriteTime) -lt $($b.CreationTime) )
{
  $a.LastWriteTime = $b.CreationTime
}