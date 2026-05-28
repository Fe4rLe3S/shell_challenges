# 🔎 Threat Detection Engine (YARA)

A lightweight malware classification and detection engine using **YARA**. This project demonstrates how to write custom rules to identify common malicious indicators, such as credential dumpers, unauthorized sensitive file access, and reverse shell payloads.

## 1. Installation

YARA is a standard tool in the cybersecurity industry and is widely supported by most package managers.
Since I am using Arch Linux, I will install it as follows:

```bash
# Arch Linux
sudo pacman -S yara
```

We can verify the installation by running `yara --version` in the terminal.

## 2. The Detection Rules (`rules.yar`)

Below is our ruleset `rules.yar` designed to catch three specific threats.

```yara
rule Detect_Mimikatz {
    meta:
        author = "Fe@rLeSS"
        description = "Detects the presence of the mimikatz process name or string"
    strings:
        $mimikatz_str = "mimikatz" nocase
    condition:
        $mimikatz_str
}

rule Detect_Shadow_Path {
    meta:
        author = "Fe@rLeSS"
        description = "Detects references to the Linux shadow password file"
    strings:
        $shadow_path = "/etc/shadow"
    condition:
        $shadow_path
}

rule Detect_Reverse_Shell {
    meta:
        author = "Fe@rLeSS"
        description = "Detects strings indicating a reverse shell payload"
    strings:
        $rev_shell = "reverse_shell" nocase
    condition:
        $rev_shell
}

```

> 'nocase' makes it case-insensitive (matches Mimikatz, MIMIKATZ, etc.)

## 3. Testing the Engine

To verify our rules work, we need a mock "malicious" file.

**Step 1:** A file named `sample_payload.txt` is created with this simulated attack script:

```bash
#!/bin/bash
echo "Attempting to execute mimikatz.exe in memory..."
cat /etc/shadow > /tmp/creds.txt
python -c 'import pty; pty.spawn("/bin/bash")' # reverse_shell initiated

```

**Step 2:** Run YARA against the file.
*(The `-s` flag tells YARA to print exactly which string triggered the alert, making it great for debugging).*

```bash
yara -s rules.yar sample_payload.txt

```

**Step 3:** Verify the results.

```text
Detect_Mimikatz sample_payload.txt
0x28:$mimikatz_str: mimikatz
Detect_Shadow_Path sample_payload.txt
0x47:$shadow_path: /etc/shadow
Detect_Reverse_Shell sample_payload.txt
0x95:$rev_shell: reverse_shell
```

## 4. Technical Concepts

### 📄 File Matching vs. ⚙️ Process Matching

* **File Matching (`yara rules.yar target_file`):** This analyzes static data resting on a physical hard drive. It is incredibly fast but can be evaded by attackers who use encryption, packing, or obfuscation to hide their code on the disk.
* **Process Matching (`yara rules.yar <PID>`):** This analyzes the volatile memory (RAM) of a program that is actively running. This is highly effective against sophisticated malware. Even if a virus is heavily encrypted on the hard drive, it *must* decrypt itself in memory to run—allowing YARA to catch the raw, exposed malicious strings.

---

### Dynamic Rule Updates (Zero-Downtime)

In a production environment, you cannot afford to turn off your security software just to add a new rule. To add rules without restarting a running service, you can use these architectural patterns:

1. **File Watchers (`inotify`):** The security service monitors the `rules/` directory. When a new `.yar` file is dropped in, the operating system alerts the service, triggering an automatic reload in the background.
2. **Atomic Pointer Swapping (C/C++ `libyara`):** The active scanning engine runs on a dedicated background thread. When new rules arrive, a separate thread compiles them into a *new* scanner object. Once compilation is perfectly complete, the service swaps the active pointer to the new scanner in a single CPU cycle. Active file scans finish using the old rules, and all new scans instantly use the updated ruleset without dropping a single request.
