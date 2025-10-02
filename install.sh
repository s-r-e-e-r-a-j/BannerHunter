#!/bin/bash

# BannerHunter Installer Script
# Works on Linux and Termux (Android)
# Checks dependencies, installs missing packages, and builds BannerHunter

TOOL_NAME="BannerHunter"
SRC_FILE="bannerhunter.c"
BIN_FILE="bannerhunter"

# Detect environment
if [[ "$(uname -o 2>/dev/null)" == "Android" ]]; then
    ENV="termux"
    INSTALL_DIR="$PREFIX/bin"
else
    ENV="linux"
    INSTALL_DIR="/usr/local/bin"
fi

echo "[*] Detected environment: $ENV"
echo "[*] Install directory: $INSTALL_DIR"

# Dependencies
DEPENDENCIES_LINUX=("gcc" "openssl" "libssl-dev")
DEPENDENCIES_TERMUX=("clang" "openssl")

install_linux_packages() {
    PM=""
    if command -v apt >/dev/null 2>&1; then
        PM="apt"
        echo "[*] Using apt"
        apt update -y
    elif command -v pacman >/dev/null 2>&1; then
        PM="pacman"
        echo "[*] Using pacman"
        pacman -Sy --noconfirm
    elif command -v dnf >/dev/null 2>&1; then
        PM="dnf"
        echo "[*] Using dnf"
    elif command -v yum >/dev/null 2>&1; then
        PM="yum"
        echo "[*] Using yum"
    else
        echo "[!] No supported package manager found (apt, pacman, dnf, yum)"
        exit 1
    fi

    # Install only missing packages
    for pkg in "${DEPENDENCIES_LINUX[@]}"; do
        case $PM in
            apt)
                dpkg -s "$pkg" >/dev/null 2>&1 || apt install -y "$pkg"
                ;;
            pacman)
                pacman -Qi "$pkg" >/dev/null 2>&1 || pacman -S --noconfirm "$pkg"
                ;;
            dnf)
                rpm -q "$pkg" >/dev/null 2>&1 || dnf install -y "$pkg"
                ;;
            yum)
                rpm -q "$pkg" >/dev/null 2>&1 || yum install -y "$pkg"
                ;;
        esac
    done
}

install_termux_packages() {
    echo "[*] Using Termux pkg manager"
    pkg update -y
    for pkg in "${DEPENDENCIES_TERMUX[@]}"; do
        dpkg -s "$pkg" >/dev/null 2>&1 || pkg install -y "$pkg"
    done
}

# Ensure Linux user runs as root
if [[ "$ENV" == "linux" && "$EUID" -ne 0 ]]; then
    echo "[!] You must run this script as root on Linux."
    exit 1
fi

# Install dependencies
echo "[*] Checking dependencies..."
if [[ "$ENV" == "linux" ]]; then
    install_linux_packages
else
    install_termux_packages
fi

if [[ "$ENV" == "termux" ]]; then
   # Check if clang is available
   if ! command -v clang >/dev/null 2>&1; then
       echo "[!] clang not found. Make sure it is installed and in PATH."
       exit 1
   fi
else
     # check if gcc is available 
     if ! command -v gcc >/dev/null 2>&1; then
          echo "[!] gcc not found. Make sure it is installed and in PATH."
          exit 1
     fi
fi

# Check source file
if [[ ! -f "$SRC_FILE" ]]; then
    echo "[!] Source file $SRC_FILE not found. Please place it in the same directory."
    exit 1
fi

# Build BannerHunter
echo "[*] Building $TOOL_NAME..."
if [[ "$ENV" == "termux" ]]; then
     clang "$SRC_FILE" -o "$INSTALL_DIR/$BIN_FILE" -lssl -lcrypto
else      
     gcc "$SRC_FILE" -o "$INSTALL_DIR/$BIN_FILE" -lssl -lcrypto
fi

# Check build success
if [[ -f "$INSTALL_DIR/$BIN_FILE" ]]; then
    chmod +x "$INSTALL_DIR/$BIN_FILE"
    echo "[+] Build successful! You can run the tool with '$BIN_FILE' from anywhere."
else
    echo "[!] Build failed. Check compiler output."
    exit 1
fi

echo "[*] Installation completed."
