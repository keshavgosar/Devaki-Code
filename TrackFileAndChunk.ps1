# TrackAndPush.ps1
# Tracks large files with Git LFS and pushes changes in chunks

$SIZE_LIMIT = 94371840  # 90 MB in bytes
$GITATTR = ".gitattributes"
$CONTENT_DIR = "Content"   
$BRANCH = "main"           

if (-not (Test-Path $GITATTR)) {
    Write-Host "Creating $GITATTR file..."
    New-Item -ItemType File -Path $GITATTR -Force | Out-Null
}

Write-Host "Scanning for files larger than 90MB inside $CONTENT_DIR..."

$trackedCount = 0
$trackedFiles = @()
$CurrentDir = (Get-Location).Path + "\"

Get-ChildItem -Path $CONTENT_DIR -Recurse -File | ForEach-Object {
    if ($_.Length -ge $SIZE_LIMIT) {
        $relPath = $_.FullName.Replace($CurrentDir, "").Replace('\', '/')
        $alreadyTracked = Select-String -Path $GITATTR -Pattern ([regex]::Escape($relPath)) -Quiet
        if (-not $alreadyTracked) {
            Write-Host "Tracking large file: $relPath"
            git lfs track "$relPath" | Out-Null
            $trackedFiles += $relPath
            $trackedCount++
        }
    }
}

if ($trackedCount -gt 0) {
    Write-Host "`nTracked $trackedCount new large file(s) with Git LFS:" -ForegroundColor Green
    $trackedFiles | ForEach-Object { Write-Host $_ }
} else {
    Write-Host "`nNo new files over 90MB needed tracking." -ForegroundColor Yellow
}

Write-Host "`n[Note] Remember to add and commit any changes to $GITATTR." -ForegroundColor Yellow

$files = (git ls-files -m -o --exclude-standard) -split "`n"
$chunkSize = 100
$totalFiles = $files.Count
$chunkNum = 1

for ($i = 0; $i -lt $totalFiles; $i += $chunkSize) {
    Write-Host "Committing chunk $chunkNum..."
    git reset > $null

    for ($j = $i; $j -lt ($i + $chunkSize) -and $j -lt $totalFiles; $j++) {
        git add "$($files[$j])"
    }

    $message = "Devaki Project Chunk $chunkNum"
    git commit -m "$message"

    Write-Host "Pushing chunk $chunkNum to $BRANCH..."
    git push origin $BRANCH

    $chunkNum++
}
