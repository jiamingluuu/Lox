Lox is an object-oriented language prototype that supports a minimal amount of 
functionality of a programming language.

## Quick Start
To compile the executable
```sh
make clox
```

The generated executable supports interactive mode:
```sh
>$ ./clox

# here begins the prompt of clox
> print "hello world";
hello world
```
Use `Ctrl+d` to exit.

Or you can execute the source code of a entire file:
```sh
> $ ./clox example/fib.lox
_______________________________
Factorial of 3 is:
6.000000
_______________________________
First 10 Fibonacci numbers are:
0.000000
1.000000
1.000000
2.000000
3.000000
5.000000
8.000000
13.000000
21.000000
34.000000
```