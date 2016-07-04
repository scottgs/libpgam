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

#include <stdlib.h>
#include <math.h>

#include "local_pg.h"

/*
   
  sum_elements (array a)
   
  s = SUM(a_i)
*/

PG_FUNCTION_INFO_V1(argmin);

Datum
argmin(PG_FUNCTION_ARGS)
{
	int i=0; // iteration
	int min_i = 0; // marker
	// just use a double
	double s = 0; // marked value

	// Unwrap
	ArrayType *anyArray = PG_GETARG_ARRAYTYPE_P(0);
	Oid oid = ARR_ELEMTYPE(anyArray);

	// there is clearly a better way to do the below 
	// in a general way, but that will have to wait
	// I need should be able to iterate with typelen
	//	int16 typlen;
	//	bool typbyval;
	//	char typalign;
	//    get_typlenbyvalalign(oid,&typlen,&typbyval, &typalign);
	//    Datum el = array_ref(anyArray, ARR_NDIM(anyArray), i, -1, typlen, typbyval, typalign, &isNull );	

	if(oid == INT2OID)
	{
		int16 *a = (int16 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] < s)
			{
				s = a[i];
				min_i = i;
			}
		}
	}
	else if (oid == INT4OID)
	{
		int32 *a = (int32 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] < s)
			{
				s = a[i];
				min_i = i;
			}
		}
	}
	else if (oid == INT8OID)
	{
		int64 *a = (int64 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] < s)
			{
				s = a[i];
				min_i = i;
			}
		}
	}
	else if (oid == FLOAT4OID)
	{
		float4 *a = (float4 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] < s)
			{
				s = a[i];
				min_i = i;
			}
		}
	}
	else if (oid == FLOAT8OID)
	{
		float8 *a = (float8 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] < s)
			{
				s = a[i];
				min_i = i;
			}
		}
	}
	else if(oid == NUMERICOID)
	{
		ereport(INFO, (errmsg_internal("Numeric[] not implemented: %d", oid)));

	}
	else
	{
		ereport(INFO, (errmsg_internal("wrong types: %d", oid)));
	}

		
	PG_RETURN_INT32( min_i + 1 );
}

PG_FUNCTION_INFO_V1(argmax);

Datum
argmax(PG_FUNCTION_ARGS)
{
	int i=0; // iteration
	int max_i = 0; // marker

	// just use a double
	double s = 0; // marked value

	// Unwrap
	ArrayType *anyArray = PG_GETARG_ARRAYTYPE_P(0);
	Oid oid = ARR_ELEMTYPE(anyArray);

	// there is clearly a better way to do the below 
	// in a general way, but that will have to wait
	// I need should be able to iterate with typelen
	//	int16 typlen;
	//	bool typbyval;
	//	char typalign;
	//    get_typlenbyvalalign(oid,&typlen,&typbyval, &typalign);
	//    Datum el = array_ref(anyArray, ARR_NDIM(anyArray), i, -1, typlen, typbyval, typalign, &isNull );	

	if(oid == INT2OID)
	{
		int16 *a = (int16 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] > s)
			{
				s = a[i];
				max_i = i;
			}
		}
	}
	else if (oid == INT4OID)
	{
		int32 *a = (int32 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] > s)
			{
				s = a[i];
				max_i = i;
			}
		}
	}
	else if (oid == INT8OID)
	{
		int64 *a = (int64 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] > s)
			{
				s = a[i];
				max_i = i;
			}
		}
	}
	else if (oid == FLOAT4OID)
	{
		float4 *a = (float4 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] > s)
			{
				s = a[i];
				max_i = i;
			}
		}
	}
	else if (oid == FLOAT8OID)
	{
		float8 *a = (float8 *)  ARR_DATA_PTR(anyArray);
		s = a[i];
		// move through it
		for(++i; i< ARR_DIMS(anyArray)[0]; ++i)
		{
			if (a[i] > s)
			{
				s = a[i];
				max_i = i;
			}
		}
	}
	else if(oid == NUMERICOID)
	{
		ereport(INFO, (errmsg_internal("Numeric[] not implemented: %d", oid)));

	}
	else
	{
		ereport(INFO, (errmsg_internal("wrong types: %d", oid)));
	}

		
	PG_RETURN_INT32( max_i + 1 );
}


