/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * The memory shared between mwmbench and its probe in mwm
 * (mwmpreload.c).  mwm writes, mwmbench reads; the sequence numbers are
 * futex words that mwm wakes.
 */
#ifndef MWMBENCH_H
#define MWMBENCH_H

struct mwmbench_shm {
	unsigned long mallocs;    /* malloc, calloc and moving realloc calls */
	unsigned long replies;    /* _XReply calls: round trips */
	unsigned long requests;   /* X requests issued when mwm last went idle */
	unsigned int idle_seq;    /* bumped when mwm blocks in the Xt loop */
	unsigned int idle;        /* 1 while it is blocked there */
	unsigned int query_seq;   /* bumped by every XQueryPointer */
	int query_x, query_y;     /* the root position it returned */
	unsigned int grab_seq;    /* bumped by every successful XGrabPointer */
};

#endif /* MWMBENCH_H */
