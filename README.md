# slang
This is language based on jlox from the book https://craftinginterpreters.com/ by Robert Nystrom. It is written in c++.
slang has slightly different syntax, but essentially it is the same.

## Features
Additionally, slang supports while-else loops:
```slang
while (some_boolean) {
    // if some_boolean is true it will run body as a regular while loop
    print "true";
} else {
    // if and only if some_boolean is false on first iteration, it will run else branch
    print "false"
}
```

also added support for `break` statements, and arrow functions:
```slang
fn add(a, b) => a + b;
// body of the arrow function is an expression that is implicitly wrapped in the return statement.
```

Also, slang has different than jlox memory management, since it does not rely on JVM garbage collector,
instead it uses a simple reference counting mechanism.

## Build
Requirements:
- A c++17 compiler

Run `./build.sh` script with `init` for the initial build:
```bash
./build.sh init
```

`./build.sh` with `rebuild` for rebuild:
```bash
./build.sh rebuild
```

and just `./build` for regular build:
```bash
./build.sh
```

