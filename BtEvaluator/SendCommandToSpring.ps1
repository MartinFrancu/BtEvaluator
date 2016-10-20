 param (
    $message
 )

Function Send-StringOverTcp ( 
    [Parameter(Mandatory=$True)][String]$DataToSend,
    [Parameter(Mandatory=$True)][String]$Hostname, 
    [Parameter(Mandatory=$True)][UInt16]$Port)
{
    Try
    {
        $ErrorActionPreference = "Stop"
        $TCPClient  = New-Object Net.Sockets.TcpClient
        $TCPClient.Connect($Hostname, $Port)
        $NetStream  = $TCPClient.GetStream()
        [Byte[]]$Buffer = [Text.Encoding]::ASCII.GetBytes($DataToSend)
        $NetStream.Write($Buffer, 0, $Buffer.Length)
        $NetStream.Flush()
    }
    Catch
    {
    }
    Finally
    {
        If ($NetStream) { $NetStream.Dispose() }
        # If ($TCPClient) { $TCPClient.Dispose() }
    }
}

"Invoking command /" + $message
Send-StringOverTcp -DataToSend $message -Hostname 127.0.0.1 -Port 8201
