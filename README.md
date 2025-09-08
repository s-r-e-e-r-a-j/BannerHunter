## Bannerhunter
**Bannerhunter** is a powerful banner grabbing tool written in C.
It can connect to services running on different ports, attempt plain TCP grabs, and also perform SSL/TLS handshakes for encrypted services.
The tool sends protocol-appropriate requests (like `HEAD` with `Host` headers for HTTP/HTTPS) to increase the chance of extracting banners.

## Features

- Works on both **Linux** and **Termux**

- Supports **plain TCP** and **SSL/TLS** connections

- Sends proper `Host` headers for HTTP/HTTPS

- Can attempt banner grabs across multiple ports (HTTP, HTTPS, FTP, SSH, SMTP, and custom services).

- Receive timeout handling to prevent hangs when services don’t respond

- Installer and uninstaller scripts for easy setup and removal

## Disclaimer 
BannerHunter is for educational and authorized security testing only. 
Do not use it on systems without permission. The author is not responsible for misuse.

  ## Installation

1. **Clone the repository:**
```bash
git clone https://github.com/s-r-e-e-r-a-j/BannerHunter.git
```
2. **Navigate to the project directory:**
```bash
cd BannerHunter
```
3. **Run the installer to install dependencies and setup the system-wide command (Linux & Termux):**
```bash
sudo bash install.sh
```
The installer will check for required dependencies and install them if missing.

## Usage
```bash
bannerhunter <host> <port>
```
```bash
bannerhunter <host> <port1> <port2> <port3>
```

**Example (Single Port):**
```bash
bannerhunter example.com 80
```
Connects to `example.com` on port `80` and attempts to grab the banner.

**Example (Multiple Ports):**
```bash
bannerhunter example.com 21 22 25 80 443
```
Connects to `example.com` on ports `21`, `22`, `25`, `80`, and `443`, grabbing banners from each service.

## Uninstallation
```bash
sudo bash uninstall.sh
```
This will remove the bannerhunter command from your system(Termux & Linux).

## License
This project is licensed under the MIT License
