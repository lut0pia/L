# Script reference

## Math

- `clamp(x, min, max)` returns `min` if `x < min`, `max` if `x > max`, or `x` otherwise
- `max(...)` returns the maximum value of all its parameters
- `min(...)` returns the minimum value of all its parameters
- `rand()` returns a random float from 0 to 1

### Vectors

- `cross(a, b)` returns the cross product of 3-float vectors `a` and `b`
- `distance(a, b)` returns the distance between 3-float vectors `a` and `b`
- `dot(a, b)` returns the dot product of 3-float vectors `a` and `b`
- `length(v)` returns the length of 3-float vector `v`
- `normalize(v)` returns the normalized 3-float vector `v`
- `vec(x, y, z)` returns a 3-float vector from its 3 parameters

### Scalars

- `pow(x, p)` returns float `x` raised to the power `p`
- `sqrt(x)` returns the square root of float `x`
- `sin(x)` returns `sin(x)`
- `cos(x)` returns `cos(x)`
- `tan(x)` returns `tan(x)`
- `abs(x)` returns absolute value of `x`
- `floor(x)` returns closest lesser integer to `x`
- `ceil(x)` returns closest greater integer to `x`

## Time

- `now()` returns the current time
- `time(s)` returns time from seconds

## Miscellaneous

- `count(o)` returns the number of values in table `o`
- `print(...)` prints all its parameters in order to stdout
- `typename(v)` returns the name of the type of `v` as a string
