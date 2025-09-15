/**
 * @file    main.c
 * @author  Vaurkhorov
 * @date    13 Sept 2025
 * @version 0.1
*/
#include <linux/module.h>     
#include <linux/kernel.h>     // KERN_INFO 
#include <linux/init.h>       // macros
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include "ProcMap.h"

#define PROC_NAME "vipc"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaurkhorov");
MODULE_DESCRIPTION("IPC LKM");
MODULE_VERSION("0.1");

static ProcMap *pm;
static struct proc_dir_entry *procfile;

int ipc_read

static int __init ipc_start(void)
{
    printk(KERN_INFO "Initialising map\n");

    procfile = create_proc_entry(PROC_NAME, 0666, NULL);
    if (procfile == NULL) {
        remove_proc_entry(PROC_NAME, &proc_root);
        printk(KERNEL_ALERT "Error: could not initialise /proc file");
        return -ENOMEM;
    }

//    procfile->
    
    pm = new_pm();
    if (pm == NULL) {
        return -ENOMEM;
    }
    
    printk(KERN_INFO "Module loaded\n");
    return 0;
}

static void __exit ipc_end(void)
{
    printk(KERN_INFO "Closing\n");
    kfree(pm);
}

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
    int return_length = 0;

    pid_t receiver_pid = 0;

    ErrorCode code = pm_add(pm, buffer, length, receiver_pid, sender_pid)
}

module_init(ipc_start);
module_exit(ipc_end);
