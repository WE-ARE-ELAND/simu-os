# SimuOS Acrylic Glass Profile Installer
# Usage: powershell -ExecutionPolicy Bypass -File install-glass.ps1

$ErrorActionPreference = "Stop"

# --- Configuration ---
$simuOsPath     = "$PSScriptRoot\build\launch"
$profileName    = "SimuOS"
$profileGuid    = "{e1f5a5c3-7b4a-4d6e-9c8f-2b3a1e0d5c7f}"
$opacity        = 0.6      # 0.0 (fully transparent) ~ 1.0 (opaque)
$colorScheme    = "One Half Dark"
$fontFace       = "Cascadia Code"
$fontSize       = 12

# --- Locate settings.json ---
$settingsPath = Join-Path $env:LOCALAPPDATA "Packages\Microsoft.WindowsTerminal_8wekyb3d8bbwe\LocalState\settings.json"

if (!(Test-Path $settingsPath)) {
    Write-Host "[!] settings.json not found at:" -ForegroundColor Yellow
    Write-Host "    $settingsPath" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "[!] Open Windows Terminal first to generate it." -ForegroundColor Yellow
    Write-Host "    Then re-run this script." -ForegroundColor Yellow
    exit 1
}

$settings = Get-Content $settingsPath -Raw | ConvertFrom-Json

# --- Check if SimuOS profile already exists ---
$existing = $settings.profiles.list | Where-Object { $_.name -eq $profileName }
if ($existing) {
    Write-Host "[i] SimuOS profile already exists. Updating..." -ForegroundColor Cyan
    $existing.commandline    = "cmd /c `"$simuOsPath`""
    $existing.acrylicOpacity = $opacity
    $existing.useAcrylic     = $true
    $existing.colorScheme    = $colorScheme
    $existing.fontFace       = $fontFace
    $existing.fontSize       = $fontSize
} else {
    Write-Host "[+] Adding SimuOS profile..." -ForegroundColor Green
    $newProfile = [ordered]@{
        guid               = $profileGuid
        name               = $profileName
        commandline        = "cmd /c `"$simuOsPath`""
        hidden             = $false
        acrylicOpacity     = $opacity
        useAcrylic         = $true
        colorScheme        = $colorScheme
        fontFace           = $fontFace
        fontSize           = $fontSize
        suppressApplicationTitle = $false
    }
    $settings.profiles.list += [PSCustomObject]$newProfile
}

# --- Save ---
$settings | ConvertTo-Json -Depth 10 | Set-Content $settingsPath -Encoding UTF8

Write-Host ""
Write-Host "[*] Done!" -ForegroundColor Green
Write-Host ""
Write-Host "[*] To use:"
Write-Host "    1. Open Windows Terminal"
Write-Host "    2. Click the down-arrow in the tab bar"
Write-Host "    3. Select '$profileName'"
Write-Host ""
Write-Host "[*] Settings:"
Write-Host "    Acrylic (glass blur):  ON"
Write-Host "    Opacity:               $opacity"
Write-Host "    Color scheme:          $colorScheme"
Write-Host "    Font:                  $fontFace $fontSize"
Write-Host ""
Write-Host "[*] To adjust opacity live:"
Write-Host "    Open Settings (Ctrl+,) -> Appearance -> Transparency"