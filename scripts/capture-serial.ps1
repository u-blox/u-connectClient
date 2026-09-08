param(
    [string]$PortName = "COM43",
    [int]$Baud = 115200,
    [int]$Seconds = 35,
    [string]$LogFile = "capture.log"
)

$port = New-Object System.IO.Ports.SerialPort $PortName, $Baud, ([System.IO.Ports.Parity]::None), 8, ([System.IO.Ports.StopBits]::One)
$port.ReadTimeout = 500
$port.Open()

$sw = [System.Diagnostics.Stopwatch]::StartNew()
Remove-Item -Path $LogFile -ErrorAction SilentlyContinue

while ($sw.Elapsed.TotalSeconds -lt $Seconds) {
    try {
        $line = $port.ReadLine()
        Write-Output $line
        Add-Content -Path $LogFile -Value $line
    } catch [System.TimeoutException] {
        continue
    }
}

$port.Close()
