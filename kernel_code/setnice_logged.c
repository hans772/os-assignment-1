// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/sched/prio.h>

SYSCALL_DEFINE1(setnice_logged, int, nice_val)
{
    int old_nice;
    int new_nice;

    if (nice_val < MIN_NICE || nice_val > MAX_NICE)
        return -EINVAL;

    if (!can_nice(current, nice_val))
        return -EPERM;

    int retval = security_task_setnice(current, nice_val);
    if (retval)
        return retval;

    old_nice = task_nice(current);

    set_user_nice(current, (long)nice_val);

    new_nice = task_nice(current);

    const char *niceness = (new_nice < ((MIN_NICE + MAX_NICE) >> 1)) ? "mean >: (" : "nice :)";

    printk(KERN_INFO
           "setnice_logged: pid=%d comm=%s old_nice=%d new_nice=%d\nprocess is now : %s",
           task_pid_nr(current), current->comm, old_nice, new_nice, niceness);

    return 0;
}