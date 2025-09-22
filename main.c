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

    pid_t receiver_pid = 0;
    const char *filename = file->f_path.dentry->d_name.name;
    int error = kstrtoint(filename, 10, &receiver_pid);
    if (error) {
        return -EBADF;
    }

    char *msg = kcalloc(1, length, GFP_KERNEL);
    if (copy_from_user(msg, buffer, length)) {
        return -EFAULT;
    }

    ErrorCode code = pm_add(pm, buffer, length, receiver_pid, sender_pid);

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
        remove_proc_entry(PROC_NAME, procfile);
        printk(KERN_ALERT "Error: could not initialise /proc file");
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

module_init(ipc_start);
module_exit(ipc_end);
