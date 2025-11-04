# 💻 Linux Command Practice



---

## 📘 Overview
This lab includes Bash scripting and command-line exercises covering environment variables, file redirection, searching, linking, and prompt customization.

---

## 🧩 Section 1
**Task:**  
Write a Bash script that checks **if the `.bashrc` file exists** in the user's home directory.  

If it does, append new environment variables to the file:
- `HELLO` → value of `$HOSTNAME`
- `LOCAL` → output of the `whoami` command  

Additionally, the script should include a command to open another terminal at the end.  
Finally, describe what happens when the new terminal is opened.

### ✅ Example Answer
```bash
#!/bin/bash

PATH_BASHRC="$HOME/.bashrc"

if [ -f "$PATH_BASHRC" ]; then
    echo ".bashrc exists in your home directory."
    echo "export HELLO=$HOSTNAME" >> "$PATH_BASHRC"
    echo "LOCAL=$(whoami)" >> "$PATH_BASHRC"
    gnome-terminal &
else
    echo ".bashrc not found in $HOME"
fi
```

**Explanation:**  
When a new terminal opens, Bash automatically sources `.bashrc`.  
As a result, the environment variable `HELLO` and the local variable `LOCAL` are available in the new session.

---

## 🧭 Section 2

### 1️⃣ Question
List the user commands and redirect the output to `/tmp/commands.list`.

#### ✅ Answer
```bash
ls /usr/bin > /tmp/commands.list
```

---

### 2️⃣ Question
Edit your profile to display the date at login and change your prompt permanently.

#### ✅ Answer
```bash
ls -a | grep .profile
nano ~/.profile
```
Add the following lines:
```bash
date
export PS1="\u@\h:\w> "
```
Then reload:
```bash
source ~/.profile
```

---

### 3️⃣ Question
What is the command to count words in a file or number of files in a directory?  
a. Count the number of user commands.

#### ✅ Answer
```bash
wc -w filename.txt         # Count words in a file
ls /usr/bin | wc -l        # Count number of user commands
```

---

### 4️⃣ Question
What happens if you execute:  

a. `cat filename1 | cat filename2`  
b. `ls | rm`  
c. `ls /etc/passwd | wc -l`

#### ✅ Answer
- (a) Concatenates and prints the contents of both files.  
- (b) Dangerous — `rm` will try to delete files whose names come from `ls` output.  
- (c) Counts the number of lines in `/etc/passwd` output (typically returns `1`).

---

### 5️⃣ Question
Write a command to search for all files on the system named `.profile`.

#### ✅ Answer
```bash
sudo find / -name ".profile" 
```

---

### 6️⃣ Question
List the inode numbers of `/`, `/etc`, and `/etc/hosts`.

#### ✅ Answer
```bash
ls -i / /etc /etc/hosts
```

---

### 7️⃣ Question
Create a **symbolic link** of `/etc/passwd` in `/boot`.

#### ✅ Answer
```bash
sudo ln -s /etc/passwd /boot/passwd_link
```

---

### 8️⃣ Question
Create a **hard link** of `/etc/passwd` in `/boot`.  
Could you? Why?

#### ✅ Answer
```bash
sudo ln /etc/passwd /boot/passwd_hard
```
It may **fail** because `/boot` is often on a separate filesystem and partition —  
**hard links can only exist within the same filesystem and in same partition.**

---

### 9️⃣ Question
Execute:
```bash
echo \
```
It will jump to the next line and display the prompt as `>`.  
What is that prompt, and how can you change it from `>` to `:`?

#### ✅ Answer
The `>` prompt means the shell is waiting for command continuation.  
You can change the secondary prompt (`PS2`) by:
```bash
export PS2=": "
```

---


