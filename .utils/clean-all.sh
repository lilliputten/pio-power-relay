#!/bin/sh
# @desc Clean all temp files
# @changed 2026.10.16, 01:50

scriptsPath=$(dirname "$(echo "$0" | sed -e 's,\\,/,g')")
rootPath=`dirname "$scriptsPath"`
prjPath="$rootPath" # `pwd`

# Import config variables (expected variables `$DIST_REPO` and `$PUBLISH_FOLDER`)...
test -f "$scriptsPath/config.sh" && . "$scriptsPath/config.sh"
test -f "$scriptsPath/config-local.sh" && . "$scriptsPath/config-local.sh"

echo "Clearing the root folder..."
$RMCMD -Rf \
  build \
  .pio \
  .ccls-cache \
  .next \
  .swc \
  node_modules/.cache \
  *.tsbuildinfo \
  local.log* \
  *.log \
  log-* \
  *.py[co] \
  .*sw[op] \
  2> /dev/null \
  ; echo OK

$FINDCMD . \
  -not \( -name '.git' -prune \) \
  -not \( -name '.vscode' -prune \) \
  -not \( -name '.next' -prune \) \
  -not \( -name '.husky' -prune \) \
  -not \( -name '.venv*' -prune \) \
  -not \( -name 'out' -prune \) \
  -not \( -name 'build*' -prune \) \
  -not \( -name '*UNUSED' -prune \) \
  -not \( -name 'publish*' -prune \) \
  -not \( -name 'node_modules' -prune \) \
  -not \( -name '__tests__' -prune \) \
  -not \( -name '__mocks__' -prune \) \
  -not \( -name '__snapshots__' -prune \) \
  \
  \( \
    -name '*~' \
    -o -name '*_' \
    -o -name '*.py[co]' \
    -o -name '.*sw[op]' \
    -o -name '*.bak' \
    -o -name '*.tmp' \
    -o -name '__pycache__' \
  \) \
  -exec $RMCMD -Rvf {} \; \
  ; echo OK

echo "Removing empty folders..."
$FINDCMD . \( \
  -not \( -name '.git' -prune \) \
  -not \( -name '.vscode' -prune \) \
  -not \( -name '.next' -prune \) \
  -not \( -name '.venv*' -prune \) \
  -not \( -name 'out' -prune \) \
  -not \( -name 'build*' -prune \) \
  -not \( -name '*UNUSED' -prune \) \
  -not \( -name 'publish*' -prune \) \
  -not \( -name 'node_modules' -prune \) \
  -type d -empty \
  \) \
  -exec $RMCMD -Rvf {} 2> /dev/null \;

echo OK
