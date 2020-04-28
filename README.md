# OS_project_1 2020
####  NOTE: The program is based on 1-processor virtual machine. The result will be uncertain in virtual machine with multiple processors.
### install system call and complie kernel
##### 1. .c file
- Copy `kernel_files/proc_time.c` to `linux-{version}/kernel/proc_time.c` 
- Copy `kernel_files/proc_print.c` to `linux-{version}/kernel/proc_print.c`
##### 2. Add following lines to  `linux-{version}/kernel/Makefile`
```
obj-y += proc_time.o
obj-y += proc_print.o
```
##### 3. Add following lines in "`linux-{version}/include/linux/syscalls.h`"
```
asmlinkage int sys_proc_time(unsigned long __user *sec, unsigned long __user *nsec);
asmlinkage int sys_proc_print(unsigned long s, unsigned long sn , unsigned long es , unsigned long en , int pid);
```
##### 4. Add  following lines to  `linux-{version}/arch/x86/entry/syscalls/syscall_64.tbl`
```
333 64 proc_time sys_proc_time
334 64 proc_print sys_proc_print
```
##### 5. make -j4 bzImage
##### 6. make install
##### 7. reboot


### Usage
```
make
./sche < FILE
``` 
