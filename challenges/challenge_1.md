# Challenge 1 : Linux Service Management

## My Approach

I completed this challenge on an **Arch Linux** system running the Niri window manager. I selected the **`nvidia-powerd`** daemon for this demonstration. This specific service handles NVIDIA Dynamic Boost, which manages and balances power limits between the CPU and GPU in real-time. 

Because it runs within user space while maintaining a system DBus connection and interacting with hardware telemetry via `systemd`, it serves as a highly visible and predictable target to showcase service states, process tracking, and signal management on a modern Linux subsystem.

## Step-by-Step Solution

### Step 1 : Check if the service is running and find its PID

First, I checked the initialization status of the target power daemon using `systemctl` to ensure it was running and to locate its process ID.

```zsh
systemctl status nvidia-powerd
```

**Output:**

```text
● nvidia-powerd.service - nvidia-powerd service
     Loaded: loaded (/usr/lib/systemd/system/nvidia-powerd.service; enabled
; preset: disabled)
     Active: active (running) since Sun 2026-05-24 12:58:37 IST; 14min ago
 Invocation: a4388d949dbe40a1a4becec67e64ce6f
   Main PID: 785 (nvidia-powerd)
      Tasks: 5 (limit: 28279)
     Memory: 7.2M (peak: 9.2M)
        CPU: 26.086s
     CGroup: /system.slice/nvidia-powerd.service
             └─785 /usr/bin/nvidia-powerd

May 24 12:58:37 0xFe4rLeSS systemd[1]: Started nvidia-powerd service.
May 24 12:58:37 0xFe4rLeSS nvidia-powerd[785]: nvidia-powerd version:2.0 (build 1)
May 24 12:58:37 0xFe4rLeSS nvidia-powerd[785]: DBus Connection is established
```

The service is verified active, and its initial Main PID is `785`.

I also cross-verified this PID tracking via pgrep:

```zsh
pgrep -l nvidia-powerd
```

**Output:**

```text
785 nvidia-powerd
```

**Screenshot:**

![Serivce info](./assets/ch_1/screenshots/service_info.png)

---

### Step 2 : Stop the service

Next, I used administrative privileges to gracefully stop the power manager daemon.

```zsh
sudo systemctl stop nvidia-powerd
```

---

### Step 3 : Confirm it stopped

I verified that the process was completely purged from the system process table and that systemd successfully caught the unit state modification.

```zsh
systemctl is-active nvidia-powerd
```

**Output:**

```text
inactive
```

**Screenshot:**

![Serivce stopped](./assets/ch_1/screenshots/service_stopped.png)\


---

### Step 4 : Start the service again

I instructed systemd to safely spin up a clean instance of the daemon, which allocates a fresh process thread and a brand-new PID.

```zsh
sudo systemctl start nvidia-powerd
```

Now, let's target the newly spawned process descriptor:

```zsh
pgrep -l nvidia-powerd
```

**Output:**

```text
28734 nvidia-powerd
```

---

### Step 5 : Kill the process directly using its PID and check the service status

To simulate an unexpected daemon crash, I bypassed systemctl completely and directed a destructive termination signal (SIGKILL) straight to the new PID using raw process controls.

```zsh
sudo kill -9 28734
```
```zsh
systemctl status nvidia-powerd
```

**Screenshot:**

![Serivce killed](./assets/ch_1/screenshots/service_killed.png)


## Results

The target execution maps demonstrate how systemd effectively structures runtime exceptions when process management tools interact outside standard tracking controls.

To clean up my workspace context and re-establish proper system energy thresholds, I cycled the module manually:

```zsh
sudo systemctl stop nvidia-powerd
sudo systemctl start nvidia-powerd
systemctl is-active nvidia-powerd
```
**Output:**

```text
active
```
