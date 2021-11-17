# C/Migemo library

C/Migemo is regexp pattern generator/expander.

See [README in Japanese](doc/README_j.txt) for details.

## License

Distributed under MIT license.

See LICENSE also.

## Vim plugin

If you want to use this repository only as a vim plugin,
use https://github.com/haya14busa/vim-migemo

----

## For developer

### Unit Tests

To execute unit tests.

    $ cd test
    $ make

### Profiling

Check performance by profiling, before and after your modification.
To get profiling report:

    # prepare dictionaries
    $ cd dict2
    $ make

    # profile: result is in profile.log
    $ cd src/testdir
    $ make profile

### Unit Tests

TODO: write unit tests and its usage.
