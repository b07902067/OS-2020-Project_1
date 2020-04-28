# OS_project_1 2020

### install system call and complie kernel
#### 1. Copy `kernel_files/proc_time.c` to `linux-{version}/kernel/proc_time.c` and `kernel_files/proc_print.c` to `linux-{version}/kernel/proc_print.c`
#### 2. Add lines "`obj-y += proc_time.o`"  and "`obj-y += proc_print.o`"in `linux-{version}/kernel/Makefile`
#### 3. Add following in "`linux-{version}/include/linux/syscalls.h`":
```
asmlinkage int sys_proc_time(unsigned long __user *sec, unsigned long __user *nsec);
asmlinkage int sys_proc_print(unsigned long s, unsigned long sn , unsigned long es , unsigned long en , int pid);
```
#### 4. Add  `333 64 proc_time sys_proc_time` and `334 64 proc_print sys_proc_print` in `linux-{version}/arch/x86/entry/syscalls/syscall_64.tbl`
#### 5. make -j4 bzImage
#### 6. make install
#### 7. reboot


### Usage
```
make
./sche < FILE
``` 
