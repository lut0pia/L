# LScript reference

## Function calls

LScript's syntax is inspired from Lisp. Functions calls look like `(function parameter...)`. The `|` symbol is syntactic sugar to mean "put everything before in parentheses", so this `(function parameter | other_parameter)` actually extends to `((function parameter) other_parameter)`. The `'` symbol means that the next word should be returned as a token, not a value.

## Variables
LScript uses local and global variables. Local variables are only accessible in the current scope (the body of a function or a script file). By default, variables are global, you can define local variables with the keyword `local`. You're advised to work only in your local scope to avoid collisions between scripts (although referring to the same variable from different scripts may be intended, in which case use global scope).

* `local` can be used to define a local variable on the stack (the second parameter is the initial value and can be omitted).
```clojure
(local a 4)
(print a) ; Prints 4
```
* `set` can be used to set a variable to a value, be it local or global.
```clojure
(local a)
(set a 42)
(print a) ; Prints 42
```
In this example, if the first line were omitted, the value `42` would have been set to the global variable `a` instead.

## Blocks
* `do` can be used to make sequential instructions into one instruction.
```clojure
(do
	(println "I")
	(println "am")
	(println "leg")
)
```

## Objects

You can access object fields with either `.token` or `:value`.

* `{}` can be used to create an object. Each pair of parameters is a key and a value.
```clojure
(do
	(local o {'hey "truc" 3 "machin"})
	(print o:3) ; Prints machin
	(print o.hey) ; Prints truc
)
```

## Functions
* `fun` can be used to create a function. The last parameter is the function's instruction, all previous parameters must be symbols and are the function's parameter names.
```clojure
(local fart (fun (print "PROUT!")))
(local do_twice (fun f (do (f) (f))))
(do_twice fart) ; Prints PROUT!PROUT!
```

## Conditions
* `if` can be used to execute different instructions depending on conditions. You can have as many pairs of condition-instruction as you want, they'll be tested in order and the if instruction will return at the first validated condition. If no condition is validated and there's a sole instruction after the pairs, then that instruction is executed.
```clojure
(if
	condition (do_thing) ; if condition then do_thing
	other_condition (do_other_thing) ; else if other_condition then do_other_thing
	(do_whatever) ; else do_whatever
)
```
* `switch` can be used to execute different instructions depending on a value. You can have as many pairs of match-instruction as you want, they'll be tested in order and the switch instruction will return at the first validated match. If no match is validated and there's a sole instruction after the pairs, then that instruction is executed.
```clojure
(switch value
	3 (do_thing) ; if value=3 then do_thing
	"foo" (do_other_thing) ; if value="foo" then do_other_thing
	(do_whatever) ; else do_whatever
)
```
* `or` and `and` execute their parameters in order, and do not execute any more instructions as soon as their return value is known.
```clojure
(print (or true false)) ; Prints true
(print (and true false)) ; Prints false
```

## Loops
* `while` can be used to loop while a condition is true.
```clojure
(while running (run))
```
* `foreach` can be used to loop over the elements of an object.
```clojure
(local o {"test" 2 3 4})
(foreach k v o (do
	(println k ": " v)
))
; This example prints:
; test: 2
; 3: 4
```
