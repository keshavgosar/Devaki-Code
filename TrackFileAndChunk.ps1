# TrackLargeFiles.ps1
# Tracks files over 90MB in Project_UE\Content using Git LFS

$SIZE_LIMIT = 94371840  # 90 MB in bytes
$GITATTR = ".gitattributes"
$CONTENT_DIR = "Content"

if (-not (Test-Path $GITATTR)) {
    Write-Host "Creating $GITATTR file..."
    New-Item -ItemType File -Path $GITATTR -Force | Out-Null
}

Write-Host "Scanning for files larger than 90MB inside $CONTENT_DIR..."

$trackedCount = 0
$trackedFiles = @()

# Get absolute current directory to remove from path
$CurrentDir = (Get-Location).Path + "\"

Get-ChildItem -Path $CONTENT_DIR -Recurse -File | ForEach-Object {
    if ($_.Length -ge $SIZE_LIMIT) {
        # Get relative path from current directory, convert backslashes to slashes
        $relPath = $_.FullName.Replace($CurrentDir, "").Replace('\', '/')

        # Check if already tracked in .gitattributes
        $alreadyTracked = Select-String -Path $GITATTR -Pattern ([regex]::Escape($relPath)) -Quiet

        if (-not $alreadyTracked) {
            Write-Host "Tracking large file: $relPath"
            git lfs track "$relPath" | Out-Null
            $trackedFiles += $relPath
            $trackedCount++
        }
    }
}

Write-Host ""

if ($trackedCount -gt 0) {
    Write-Host "Tracked $trackedCount new large file(s) with Git LFS:" -ForegroundColor Green
    $trackedFiles | ForEach-Object { Write-Host $_ }
} else {
    Write-Host "No new files over 90MB needed tracking." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "[Note] Remember to add and commit any changes to $GITATTR." -ForegroundColor Yellow

$files = (git ls-files -m -o --exclude-standard) -split "n"

$chunkSize = 100
$totalFiles = $files.Count
$chunkNum = 1

for ($i = 0; $i -lt $totalFiles; $i += $chunkSize) {
    Write-Host "Committing chunk $chunkNum..."

    # Reset staged changes for safety
    git reset > $null

    # Stage up to $chunkSize files
    for ($j = $i; $j -lt ($i + $chunkSize) -and $j -lt $totalFiles; $j++) {
        git add "$($files[$j])"
    }

    # Commit the chunk
    $message = "Paragon Pack Chunk $chunkNum"
    git commit -m "$message"

    # Push each chunk immediately after commit
    Write-Host "Pushing chunk $chunkNum..."
    git push origin HEAD

    $chunkNum++
}