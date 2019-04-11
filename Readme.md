# Ten CLI
A simple command line interface to the Ten programming language.
Currently only Unix systems are supported, and only a GNUmakefile
is implemented, so for non-GNU toolchains will have to build
manually.  It's pretty simple, so that shouldn't be much of an
issue.

## Build and Install
To build Ten CLI either clone or download this repo, and CD into
its directory.  Then do:

    make
    sudo make install

Or if `make` doesn't link to `gmake` on your system, then use `gmake`
explicitly instead.

## Usage
I'll just copy the help menu here:

    ten
      : launch a REPL
    ten script
      : run a script file
    ten [-v | --version]
      : show version and copyright info
    ten [-h | --help]
      : show this help message

The CLI currently doesn't support passing command line
arguments to the script.  That'll be added fairly soon.
