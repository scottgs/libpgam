-- ************************************************
-- ************************************************
-- ************************************************
--	Installs the wrapper functions into the DB
--		Some functions are SQL or PL/pgSQL
-- ************************************************
-- ************************************************
-- ************************************************

-- ************************************************
--	Basic Array Math
-- ------------------------------------------------

-- TODO: Move to AnyArray and AnyElement
CREATE OR REPLACE FUNCTION sum_elements(integer[]) 
	RETURNS bigint
     AS '/usr/local/lib/libpgam.so', 'sum_elements'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_add(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayAdd'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_subtract(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arraySubtract'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_multiply_scalar(a anyarray, scalar numeric) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayMultiplyScalar'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_divide_scalar(a anyarray, scalar numeric) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayDivideScalar'
     LANGUAGE C IMMUTABLE STRICT;

-- TODO: Move to C
CREATE OR REPLACE FUNCTION array_product(a anyarray, b anyarray)
RETURNS ANYARRAY
LANGUAGE SQL 
IMMUTABLE STRICT
AS $$
select array_agg(a*b) from (select unnest($1) as a ,unnest($2) as b) as foo
$$;

-- TODO: Move to C
CREATE OR REPLACE FUNCTION array_dot_product(a anyarray, b anyarray)
RETURNS ANYELEMENT
LANGUAGE SQL 
IMMUTABLE STRICT
AS $$
select sum(a*b) from (select unnest($1) as a ,unnest($2) as b) as foo
$$;



CREATE OR REPLACE FUNCTION array_greatest_elements(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayGreatestElements'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_least_elements(a anyarray, b anyarray) 
	RETURNS anyarray
     AS '/usr/local/lib/libpgam.so', 'arrayLeastElements'
     LANGUAGE C IMMUTABLE STRICT;

          
-- ************************************************
--	Other Set-like things
-- ------------------------------------------------
     
CREATE OR REPLACE FUNCTION find_percentile_location(integer[], double precision) 
	RETURNS integer
     AS '/usr/local/lib/libpgam.so', 'find_percentile_location'
     LANGUAGE C IMMUTABLE STRICT;



-- ************************************************
--	Distance Metrics
-- ------------------------------------------------

CREATE OR REPLACE FUNCTION p1_norm_length_normalized(double precision[], double precision[]) 
	RETURNS double precision
     AS '/usr/local/lib/libpgam.so', 'pg_p1_norm_length_normalized'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION p1_norm(double precision[], double precision[]) 
	RETURNS double precision
     AS '/usr/local/lib/libpgam.so', 'pg_p1_norm'
     LANGUAGE C IMMUTABLE STRICT;


-- ************************************************
--	Array as Set 
-- ------------------------------------------------

CREATE OR REPLACE FUNCTION array_argmin(a anyarray) 
	RETURNS int
     AS '/usr/local/lib/libpgam.so', 'argmin'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_argmax(a anyarray) 
	RETURNS int
     AS '/usr/local/lib/libpgam.so', 'argmax'
     LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_max(a anyarray)
RETURNS ANYELEMENT
LANGUAGE SQL 
IMMUTABLE STRICT
AS $$
select max(x) from unnest($1) x
$$;

CREATE OR REPLACE FUNCTION array_min(a anyarray)
RETURNS ANYELEMENT
LANGUAGE SQL 
IMMUTABLE STRICT
AS $$
select min(x) from unnest($1) x
$$;

