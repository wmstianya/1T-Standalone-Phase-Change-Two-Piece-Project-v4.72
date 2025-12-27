param(
    [Parameter(Mandatory = $true)]
    [string]$Port,

    [int]$Baud = 115200,

    [string]$LogPath = ".\.cursor\debug.log"
)

$serial = New-Object System.IO.Ports.SerialPort $Port, $Baud, "None", 8, "One"
$serial.NewLine = "`n"
$serial.ReadTimeout = 500
$serial.WriteTimeout = 500

try {
    $serial.Open()
    Write-Host "Serial capture started. Port=$Port Baud=$Baud LogPath=$LogPath"
    Write-Host "Press Ctrl+C to stop."

    while ($true) {
        try {
            $line = $serial.ReadLine()
            if ($null -ne $line) {
                $trim = $line.Trim()
                if ($trim.StartsWith("{") -and $trim.EndsWith("}")) {
                    # Ensure NDJSON is written as UTF-8 (avoid WindowsPowerShell default UTF-16)
                    [System.IO.File]::AppendAllText($LogPath, $trim + "`n", [System.Text.Encoding]::UTF8)
                }
            }
        } catch [System.TimeoutException] {
            # keep looping
        }
    }
}
finally {
    if ($serial.IsOpen) { $serial.Close() }
}


