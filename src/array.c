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

PG_FUNCTION_INFO_V1(sum_elements);

Datum
sum_elements(PG_FUNCTION_ARGS)
{
     int i=0;
     ArrayType *anyArray = PG_GETARG_ARRAYTYPE_P(0);
     int32 *a = (int32 *)  ARR_DATA_PTR(anyArray);
     int64 s = 0;

     for(; i< ARR_DIMS(anyArray)[0]; ++i)
	  s = s + (int64) a[i];

     PG_RETURN_INT64( s );
}

/*
       
  lower_cut (array a, double precision pct)
       
*/

PG_FUNCTION_INFO_V1(find_percentile_location);

Datum
find_percentile_location(PG_FUNCTION_ARGS)
{
     int32 idx = 0;
     ArrayType *anyArray = PG_GETARG_ARRAYTYPE_P(0);
     int32 *a = (int32 *)  ARR_DATA_PTR(anyArray);
     int32 size = ARR_DIMS(anyArray)[0];
     float8 percentile = PG_GETARG_FLOAT8(1);

     /* get the sum of the elements */
     int64 sum = 0;
     for(; idx< size; ++idx)
	  sum = sum + (int64) a[idx];

     /* find our target value in the CDF */
     float8 target = percentile * sum;

     /* remember the last bin with a non-zero value. this
	this fixes a glitch that causes the 100% location
	to always be at 65535 */
     int32 last_nz_idx = 0;
	
     /* count until we get to the target, then return */
     int64 cnt = 0;
     for(idx=0; idx< size; ++idx)
     {
	  cnt += a[idx];
	  if(cnt > target)
	       PG_RETURN_INT32( idx );

	  if(a[idx] > 0)
	       last_nz_idx = idx;
     }

     /* the default case */
     PG_RETURN_INT32( last_nz_idx );
}

typedef Datum (*arrayCallback)(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data);
typedef bool (*arrayTest)(const ArrayType * array1, const ArrayType * array2, void * data);

static void incrementCoord(int * idx, int * dimSize, int ndim, int coord)
{
     if(idx == NULL || dimSize == NULL || ndim == 0 || coord >= ndim)
     {
	  return;
     }
     
     ++idx[coord];

     if(idx[coord] == dimSize[coord])
     {
	  idx[coord] = 0;
	  incrementCoord(idx, dimSize, ndim, coord+1);
     }
}

/*
 * The checks made by arrayIterator are the following. If they fail, NULL is returned:
 * 
 * 1. Both arrays are not NULL
 * 
 * 2. If both arrays are not null, their dimensions and sizes match
 *
 * 3. The types of the elements of the arrays are the same. This
 * should be checked by postgres anyway, so I won't explitly check it.
 *
 * 4. The elements in the array are numeric or ints
 * 
 */
ArrayType * arrayIterator(ArrayType * array1, ArrayType *array2,
			  arrayTest test, arrayCallback cb, void * data)
{
     ArrayType * arrays[2];
     ArrayType * result = NULL;
     
     /* This places the non-null array at the top. If they are both
      * non-null, order is preserved. */
     if(array1 == NULL)
     {
	  arrays[0] = array2;
	  arrays[1] = array1;
     }
     else
     {
	  arrays[0] = array1;
	  arrays[1] = array2;
     }

     /* both are null*/
     if(arrays[0] == NULL)
     {
	  ereport(INFO, (errmsg_internal("both are NULL")));
	  IT_RETURN_NULL();
     }

     /* the test failed */
     if(!test(arrays[0], arrays[1], data))
     {
	  return arrays[0];
     }

     /* Begin.
      *
      * array[0] is not NULL
      * array[1] may be NULL
      * 
      * */
     {
	  int volume = 1;
	  Datum element[2] = {(Datum)NULL};
	  Oid arrayType[2] = {ARR_ELEMTYPE(arrays[0]), 0};
	  int16 typlen[3];
	  bool typbyval[3];
	  char typalign[3];
	  int idx[ARR_NDIM(arrays[0])];
	  int idxOne[ARR_NDIM(arrays[0])];
	  int idxTwo[ARR_NDIM(arrays[0])];
	  bool isNull[2] = {false};
	  int j = 0;
	  int i = 0;
	  bool elemsAreInts = false;
	  Datum * blankElems = NULL;
	  bool * blankElemsIsNull = NULL;

	  if(arrays[1] == NULL)
	  {
	       arrayType[1] = arrayType[0];
	  }
	  else
	  {
	       arrayType[1] = ARR_ELEMTYPE(arrays[1]);
	  }

	  if(arrayType[0] == INT2OID ||
	     arrayType[0] == INT4OID ||
	     arrayType[0] == INT8OID)
	  {
	       elemsAreInts = true;
	  }
	  else if(arrayType[0] == NUMERICOID ||
		  arrayType[0] == FLOAT4OID ||
		  arrayType[0] == FLOAT8OID)
	  {
	       elemsAreInts = false;
	  }
	  else
	  {
	       ereport(INFO, (errmsg_internal("wrong types: %d", arrayType[0])));
	       IT_RETURN_NULL();
	  }

	  if(arrays[1] != NULL)
	  {
	       if(ARR_NDIM(arrays[0]) != ARR_NDIM(arrays[1]))
	       {
		    ereport(INFO, (errmsg_internal("sizes do not match, ndim1: %d and ndim2: %d",
						    ARR_NDIM(arrays[0]), ARR_NDIM(arrays[1]))));
		    IT_RETURN_NULL();
	       }

	       /* Check if the arrays match in every dimension*/
	       for(i = 0;
		   i < ARR_NDIM(arrays[0]);
		   ++i)
	       {
		    if(ARR_DIMS(arrays[0])[i] != ARR_DIMS(arrays[1])[i])
		    {
			 ereport(INFO, (errmsg_internal("dimensions do not match")));
			 IT_RETURN_NULL();
		    }
	       }
	  }
	  
	  /* calculate volume */
	  for(i = 0;
	      i < ARR_NDIM(arrays[0]);
	      ++i)
	  {
	       volume *= ARR_DIMS(arrays[0])[i];
	       idx[i] = 0;
	  }

	  /* get all of the type stuff */
	  get_typlenbyvalalign(arrayType[0], &typlen[0], &typbyval[0], &typalign[0]);
	  get_typlenbyvalalign(arrayType[1], &typlen[1], &typbyval[1], &typalign[1]);

	  blankElems = palloc(sizeof(Datum) * volume);
	  blankElemsIsNull = palloc(sizeof(bool) * volume);
	  for(i = 0;
	      i < volume;
	      ++i)
	  {
	       blankElems[i] = (Datum)NULL;
	       blankElemsIsNull[i] = true;
	  }

	  result = construct_md_array(blankElems, blankElemsIsNull,
				      ARR_NDIM(arrays[0]), ARR_DIMS(arrays[0]), ARR_LBOUND(arrays[0]),
				      arrayType[0], typlen[0], typbyval[0], typalign[0]);

	  pfree(blankElems);
	  pfree(blankElemsIsNull);

	  for(i = 0;
	      i < volume;
	      ++i)
	  {
	       ArrayType * newResult;
	       Datum cbDatum;

	       for(j = 0;
		   j < ARR_NDIM(arrays[0]);
		   ++j)
	       {
		    idxOne[j] = idx[j] + ARR_LBOUND(arrays[0])[j];
		    
		    if(arrays[1] != NULL)
		    {
			 idxTwo[j] = idx[j] + ARR_LBOUND(arrays[1])[j];
		    }
	       }
	       element[0] = (Datum)NULL;
	       element[1] = (Datum)NULL;

	       /* The -1 is passed where the array length goes. If we pass
		* something positive, it assumes the array is one
		* dimensional . . . */
	       element[0] = array_ref(arrays[0], ARR_NDIM(arrays[0]), idxOne, -1, typlen[0], typbyval[0], typalign[0], &isNull[0]);

	       if(arrays[1] != NULL)
	       {
		    element[1] = array_ref(arrays[1], ARR_NDIM(arrays[1]), idxTwo, -1, typlen[1], typbyval[1], typalign[1], &isNull[1]);		   
	       }
	       
	       cbDatum = cb(element[0], element[1], arrayType[0], idxOne, idxTwo, data);
	       
	       newResult = array_set(result, ARR_NDIM(result), idxOne,
				     cbDatum,
				     false, -1, typlen[0], typbyval[0], typalign[0]);
	       pfree(result);
	       result = newResult;
		    
	       incrementCoord(idx, ARR_DIMS(arrays[0]), ARR_NDIM(arrays[0]), 0);
	  }
     }

     return result;
}

/* test that both arrays are non-null */
bool binaryTest(const ArrayType * array1, const ArrayType * array2, void * data)
{
     if(array1 != NULL && array2 != NULL)
     {
	  return true;
     }
     return false;
}

bool unaryTest(const ArrayType * array1, const ArrayType * array2, void * data)
{
     if(array1 != NULL || array2 != NULL)
     {
	  return true;
     }
     return false;
}



/* ADDITION */
Datum addCb(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data)
{
     Datum ret = (Datum)NULL;

     if(oid == INT2OID)
     {
	  ret = DirectFunctionCall2(int2pl, num1, num2);
     }
     else if(oid == INT4OID ||
	     oid == INT8OID)
     {
	  ret = DirectFunctionCall2(int4pl, num1, num2);
     }
     else if(oid == FLOAT4OID)
     {
	  ret = DirectFunctionCall2(float4pl, num1, num2);
     }
     else if(oid == FLOAT8OID)
     {
	  ret = DirectFunctionCall2(float8pl, num1, num2);
     }
     else if(oid == NUMERICOID)
     {
     	  ret = DirectFunctionCall2(numeric_add, num1, num2);
     }
     else
     {
	ereport(INFO, (errmsg_internal("ELSE ?")));
     }

     return ret;
}

PG_FUNCTION_INFO_V1(arrayAdd);

Datum
arrayAdd(PG_FUNCTION_ARGS)
{
     ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
     ArrayType * array2 = GET_ARRAY_PTR_COPY(1);
     ArrayType * result = NULL;

     RETURN_NON_NULL_ARG(array1, array2);
     RETURN_NON_EMPTY_ARRAY_ARG(array1, array2);

     result = arrayIterator(array1, array2, binaryTest, addCb, NULL);
     if(result != NULL)
     {
	  PG_RETURN_ARRAYTYPE_P(result);
     }
     PG_RETURN_NULL();
}

/* SUBTRACTION */
Datum subCb(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data)
{
     Datum ret = (Datum)NULL;

     if(oid == INT2OID)
     {
	  ret = DirectFunctionCall2(int2mi, num1, num2);
     }
     else if(oid == INT4OID ||
	     oid == INT8OID)
     {
	  ret = DirectFunctionCall2(int4mi, num1, num2);
     }
     else if(oid == FLOAT4OID)
     {
	ereport(INFO, (errmsg_internal("FLOAT4OID")));
	 ret = DirectFunctionCall2(float4mi, num1, num2);
     }
     else if(oid == FLOAT8OID)
     {
	  ret = DirectFunctionCall2(float8mi, num1, num2);
     }
     else if(oid == NUMERICOID)
     {
     	  ret = DirectFunctionCall2(numeric_sub, num1, num2);
     }
     else
     {
     }

     return ret;
}

PG_FUNCTION_INFO_V1(arraySubtract);

Datum
arraySubtract(PG_FUNCTION_ARGS)
{
     ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
     ArrayType * array2 = GET_ARRAY_PTR_COPY(1);
     ArrayType * result = NULL;

     RETURN_NON_NULL_ARG(array1, array2);
     RETURN_NON_EMPTY_ARRAY_ARG(array1, array2);
     result = arrayIterator(array1, array2, binaryTest, subCb, NULL);
     if(result != NULL)
     {
	  PG_RETURN_ARRAYTYPE_P(result);
     }
     PG_RETURN_NULL();
}

/* SCALAR MULTIPLICATION */
struct Scalar
{
     Datum scalar;
};

Datum scalarMulCb(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data)
{
     struct Scalar * scalar = (struct Scalar *)data;
     if(scalar == NULL || scalar->scalar == NULL)
     {
	  return NULL;
     }

     Datum ret = (Datum)NULL;
     Datum mul = (Datum)NULL;
     if(oid == INT2OID)
     {
	  mul = DirectFunctionCall1(numeric_int2, scalar->scalar);
	  ret = DirectFunctionCall2(int2mul, num1, mul);
     }
     else if(oid == INT4OID ||
	     oid == INT8OID)
     {
	  mul = DirectFunctionCall1(numeric_int4, scalar->scalar);
	  ret = DirectFunctionCall2(int4mul, num1, mul);
     }
     else if(oid == FLOAT4OID)
     {
	  mul = DirectFunctionCall1(numeric_float4, scalar->scalar);
	  ret = DirectFunctionCall2(float4mul, num1, mul);
     }
     else if(oid == FLOAT8OID)
     {
	  mul = DirectFunctionCall1(numeric_float8, scalar->scalar);
	  ret = DirectFunctionCall2(float8mul, num1, mul);
     }
     else if(oid == NUMERICOID)
     {
     	  ret = DirectFunctionCall2(numeric_mul, num1, scalar->scalar);
     }
     else
     {
     }

     return ret;
}

PG_FUNCTION_INFO_V1(arrayMultiplyScalar);

Datum
arrayMultiplyScalar(PG_FUNCTION_ARGS)
{
     ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
     Numeric * numScalar = PG_GETARG_NUMERIC(1);
     ArrayType * result = NULL;
     struct Scalar * scalar = palloc(sizeof(struct Scalar));
     scalar->scalar = NumericGetDatum(numScalar);
     
     result = arrayIterator(array1, NULL, unaryTest, scalarMulCb, (void *)scalar);
     
     pfree(scalar);

     if(result != NULL)
     {
	  PG_RETURN_ARRAYTYPE_P(result);
     }
     PG_RETURN_NULL();
}

/* SCALAR DIVISION */
Datum scalarDivCb(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data)
{
     if(num1 == (Datum)NULL)
     {
	  return (Datum)NULL;
     }

     struct Scalar * scalar = (struct Scalar *)data;
     if(scalar == NULL || scalar->scalar == NULL)
     {
	  return NULL;
     }

     Datum ret = (Datum)NULL;
     Datum div = (Datum)NULL;
     if(oid == INT2OID)
     {
	  div = DirectFunctionCall1(numeric_int2, scalar->scalar);
	  ret = DirectFunctionCall2(int2div, num1, div);
     }
     else if(oid == INT4OID ||
	     oid == INT8OID)
     {
	  div = DirectFunctionCall1(numeric_int4, scalar->scalar);
	  ret = DirectFunctionCall2(int4div, num1, div);
     }
     else if(oid == FLOAT4OID)
     {
	  div = DirectFunctionCall1(numeric_float4, scalar->scalar);
	  ret = DirectFunctionCall2(float4div, num1, div);
     }
     else if(oid == FLOAT8OID)
     {
	  div = DirectFunctionCall1(numeric_float8, scalar->scalar);
	  ret = DirectFunctionCall2(float8div, num1, div);
     }
     else if(oid == NUMERICOID)
     {
     	  ret = DirectFunctionCall2(numeric_div, num1, scalar->scalar);
     }
     else
     {
     }

     return ret;
}

PG_FUNCTION_INFO_V1(arrayDivideScalar);

Datum
arrayDivideScalar(PG_FUNCTION_ARGS)
{
     ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
     Numeric numScalar = PG_GETARG_NUMERIC(1);
     ArrayType * result = NULL;
     struct Scalar * scalar = palloc(sizeof(struct Scalar));
     scalar->scalar = NumericGetDatum(numScalar);     
     
     result = arrayIterator(array1, NULL, unaryTest, scalarDivCb, (void *)scalar);
     
     pfree(scalar);

     if(result != NULL)
     {
	  PG_RETURN_ARRAYTYPE_P(result);
     }
     PG_RETURN_NULL();
}

/* GREATEST ELEMENTS */
Datum maxCb(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data)
{     
     Datum ret = (Datum)NULL;

     if(oid == INT2OID)
     {
	  ret = DirectFunctionCall2(int2larger, num1, num2);
     }
     else if(oid == INT4OID ||
	     oid == INT8OID)
     {
	  ret = DirectFunctionCall2(int4larger, num1, num2);
     }
     else if(oid == FLOAT4OID)
     {
	  ret = DirectFunctionCall2(float4larger, num1, num2);
     }
     else if(oid == FLOAT8OID)
     {
	  ret = DirectFunctionCall2(float8larger, num1, num2);
     }
     else if(oid == NUMERICOID)
     {
     	  ret = DirectFunctionCall2(numeric_larger, num1, num2);
     }
     else
     {
     }

     return ret;
}

PG_FUNCTION_INFO_V1(arrayGreatestElements);

Datum
arrayGreatestElements(PG_FUNCTION_ARGS)
{
     ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
     ArrayType * array2 = GET_ARRAY_PTR_COPY(1);
     ArrayType * result = NULL;

     RETURN_NON_NULL_ARG(array1, array2);
     RETURN_NON_EMPTY_ARRAY_ARG(array1, array2);
	  
     result = arrayIterator(array1, array2, binaryTest, maxCb, NULL);
     if(result != NULL)
     {
	  PG_RETURN_ARRAYTYPE_P(result);
     }
     PG_RETURN_NULL();
}

/* LEAST ELEMENTS */
Datum minCb(const Datum num1, const Datum num2, Oid oid, const int * idxOne, const int * idxTwo, void * data)
{
     if(num1 == (Datum)NULL || num2 == (Datum)NULL)
     {
	  return (Datum)NULL;
     }
     
     Datum ret = (Datum)NULL;
     
     if(oid == INT2OID)
     {
	  ret = DirectFunctionCall2(int2smaller, num1, num2);
     }
     else if(oid == INT4OID ||
	     oid == INT8OID)
     {
	  ret = DirectFunctionCall2(int4smaller, num1, num2);
     }
     else if(oid == FLOAT4OID)
     {
	  ret = DirectFunctionCall2(float4smaller, num1, num2);
     }
     else if(oid == FLOAT8OID)
     {
	  ret = DirectFunctionCall2(float8smaller, num1, num2);
     }
     else if(oid == NUMERICOID)
     {
     	  ret = DirectFunctionCall2(numeric_smaller, num1, num2);
     }
     else
     {
     }

     return ret;
}

PG_FUNCTION_INFO_V1(arrayLeastElements);

Datum
arrayLeastElements(PG_FUNCTION_ARGS)
{
     ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
     ArrayType * array2 = GET_ARRAY_PTR_COPY(1);
     ArrayType * result = NULL;
     
     RETURN_NON_NULL_ARG(array1, array2);
     RETURN_NON_EMPTY_ARRAY_ARG(array1, array2);
     result = arrayIterator(array1, array2, binaryTest, minCb, NULL);
     if(result != NULL)
     {
	  PG_RETURN_ARRAYTYPE_P(result);
     }
     PG_RETURN_NULL();
}

