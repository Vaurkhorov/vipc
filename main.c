/**
* @file    main.c
* @author  Vaurkhorov
* @date    13 Sept 2025
* @version 0.2
*/
#include <linux/module.h>     
#include <linux/kernel.h>     // KERN_INFO 
#include <linux/init.h>       // macros
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/stddef.h>
#include <linux/types.h>
#include "ProcMap.h"

#define PROC_NAME "vipc"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaurkhorov");
MODULE_DESCRIPTION("IPC LKM");
MODULE_VERSION("0.1");

static ProcMap *pm;
static struct proc_dir_entry *procfile;

static ssize_t read_message(
    struct file *file,
    char *buffer,
    size_t length,
    loff_t *offset
) {
    pid_t receiver_pid = current->pid;
    int return_length = 0;
    
    ErrorCode code = pm_consume(pm, buffer, length, receiver_pid, &return_length);
    if (code != Ok) {
        return 0;
    } else {
        return return_length;
    }
}

static ssize_t write_message(
    struct file* file,
    const char *buffer,
    size_t length,
    loff_t *offset
) {
    pid_t sender_pid = current->pid;
    const int PID_T_SIZE = sizeof (pid_t);
    
    if (length < PID_T_SIZE) {
        printk(KERN_ALERT "Not enough data written to vipc.\n");
        return -EINVAL;
    }
    
    pid_t receiver_pid;
    if (copy_from_user(&receiver_pid, (pid_t __user *)buffer, PID_T_SIZE)) {
        printk(KERN_ALERT "Receiver PID wasn't copied.\n");
        return -EFAULT;
    }
    size_t msg_length = length - PID_T_SIZE;
    
    if (copy_from_user(msg, buffer, length)) {
    
    char *msg = kcalloc(1, msg_length + 1, GFP_KERNEL);
    
    if (copy_from_user(msg, buffer + PID_T_SIZE, msg_length)) {
        printk(KERN_ALERT "Data wasn't copied.\n");
        return -EFAULT;
    }

    
    ErrorCode code = pm_add(pm, msg, msg_length, receiver_pid, sender_pid);
    
    if (code != Ok) {
        return code;    // ensure that the enum values are errno ints
    }
    
    return length;
}

const struct proc_ops proc_vops = {
    .proc_read = read_message,
    .proc_write = write_message,
};

static int __init ipc_start(void)
{
    printk(KERN_INFO "Initialising map\n");
    procfile = proc_create(PROC_NAME, 0666, NULL, &proc_vops);
    if (procfile == NULL) {
        remove_proc_entry(PROC_NAME, NULL);
        printk(KERN_ALERT "Error: could not initialise /proc file.\n");
        return -ENOMEM;
    }
    
    pm = new_pm();
    if (pm == NULL) {
        return -ENOMEM;
    }
    
    printk(KERN_INFO "vipc module loaded.\n");
    return 0;
}

static void __exit ipc_end(void)
{
    printk(KERN_INFO "Closing vipc.\n");
    remove_proc_entry(PROC_NAME, NULL);
    kfree(pm);
}

module_init(ipc_start);
module_exit(ipc_end);
