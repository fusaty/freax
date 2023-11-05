#ifndef _freax_LOCKDEP_H
#define _freax_LOCKDEP_H

#include <freax/spinlock.h>

struct lock_class_key {
	unsigned int a;
};

static inline void lockdep_set_class(spinlock_t *lock,
					struct lock_class_key *key)
{
}

extern int lockdep_is_held(const void *);
#endif /* _freax_LOCKDEP_H */
