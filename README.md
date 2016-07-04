## libpgam : PostgreSQL extensions for Array Math

### Math Functions

1. array_add(a  anyarray,b  anyarray)
1. array_subtract(a  anyarray,b  anyarray)
1. array_multiply_scalar(a  anyarray,scalar numeric)
1. array_divide_scalar(a  anyarray,scalar numeric)

### Array as Set

1. array_greatest_elements(a  anyarray,b  anyarray)
1. array_least_elements(a  anyarray,b  anyarray)
1. array_argmin(a  anyarray)
1. array_argmax(a  anyarray)
1. array_minval(a  anyarray)
1. array_maxval(a  anyarray)

### Other Set Like Things
1. find_percentile_location(integer[],double  precision)
1. sum_elements(integer[])
1.  
### Vector Distances

1. p1_norm_length_normalized(double precision[],double precision[])
1. p1_norm(double precision[],double precision[])

