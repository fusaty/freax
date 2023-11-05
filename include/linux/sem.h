/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_SEM_H
#define _freax_SEM_H

#include <uapi/freax/sem.h>

struct task_struct;
struct sem_undo_list;

#ifdef CONFIG_SYSVIPC

struct sysv_sem {
	struct sem_undo_list *undo_list;
};

extern int copy_semundo(unsigned long clone_flags, struct task_struct *tsk);
extern void exit_sem(struct task_struct *tsk);

#else

struct sysv_sem {
	/* empty */
};

static inline int copy_semundo(unsigned long clone_flags, struct task_struct *tsk)
{
	return 0;
}

static inline void exit_sem(struct task_struct *tsk)
{
	return;
}
#endif

#endif /* _freax_SEM_H */
