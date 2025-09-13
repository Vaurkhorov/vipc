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
#include "ProcMap.h"

#define PROC_NAME "vipc"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaurkhorov");
MODULE_DESCRIPTION("IPC LKM");
MODULE_VERSION("0.1");

static int __init ipc_start(void)
{
    printk(KERN_INFO "Loading hello module...\n");
    printk(KERN_INFO "Hello world\n");
    return 0;
}

static void __exit ipc_end(void)
{
    printk(KERN_INFO "Closing\n");
}

module_init(ipc_start);
module_exit(ipc_end);
