## Bannerhunter
**Bannerhunter** is a powerful banner grabbing tool written in C.
It can connect to services running on different ports, attempt plain TCP grabs, and also perform SSL/TLS handshakes for encrypted services.
The tool sends protocol-appropriate requests (like `HEAD` with `Host` headers for HTTP/HTTPS) to increase the chance of extracting banners.

## Features

- Works on both **Linux** and **Termux**

- Supports **plain TCP** and **SSL/TLS** connections

- Sends proper `Host` headers for HTTP/HTTPS

- Works across multiple ports (HTTP, HTTPS, FTP, SSH, SMTP, custom services, etc.)

- Timeout handling for dead or filtered ports

- Installer and uninstaller scripts for easy setup and removal
