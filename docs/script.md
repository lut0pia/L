# Script reference

## Syntax
L-script's syntax is inspired from Lisp. Everything looks like `(command parameter...)`. Occasionally it looks like `(object'method | parameter...)`. The `|` symbol is really just syntactic sugar to mean "put everything before in parentheses", so the previous example actually extends to `((object'method) parameter...)`. The `'` symbol means that the next word should be returned as a token, not a value.

## Variables
L-script uses local and global variables. Local variables are only accessible in the current scope (the body of a function or a script file). By default, variables are global, you can define local variables with the keyword `local`. You're advised to work only in your local scope to avoid collisions between scripts (although referring to the same variable from different scripts may be intended, in which case use global scope).

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
	(print "I\n")
	(print "am\n")
	(print "leg\n")
)
```

## Objects
When the first part of a list is not a command but an object (as in `(object something)`), the result of the expression is the attribute of the object that has the key `something`.

* `object` can be used to create an object. Each pair of parameters is a key and a value. There also exists syntactic sugar for object creation in the form of `{}`
```clojure
(do
	(local o (object 1 "truc" 3 "machin"))
	(print (o 3)) ; Prints machin
	(local o2 {"lel" 32}) ; Alternative syntax
)
```

## Functions
* `fun` can be used to create a function. It takes either a list of symbols (its parameters) and an instruction or just an instruction.
```clojure
(local fart (fun (print "PROUT!")))
(local do-twice (fun (f) (do (f) (f))))
(do-twice fart) ; Prints PROUT!PROUT!
```

## Conditions
* `if` can be used to execute different instructions depending on conditions. You can have as many pairs of condition-instruction as you want, they'll be tested in order and the if instruction will return at the first validated condition. If no condition is validated and there's a sole instruction after the pairs, then that instruction is executed.
```clojure
(if
	condition (do-thing) ; if condition then do-thing
	other-condition (do-other-thing) ; else if other-condition then do-other-thing
	(do-whatever) ; else do-whatever
)
```
* `switch` can be used to execute different instructions depending on a value. You can have as many pairs of match-instruction as you want, they'll be tested in order and the switch instruction will return at the first validated match. If no match is validated and there's a sole instruction after the pairs, then that instruction is executed.
```clojure
(switch value
	3 (do-thing) ; if value=3 then do-thing
	"foo" (do-other-thing) ; if value="foo" then do-other-thing
	(do-whatever) ; else do-whatever
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
	(print k ": " v "\n")
))
; This example prints:
; test: 2
; 3: 4
```

## Miscellaneous
* `print` prints all its parameters in order to stdout.
* `rand` returns a random float from 0 to 1.
* `vec` returns a 3-float vector from its 3 parameters.
* `typename` returns the name of the type of its parameter as a string.
