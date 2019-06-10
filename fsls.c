/*************************************************************************************************************
   Kernel samples by Nagesh N.
   This module Demo for listing FS in UBUNTU kernel 3.13.0-32 and 3.13.11.

   Module: fsls.c
   Test Steps: compile, load, dmesg . Chk printk in dmesg and /var/log/system.log
*************************************************************************************************************/

#include <linux/module.h>    
#include <linux/kernel.h>    
#include <linux/init.h>     
#include <linux/fs.h>     
#include <linux/types.h>  
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nagesh_Nanjundachari");
MODULE_DESCRIPTION("Simple Module Print FileSystems");

static int __init fsls_init(void)
{
int i=0;   
struct file_system_type **ptr_fs1, *ptr_fs2;

    ptr_fs1 = (struct file_system_type **)kallsyms_lookup_name("file_systems");
    ptr_fs2 = *ptr_fs1; 
    printk(KERN_INFO "FS_LIST :\n");
    printk(KERN_INFO "..................\n");
    while ( ptr_fs2 != NULL) {
	    printk("FS%d %s\n", i++, ptr_fs2->name);
	     ptr_fs2 = ptr_fs2->next;
    }

    return 0;    
}

static void __exit fsls_exit(void)
{
    printk(KERN_INFO "EXIT FSLS.\n");
}

module_init(fsls_init);
module_exit(fsls_exit);
