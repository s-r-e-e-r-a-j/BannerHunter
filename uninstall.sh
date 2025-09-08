#!/bin/bash
# Bannerhunter Uninstaller
# Works on Linux and Termux
set -e

TOOL_NAME="bannerhunter"
INSTALL_PATH_LINUX="/usr/local/bin/$TOOL_NAME"
INSTALL_PATH_TERMUX="$PREFIX/bin/$TOOL_NAME"

echo "Detecting environment..."

# Detect Termux or Linux
if [[ -n "$PREFIX" && -d "$PREFIX/bin" && "$PREFIX" == *com.termux* ]]; then
    ENVIRONMENT="termux"
    INSTALL_PATH="$INSTALL_PATH_TERMUX"
else
    ENVIRONMENT="linux"
    INSTALL_PATH="$INSTALL_PATH_LINUX"
fi

echo "Environment: $ENVIRONMENT"

# Linux requires root
if [[ "$ENVIRONMENT" == "linux" && "$EUID" -ne 0 ]]; then
    echo "Error: Please run this uninstaller as root."
    exit 1
fi

# Remove installed binary
if [[ -f "$INSTALL_PATH" ]]; then
    echo "Removing $INSTALL_PATH"
    rm -f "$INSTALL_PATH"
    echo "Bannerhunter has been uninstalled successfully."
else
    echo "Bannerhunter is not installed in $INSTALL_PATH"
fi
