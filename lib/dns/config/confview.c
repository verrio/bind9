/*
 * Copyright (C) 1999  Internet Software Consortium.
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

#include <config.h>

#include <sys/types.h>

#include <isc/assertions.h>
#include <isc/net.h>

#include <dns/confzone.h>
#include <dns/confcommon.h>
#include <dns/confview.h>
#include <dns/log.h>

#include "confpvt.h"


isc_result_t
dns_c_viewtable_new(isc_log_t *lctx,
		    isc_mem_t *mem, dns_c_viewtable_t **viewtable)
{
	dns_c_viewtable_t *table;
	
	REQUIRE(mem != NULL);
	REQUIRE(viewtable != NULL);

	table = isc_mem_get(mem, sizeof *table);
	if (table == NULL) {
		isc_log_write(lctx, DNS_LOGCATEGORY_CONFIG,
			      DNS_LOGMODULE_CONFIG, ISC_LOG_CRITICAL,
			      "Out of memory");
		return (ISC_R_NOMEMORY);
	}

	table->mem = mem;

	ISC_LIST_INIT(table->views);

	*viewtable = table;

	return (ISC_R_SUCCESS);
}


isc_result_t
dns_c_viewtable_delete(isc_log_t *lctx,
		       dns_c_viewtable_t **viewtable)
{
	dns_c_viewtable_t *table;
	
	REQUIRE(viewtable != NULL);

	table = *viewtable;
	if (table == NULL) {
		return (ISC_R_SUCCESS);
	}
	*viewtable = NULL;
	
	dns_c_viewtable_clear(lctx, table);

	isc_mem_put(table->mem, table, sizeof *table);

	return (ISC_R_SUCCESS);
}


void
dns_c_viewtable_addview(isc_log_t *lctx,
			dns_c_viewtable_t *viewtable, dns_c_view_t *view)
{
	(void) lctx;			/* lint */
	
	REQUIRE(viewtable != NULL);
	REQUIRE(view != NULL);
	
	ISC_LIST_APPEND(viewtable->views, view, next);
}



void
dns_c_viewtable_rmview(isc_log_t *lctx,
		       dns_c_viewtable_t *viewtable, dns_c_view_t *view)
{
	(void) lctx;			/* lint */
	
	REQUIRE(viewtable != NULL);
	REQUIRE(view != NULL);
	
	ISC_LIST_UNLINK(viewtable->views, view, next);
}



isc_result_t
dns_c_viewtable_clear(isc_log_t *lctx,
		      dns_c_viewtable_t *table)
{
	dns_c_view_t *elem;
	dns_c_view_t *tmpelem;
	isc_result_t r;
	
	REQUIRE(table != NULL);
	
	elem = ISC_LIST_HEAD(table->views);
	while (elem != NULL) {
		tmpelem = ISC_LIST_NEXT(elem, next);
		ISC_LIST_UNLINK(table->views, elem, next);
		
		r = dns_c_view_delete(lctx, &elem);
		if (r != ISC_R_SUCCESS) {
			isc_log_write(lctx, DNS_LOGCATEGORY_CONFIG,
				      DNS_LOGMODULE_CONFIG,
				      ISC_LOG_CRITICAL,
				      "Failed to delete view.\n");
			return (r);
		}

		elem = tmpelem;
	}

	return (ISC_R_SUCCESS);
}



isc_result_t
dns_c_viewtable_viewbyname(isc_log_t *lctx,
			   dns_c_viewtable_t *viewtable,
			   const char *viewname,
			   dns_c_view_t **retval)
{
	dns_c_view_t *elem;

	(void) lctx;			/* lint */
	
	REQUIRE(viewtable != NULL);
	REQUIRE(retval != NULL);
	REQUIRE(viewname != NULL);
	REQUIRE(strlen(viewname) > 0);

	elem = ISC_LIST_HEAD(viewtable->views);
	while (elem != NULL) {
		if (strcmp(viewname, elem->name) == 0) {
			break;
		}

		elem = ISC_LIST_NEXT(elem, next);
	}
	
	if (elem != NULL) {
		*retval = elem;
	}

	return (elem == NULL ? ISC_R_NOTFOUND : ISC_R_SUCCESS);
}



isc_result_t
dns_c_viewtable_rmviewbyname(isc_log_t *lctx,
			     dns_c_viewtable_t *viewtable,
					  const char *name)
{
	dns_c_view_t *view;
	isc_result_t res;

	res = dns_c_viewtable_viewbyname(lctx, viewtable, name, &view);
	if (res == ISC_R_SUCCESS) {
		ISC_LIST_UNLINK(viewtable->views, view, next);
		dns_c_view_delete(lctx, &view);
	}

	return (res);
}

	

isc_result_t
dns_c_view_new(isc_log_t *lctx,
	       isc_mem_t *mem, const char *name, dns_c_view_t **newview)
{
	dns_c_view_t *view;

	REQUIRE(name != NULL);
	REQUIRE(strlen(name) > 0);
	REQUIRE(newview != NULL);

	view = isc_mem_get(mem, sizeof *view);
	if (view == NULL) {
		return (ISC_R_NOMEMORY);
	}

	/* XXXJAB not portable -- should set each field */
	memset(view, 0x0, sizeof *view); 

	view->mem = mem;
	view->name = isc_mem_strdup(mem, name);
	if (view->name == NULL) {
		isc_mem_put(mem, view, sizeof *view);
		isc_log_write(lctx, DNS_LOGCATEGORY_CONFIG,
			      DNS_LOGMODULE_CONFIG, ISC_LOG_CRITICAL,
			      "Insufficient memory");
	}

	*newview = view;

	return (ISC_R_SUCCESS);
}


void
dns_c_viewtable_print(isc_log_t *lctx,
		      FILE *fp, int indent,
		      dns_c_viewtable_t *table)
{
	dns_c_view_t *view;

	REQUIRE(fp != NULL);
	REQUIRE(indent >= 0);

	if (table == NULL) {
		return;
	}
		
	view = ISC_LIST_HEAD(table->views);
	while (view != NULL) {
		dns_c_view_print(lctx, fp, indent, view);
		fprintf(fp, "\n");

		view  = ISC_LIST_NEXT(view, next);
	}
}

void
dns_c_view_print(isc_log_t *lctx,
		 FILE *fp, int indent, dns_c_view_t *view)
{
	dns_c_printtabs(lctx, fp, indent);
	fprintf(fp, "view \"%s\" {\n", view->name);

	if (view->allowquery != NULL) {
		dns_c_printtabs(lctx, fp, indent + 1);
		fprintf(fp, "allow-query ");
		dns_c_ipmatchlist_print(lctx, fp, indent + 2,
					view->allowquery);
	}

	/* XXXJAB rest of view fields */

	dns_c_printtabs(lctx, fp, indent);
	fprintf(fp, "};\n");
}


isc_result_t
dns_c_view_setallowquery(isc_log_t *lctx,
			 dns_c_view_t *view,
			 dns_c_ipmatchlist_t *ipml,
			 isc_boolean_t deepcopy)
{
	isc_result_t res;
	
	REQUIRE(view != NULL);
	REQUIRE(ipml != NULL);

	if (view->allowquery != NULL) {
		dns_c_ipmatchlist_delete(lctx, &view->allowquery);
	}

	if (deepcopy) {
		res = dns_c_ipmatchlist_copy(lctx, view->mem,
					     &view->allowquery, ipml);
	} else {
		view->allowquery = ipml;
		res = ISC_R_SUCCESS;
	}

	return (res);
}
	

isc_result_t
dns_c_view_getallowqueryexpanded(isc_log_t *lctx,
				 isc_mem_t *mem,
				 dns_c_view_t *view,
				 dns_c_acltable_t *acltable,
				 dns_c_ipmatchlist_t **retval)
{
	dns_c_ipmatchlist_t *newlist;
	isc_result_t r;

	if (view->allowquery == NULL) {
		newlist = NULL;
		r = ISC_R_SUCCESS;
	} else {
		r = dns_c_ipmatchlist_copy(lctx, mem, &newlist, view->allowquery);
		if (r != ISC_R_SUCCESS) {
			return (r);
		}

		r = dns_c_acl_expandacls(lctx, acltable, newlist);
	}

	*retval = newlist;
	
	return (r);
}



isc_result_t
dns_c_view_delete(isc_log_t *lctx,
		  dns_c_view_t **viewptr)
{
	dns_c_view_t *view;
	
	REQUIRE(viewptr != NULL);

	if (*viewptr == NULL) {
		return (ISC_R_SUCCESS);
	}

	view = *viewptr;

	isc_mem_free(view->mem, view->name);
	
	dns_c_ipmatchlist_delete(lctx, &view->allowquery);

	isc_mem_put(view->mem, view, sizeof *view);
	
	return (ISC_R_SUCCESS);
}

	
isc_result_t
dns_c_view_getname(isc_log_t *lctx, dns_c_view_t *view, const char **retval)
{
	(void) lctx;
	
	REQUIRE(view != NULL);
	REQUIRE(retval != NULL);

	*retval = view->name;

	return (ISC_R_SUCCESS);
}

