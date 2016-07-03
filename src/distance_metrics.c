#include "local_pg.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



/*
 * Various distance metrics that may be used for terrain stuff
 */
// ++++++++++++++++++++++++++++++++++++++++++++++++++
// Declarations:
// ++++++++++++++++++++++++++++++++++++++++++++++++++
double p1_norm_length_normalized(double * array1, double * array2, int size);
double p1_norm(double * array1, double * array2, int size);

// ++++++++++++++++++++++++++++++++++++++++++++++++++
// BEGIN: PostgreSQL Bridge Implementations
// ++++++++++++++++++++++++++++++++++++++++++++++++++
PG_FUNCTION_INFO_V1(pg_p1_norm_length_normalized);

Datum
pg_p1_norm_length_normalized(PG_FUNCTION_ARGS)
{
	// Copy both the expected args (signature 1, signature 2)
	//	from the db function arg list
	// Note: The postgres internal wrapper should be defined as STRICT
	ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
	ArrayType * array2 = GET_ARRAY_PTR_COPY(1);

	// Check for non-null and non-empty, else return null
	RETURN_NON_NULL_ARG(array1, array2);
	RETURN_NON_EMPTY_ARRAY_ARG(array1, array2);

	// Size checks
	const int size = ARR_DIMS(array1)[0];
	if (size != ARR_DIMS(array2)[0])
		PG_RETURN_NULL();

	// Get regular double[] references
	float8 * signature_1 = (float8 *) ARR_DATA_PTR(array1);
	float8 * signature_2 = (float8 *) ARR_DATA_PTR(array2);

	PG_RETURN_FLOAT8( p1_norm_length_normalized(signature_1, signature_2, size) );
}

PG_FUNCTION_INFO_V1(pg_p1_norm);

Datum
pg_p1_norm(PG_FUNCTION_ARGS)
{
	// Copy both the expected args (signature 1, signature 2)
	//	from the db function arg list
	// Note: The postgres internal wrapper should be defined as STRICT
	ArrayType * array1 = GET_ARRAY_PTR_COPY(0);
	ArrayType * array2 = GET_ARRAY_PTR_COPY(1);

	// Check for non-null and non-empty, else return null
	RETURN_NON_NULL_ARG(array1, array2);
	RETURN_NON_EMPTY_ARRAY_ARG(array1, array2);

	// Size checks
	const int size = ARR_DIMS(array1)[0];
	if (size != ARR_DIMS(array2)[0])
		PG_RETURN_NULL();

	// Get regular double[] references
	float8 * signature_1 = (float8 *) ARR_DATA_PTR(array1);
	float8 * signature_2 = (float8 *) ARR_DATA_PTR(array2);

	PG_RETURN_FLOAT8( p1_norm(signature_1, signature_2, size) );
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++
// END: PostgreSQL Bridge Implementations
// ++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++
// BEGIN: C Implementations
// NOTE: Size and not NULL should have been verified
// ++++++++++++++++++++++++++++++++++++++++++++++++++
double p1_norm_length_normalized(double * array1, double * array2, int size)
{
	const double un_norm = p1_norm(array1,array2,size);
	return (un_norm/size);
}

double p1_norm(double * array1, double * array2, int size)
{
	double result = 0.0;
	int i = 0;
	for ( ; i < size; ++i)
		result += fabs(array1[i] - array2[i]);

	return result;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++
// END: C Implementations
// ++++++++++++++++++++++++++++++++++++++++++++++++++

