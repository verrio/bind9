/*
 * Copyright (C) 1998  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef ISC_RESULT_H
#define ISC_RESULT_H 1

typedef unsigned int isc_result_t;

#define ISC_R_SUCCESS			0
#define ISC_R_NOMEMORY			1
#define ISC_R_TIMEDOUT			2
#define ISC_R_NOTHREADS			3
#define ISC_R_ADDRNOTAVAIL		4
#define ISC_R_ADDRINUSE			5
#define ISC_R_NOPERM			6
#define ISC_R_NOCONN			7
#define ISC_R_NETUNREACH		8
#define ISC_R_HOSTUNREACH		9
#define ISC_R_NETDOWN			10
#define ISC_R_HOSTDOWN			11
#define ISC_R_CONNREFUSED		12
#define ISC_R_NORESOURCES		13	/* not enough resources */
#define ISC_R_EOF			14	/* end of file */
#define ISC_R_BOUND			15	/* already bound */
#define ISC_R_TASKSHUTDOWN		16	/* task was shut down */
#define ISC_R_LOCKBUSY			17
#define ISC_R_EXISTS			18
#define ISC_R_TOOSMALL			19	/* buffer is too small */
#define ISC_R_CANCELED			20
#define ISC_R_TASKNOSEND		21
#define ISC_R_TASKSHUTTINGDOWN		22	/* task is shutting down */
#define ISC_R_LASTENTRY			22	/* last entry in the list */

#define ISC_R_UNEXPECTED		0xFFFFFFFFL

char *					isc_result_totext(isc_result_t);

#endif /* ISC_RESULT_H */
