-- ************************************************
--	Installs the wrapper functions into the DB
-- ************************************************

CREATE OR REPLACE FUNCTION sum_elements(integer[]) 
	RETURNS bigint
     AS '/usr/local/lib/libpgam.so', 'sum_elements'
     LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION array_add(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayAdd'
     LANGUAGE C;

CREATE OR REPLACE FUNCTION array_subtract(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arraySubtract'
     LANGUAGE C;

CREATE OR REPLACE FUNCTION array_multiply_scalar(a anyarray, scalar numeric) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayMultiplyScalar'
     LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION array_divide_scalar(a anyarray, scalar numeric) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayDivideScalar'
     LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION array_greatest_elements(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayGreatestElements'
     LANGUAGE C;

CREATE OR REPLACE FUNCTION array_least_elements(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayLeastElements'
     LANGUAGE C;
          
     
CREATE OR REPLACE FUNCTION find_percentile_location(integer[], double precision) 
	RETURNS integer
     AS '/usr/local/lib/libpgam.so', 'find_percentile_location'
     LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION p1_norm_length_normalized(double precision[], double precision[]) 
	RETURNS double precision
     AS '/usr/local/lib/libpgam.so', 'pg_p1_norm_length_normalized'
     LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION p1_norm(double precision[], double precision[]) 
	RETURNS double precision
     AS '/usr/local/lib/libpgam.so', 'pg_p1_norm'
     LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION array_argmin(a anyarray) 
	RETURNS int
     AS '/usr/local/lib/libpgam.so', 'argmin'
     LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION array_argmax(a anyarray) 
	RETURNS int
     AS '/usr/local/lib/libpgam.so', 'argmax'
     LANGUAGE C STRICT;

