# Launches a new terminal with relevant tabs

$currentScriptPath = $MyInvocation.MyCommand.Path
$setupPath = (Split-Path -Parent $currentScriptPath) + "\setup_vs_dev_shell.ps1"

wt `
    new-tab `
        --title "Flat Worlds - Neovim" `
        --profile "Windows Powershell" `
        powershell -NoExit -Command "& $setupPath nvim ." `
        `; `
    new-tab `
        --title "Flat Worlds - Terminal" `
        --profile "Windows Powershell" `
        powershell -NoExit -Command "& $setupPath"

