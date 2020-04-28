#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage int sys_proc_print(unsigned long s , unsigned long sn , unsigned long e , unsigned long en , int pid){
	printk("[Project1] %d %lu.%09lu %lu.%09lu\n" , pid , s , sn , e, en);
	return 0;
}
