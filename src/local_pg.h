#ifndef _CGI_LOCAL_PG_H
#define _CGI_LOCAL_PG_H

// Postgres Libraries
#include <postgres.h>
#include <fmgr.h>
#include <utils/array.h>
#include <utils/lsyscache.h>
#include <utils/numeric.h>
#include <utils/builtins.h>
#include <utils/int8.h>
#include <miscadmin.h>
#include <catalog/pg_type.h>
#include <access/hash.h>

/******************************************************************************
MACROS for happiness
******************************************************************************/
/*
  Free an iterator and return null
*/
#define IT_RETURN_NULL() 				\
	{ 						\
		if(result != NULL){ 			\
			pfree(result); 			\
			result = NULL; 			\
		} 					\
		return NULL;				\
	}						\


/*
  An implementation of COALESCE(a,b)
*/
#define RETURN_NON_NULL_ARG(a, b)			\
     if(a == NULL || b == NULL)				\
     {							\
	  if(a)						\
	  {						\
	       PG_RETURN_ARRAYTYPE_P(a);		\
	  }						\
	  else if(b)					\
	  {						\
	       PG_RETURN_ARRAYTYPE_P(b);		\
	  }						\
	  else						\
	  {						\
	       PG_RETURN_NULL();			\
	  }						\
     }							\


/*
  Attempt to check for non-empty
*/
#define RETURN_NON_EMPTY_ARRAY_ARG(a, b)		\
     if(ARR_NDIM(a) == 0)				\
     {							\
	  PG_RETURN_ARRAYTYPE_P(b);			\
     }							\
     else if(ARR_NDIM(b) == 0)				\
     {							\
	  PG_RETURN_ARRAYTYPE_P(a);			\
     }							\


/*
  Get a copy of an array pointer if not null
*/
#define GET_ARRAY_PTR_COPY(n) 				\
	PG_ARGISNULL(n) ? 				\
		NULL 					\
		: 					\
		PG_GETARG_ARRAYTYPE_P_COPY(n);		\


#endif /* _CGI_LOCAL_PG_H */

