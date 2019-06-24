/*************************************************************************************************************
   Kernel samples by Nagesh N.
   This module Demo for dynamically override ext4 write operation in UBUNTU Kernel 3.13.11.
   I recompiled the kerenel by taking out constant qualifier for file_operations of ext4.
   You could do RW pages, using set_memory_rw , which allow pages to do rw. 
   And also recent kernels default with CONFIG_STRICT_RWX Which will not allow page attributes change. 

   Module: efop.c
   Test Steps: compile, insmod, dmesg, echo "HI" > nagesh, dmesg. Chk printk in dmesg and /var/log/system.log
*************************************************************************************************************/

#include <linux/module.h>    
#include <linux/moduleparam.h>
#include <linux/kernel.h>    
#include <linux/init.h>     
#include <linux/aio.h>     
#include <linux/fs.h>     
#include <linux/types.h>  
#include <linux/kallsyms.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nagesh_Nanjundachari");
MODULE_DESCRIPTION("Simple demo to CHECK AT Ext4FileWrite");

typedef struct  file_operations * fops;
typedef ssize_t (*cpwrite)(struct file *filp, const char __user *buf, size_t len, loff_t *ppos); 
typedef ssize_t (*cpaio_write)(struct kiocb *iocb, const struct iovec *iov, unsigned long nr_segs, loff_t pos);
typedef ssize_t (**ppwrite)(struct file *filp, const char __user *buf, size_t len, loff_t *ppos); 
typedef ssize_t (**ppaio_write)(struct kiocb *iocb, const struct iovec *iov, unsigned long nr_segs, loff_t pos);
static ssize_t  _aio_write(struct kiocb *iocb, const struct iovec *iov, unsigned long nr_segs, loff_t pos);
static ssize_t  _write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos);
void assign_fp( fops * fp); 

static cpwrite owrite;
static cpaio_write oaio_write;
static ppwrite write;
static ppaio_write aio_write;
static fops fp;

static ssize_t _write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos) {

	if ( !strcmp(filp->f_path.dentry->d_name.name , "nagesh"))  {
	    printk("nagesh_file_not_written\n");
	    printk("...................\n");
            return -EINVAL;
        }
	    return owrite(filp, buf, len, ppos); 
}

static ssize_t _aio_write(struct kiocb *iocb, const struct iovec *iov, unsigned long nr_segs, loff_t pos) {
struct file *filp;

            filp = iocb->ki_filp;
	    if ( !strcmp(filp->f_path.dentry->d_name.name , "nagesh"))  {
	        printk("aio.nagesh_file_not_written\n");
	        printk("...................\n");
                return -EINVAL;
            }
            return oaio_write(iocb,  iov,  nr_segs, pos);
}


void assign_fp( fops * fp) {

    owrite = (*fp)->write;
    oaio_write = (*fp)->aio_write;
    write = &(*fp)->write;
    aio_write = &(*fp)->aio_write;
    preempt_disable();
    *write = _write;
    *aio_write = _aio_write;
    preempt_enable();
}

static int __init efop_init(void)
{
    printk(KERN_INFO "EFOP_ONLOAD.\n");
    fp = (fops)kallsyms_lookup_name("ext4_file_operations");
    assign_fp(&fp);
    printk(KERN_INFO "EFOP_LOADED.\n");

    return 0;    
}

static void __exit efop_exit(void)
{
    preempt_disable();
    *write=owrite;
    *aio_write=oaio_write;
    preempt_enable();
    printk(KERN_INFO "EFOP_EXIT.\n");
}

module_init(efop_init);
module_exit(efop_exit);
