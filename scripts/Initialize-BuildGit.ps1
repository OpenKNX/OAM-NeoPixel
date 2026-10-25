# Dot-source before calling the shared release scripts. An Intel PowerShell on
# Apple Silicon launches Apple's Git shim as Intel, but the installed Command
# Line Tools can contain ARM-only libraries. Retry Git natively in that case.
$gitVersion = & git --version 2>&1
if ($LASTEXITCODE -ne 0) {
    if ($IsMacOS) {
        $nativeGitVersion = & /usr/bin/arch -arm64 /usr/bin/git --version 2>&1
        if ($LASTEXITCODE -eq 0) {
            function git {
                & /usr/bin/arch -arm64 /usr/bin/git @args
            }
            Write-Host 'Using native ARM Git for this release build.' -ForegroundColor Cyan
        }
        else {
            throw "Git is unavailable. $gitVersion $nativeGitVersion"
        }
    }
    else {
        throw "Git is unavailable. $gitVersion"
    }
}
