#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>


asmlinkage int sys_proc_time(unsigned long __user *sec , unsigned long __user *nsec){
	struct timespec t;
	getnstimeofday(&t);

	copy_to_user(sec , &t.tv_sec , sizeof(unsigned long));
	copy_to_user(nsec , &t.tv_nsec , sizeof(unsigned long));

	return 0;


}
