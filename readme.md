This here is a small sample application that uses bru to pull its external
module dependencies, which in this case are google protocol buffers 
(bru:protobuf), 0mq (bru:zeromq) and googletest.

[![Build Status](https://travis-ci.org/KjellSchubert/bru-sample.svg?branch=master)](https://travis-ci.org/KjellSchubert/bru-sample)
[![Coverage Status](https://coveralls.io/repos/KjellSchubert/bru-sample/badge.png?branch=master)](https://coveralls.io/r/KjellSchubert/bru-sample?branch=master)

For general instructions on how to install bru itself see 
[here](https://github.com/KjellSchubert/bru). Here the instructions for how
to specifically set up bru in a c9.io custom workspace: 
go to c9.io, sign in with your github account, clone your github fork of
https://github.com/KjellSchubert/bru-sample into c9. Then in your c9 
workspace hit Window - Terminal. As of the time of writing gcc --version shows 
4.8.2 is installed, but no clang. To install clang in c9.io do:

```
apt-cache search clang # => 3.5 available
sudo apt-get install clang-3.5 
# set env vars as listed in bru_overrides.gyp
export CXX=`which clang++`
export CC=`which clang++`
export LINK=`which clang++`
```

Install gyp, which bru needs to generate make or vcproj files:

```
sudo apt-get install gyp
```

Install bru & create a 'bru.sh' symlink for convenience:

```
cd ~
git clone https://github.com/KjellSchubert/bru.git
sudo ln `pwd`/bru/bru.sh /usr/bin/bru
```

Now use bru to install & test dependencies listed in foo.bru & foo.gyp:

```
cd ~/workspace
bru install  # note this looks for dependencies in ./*.bru
# alternatively explicitly install deps via: bru install protobuf zeromq googletest
bru test # should build & pass module tests
```

Note that after bru install instead of running 'bru make' or 'bru test' you
can invoke gyp & make (or ninja) manually to build upstream dependencies, but
'bru make' is usually more convenient (and works the same on Linux & Windows).
Also if you don't want to use c9.io: these same installation instruction apply
to Ubuntu.

Building the test application
===

You can build or rebuild the test app via 'bru make', which by default builds
the Debug configuration. Note that during this step gyp generated a Makefile,
so if afterwards you wanted to skip the bru & gyp steps you could build via
make directly:

```
make BUILDTYPE=Debug
```

On Ubuntu gyp generates makefiles for make, and on Windows vcxproj files for 
VisualStudio 2010 (or whatever VS you have installed?). To generate project files 
for other build systems like ninja or VS2012 for example run gyp with a different 
generator.

Running the test application
===

To run tests (using googletest as test framework):

```
out/Debug/addressbook-test
```

To run the client/server tests run:

```
out/Debug/server
# in a second shell:
out/Debug/client_req
```

See also .travis.yml for more details on build & test steps.