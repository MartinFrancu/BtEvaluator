if( $Host -and $Host.UI -and $Host.UI.RawUI ) {
  $rawUI = $Host.UI.RawUI
  $oldSize = $rawUI.BufferSize
  $typeName = $oldSize.GetType( ).FullName
  $newSize = New-Object $typeName (500, $oldSize.Height)
  $rawUI.BufferSize = $newSize
}

foreach ($i in $input) {
    $i -replace '^(In file included from )?(.*?):(\d+):(\d+): ', '$2($3,$4): $1'
}