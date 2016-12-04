# Dermis

Dermis is an experiment with libc wrappers. Dermis uses lua for changing
the behavior of libc functions.

## Getting Started

Dermis will be compiled as a shared library that can be used using
LD_PRELOAD to modify the behavior of some programs. Although adding new
functions should be trivial, only the following ones are currently
implemented:

* sleep
* fgets
* malloc

### Prerequisites

* cmake

### Installing

```
cd build
cmake ..
make
```

### Running Examples

```
cd build
./run-example.sh
./run-example.sh sleep
```

### Usage

```
DERMIS="example.lua" LD_PRELOAD="/full/path/to/dermis.so" ./examples/sleep
```

`example.lua`:

```
function sleep(secs)
    print("i should sleep", secs, "seconds... nah")
    return secs
end

dermis_register_sleep(sleep)
```

## Contributing

Just fork and submit a PR.

## License

Dermis is made available under a 3-clause BSD license.
