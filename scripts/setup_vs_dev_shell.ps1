# Sets up a VS dev shell in the current terminal
# Helper for launch_dev_env.ps1

$vsdevshell="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1"

$currentScriptPath = $MyInvocation.MyCommand.Path
$repoPath= (Split-Path -Parent $currentScriptPath) + "\.."

& $vsdevshell
cd $repoPath
clear

# Check for optional command (typically Neovim)
if ($args.Count -ge 1) {
    $command = $args[0]
    $commandArgs = $args[1..($args.Length - 1)]
    & $command @commandArgs
}
