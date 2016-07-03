-- array_add test
CREATE OR REPLACE FUNCTION array_add_test()
RETURNS void as $$
DECLARE
BEGIN
	IF (array_add(ARRAY[8, 4], ARRAY[4, 5]) <> ARRAY[12, 9]) THEN
	   RAISE EXCEPTION 'array_add failed';
	   return;
	END IF;
	IF (array_add(ARRAY[8.4, 4.56], ARRAY[4.2, 5.6]) <> ARRAY[12.6, 10.16]) THEN
	   RAISE EXCEPTION 'array_add failed';
	   return;
	END IF;
	IF (array_add(ARRAY[[2, 7], [1, 8], [2, 8]], array[[3, 1], [4, 1], [5, 9]]) <> ARRAY[[5, 8], [5, 9], [7, 17]]) THEN
	   RAISE EXCEPTION 'array_add failed';
	   return;
        END IF;
END;
$$ LANGUAGE plpgsql;

-- array_subtract test
CREATE OR REPLACE FUNCTION array_subtract_test()
RETURNS void as $$
DECLARE
BEGIN
	IF (array_subtract(ARRAY[8, 4], ARRAY[4, 5]) <> ARRAY[4, -1]) THEN
	   RAISE EXCEPTION 'array_subtract failed';
	   return;
	END IF;
	IF (array_subtract(ARRAY[8.4, 4.56], ARRAY[4.2, 5.6]) <> ARRAY[4.2, -1.04]) THEN
	   RAISE EXCEPTION 'array_subtract failed';
	   return;
	END IF;
	IF (array_subtract(ARRAY[[2, 7], [1, 8], [2, 8]], array[[3, 1], [4, 1], [5, 9]]) <> ARRAY[[-1, 6], [-3, 7], [-3, -1]]) THEN
	   RAISE EXCEPTION 'array_subtract failed';
	   return;
        END IF;
END;
$$ LANGUAGE plpgsql;

-- array_multiply_scalar test
CREATE OR REPLACE FUNCTION array_multiply_scalar_test()
RETURNS void as $$
DECLARE
BEGIN
	IF (array_multiply_scalar(ARRAY[8, 4], 2) <> ARRAY[16, 8]) THEN
	   RAISE EXCEPTION 'array_multiply_scalar failed';
	   return;
	END IF;
	IF (array_multiply_scalar(ARRAY[8.4, 4.56], 2.71828) <> ARRAY[22.833552, 12.3953568]) THEN
	   RAISE EXCEPTION 'array_multiply_scalar failed';
	   return;
	END IF;
	IF (array_multiply_scalar(ARRAY[[2, 7], [1, 8], [2, 8]], 3) <> ARRAY[[6, 21], [3, 24], [6, 24]]) THEN
	   RAISE EXCEPTION 'array_multiply_scalar failed';
	   return;
        END IF;
END;
$$ LANGUAGE plpgsql;

-- array_divide_scalar test
CREATE OR REPLACE FUNCTION array_divide_scalar_test()
RETURNS void as $$
DECLARE
BEGIN
	IF (array_divide_scalar(ARRAY[8, 4], 2) <> ARRAY[4, 2]) THEN
	   RAISE EXCEPTION 'array_divide_scalar failed';
	   return;
	END IF;
	IF (array_divide_scalar(ARRAY[8.4, 4.56], 2.71828) <> ARRAY[3.0901893844637050, 1.6775313801374399]) THEN
	   RAISE EXCEPTION 'array_divide_scalar failed';
	   return;
	END IF;
	IF (array_divide_scalar(ARRAY[[2, 7], [1, 8], [2, 8]], 3) <> ARRAY[[0, 2], [0, 2], [0, 2]]) THEN
	   RAISE EXCEPTION 'array_divide_scalar failed';
	   return;
        END IF;
END;
$$ LANGUAGE plpgsql;

-- array_greatest_elements
CREATE OR REPLACE FUNCTION array_greatest_elements_test()
RETURNS void as $$
DECLARE
BEGIN
	IF (array_greatest_elements(ARRAY[8, 4], ARRAY[1, 90]) <> ARRAY[8, 90]) THEN
	   RAISE EXCEPTION 'array_greatest_elements failed';
	   return;
	END IF;
	IF (array_greatest_elements(ARRAY[8.4, 4.56], ARRAY[3.0, 159.159]) <> ARRAY[8.4, 159.159]) THEN
	   RAISE EXCEPTION 'array_greatest_elements failed';
	   return;
	END IF;
	IF (array_greatest_elements(ARRAY[[2, 7], [1, 8], [2, 8]], array[[3, 1], [4, 1], [5, 9]]) <> ARRAY[[3, 7], [4, 8], [5, 9]]) THEN
	   RAISE EXCEPTION 'array_greatest_elements failed';
	   return;
        END IF;
END;
$$ LANGUAGE plpgsql;

-- array_least_elements
CREATE OR REPLACE FUNCTION array_least_elements_test()
RETURNS void as $$
DECLARE
BEGIN
	IF (array_least_elements(ARRAY[8, 4], ARRAY[1, 90]) <> ARRAY[1, 4]) THEN
	   RAISE EXCEPTION 'array_least_elements failed';
	   return;
	END IF;
	IF (array_least_elements(ARRAY[8.4, 4.56], ARRAY[3.0, 159.159]) <> ARRAY[3.0, 4.56]) THEN
	   RAISE EXCEPTION 'array_least_elements failed';
	   return;
	END IF;
	IF (array_least_elements(ARRAY[[2, 7], [1, 8], [2, 8]], array[[3, 1], [4, 1], [5, 9]]) <> ARRAY[[2, 1], [1, 1], [2, 8]]) THEN
	   RAISE EXCEPTION 'array_least_elements failed';
	   return;
        END IF;
END;
$$ LANGUAGE plpgsql;

-- array_min test
DROP AGGREGATE IF EXISTS array_min_agg_test (anyarray);
CREATE AGGREGATE array_min_agg_test (anyarray)
(
    sfunc = array_least_elements,
    stype = anyarray,
    initcond = '{}'
);

CREATE OR REPLACE FUNCTION array_min_test()
RETURNS void as $$
DECLARE
	v_arr int[];
BEGIN
	CREATE TABLE array_min_test_table (id varchar(10), arr double precision[]);
	INSERT INTO array_min_test_table VALUES (1, ARRAY[4, 5]);
	INSERT INTO array_min_test_table VALUES (1, ARRAY[8, 9]);
	INSERT INTO array_min_test_table VALUES (1, ARRAY[1, 7]);

	select array_min_agg_test(arr) into v_arr from array_min_test_table;
	IF (v_arr <> ARRAY[1, 5]) THEN
	   RAISE EXCEPTION 'array_min failed';
	   return;
	END IF;   
	
	DROP TABLE IF EXISTS array_min_test_table;
END;
$$ LANGUAGE plpgsql;

-- array_max test
DROP AGGREGATE IF EXISTS array_max_agg_test (anyarray);
CREATE AGGREGATE array_max_agg_test (anyarray)
(
    sfunc = array_greatest_elements,
    stype = anyarray,
    initcond = '{}'
);

CREATE OR REPLACE FUNCTION array_max_test()
RETURNS void as $$
DECLARE
	v_arr int[];
BEGIN
	CREATE TABLE array_max_test_table (id varchar(10), arr double precision[]);
	INSERT INTO array_max_test_table VALUES (1, ARRAY[4, 5]);
	INSERT INTO array_max_test_table VALUES (1, ARRAY[8, 9]);
	INSERT INTO array_max_test_table VALUES (1, ARRAY[1, 7]);

	select array_max_agg_test(arr) into v_arr from array_max_test_table;
	IF (v_arr <> ARRAY[8, 9]) THEN
	   RAISE EXCEPTION 'array_max failed';
	   return;
	END IF;   
	
	DROP TABLE IF EXISTS array_max_test_table;
END;
$$ LANGUAGE plpgsql;

select array_subtract(ARRAY[[2, 7], [1, 8], [2, 8]], array[[3, 1], [4, 1], [5, 9]]);
select array_add_test();
select array_subtract_test();
select array_multiply_scalar_test();
select array_divide_scalar_test();
select array_greatest_elements_test();
select array_least_elements_test();
select array_min_test();
select array_max_test();