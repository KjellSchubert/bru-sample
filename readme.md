This here is experimental work in progress.

For instructions on how to install bru itself see 
[here](https://github.com/KjellSchubert/bru). This here is a small sample
application that uses bru to pull its external module dependencies.

Building the test application
===

The same steps should work on both Windows & Ubuntu with only few differences:

>git clone https://github.com/KjellSchubert/bru-sample.git
>cd bru-sample
>python3 bru.py install foo.bru # TODO: syntax will likely change
>gyp --depth=. foo.gyp # TODO: avoid the --depth=.

On Ubuntu this will generate makefiles for make, and on Windows vcxproj files for 
VisualStudio 2010 (or whatever VS you have installed?). To generate project files 
for other build systems like ninja or VS2012 for example run gyp with a different 
generator:

Building the test application on Ubuntu
---

Generate a Makefile for make:

>gyp --depth=. foo.gyp

To build via make:

>make

To run the generated executables:

>out/Default/foo-test
>out/Default/googletest_sample1

Building the test application on Windows
---

Generate vcproj files for VS2012 (assuming that's the VS version you have 
installed):

>gyp --depth=. foo.gyp -G msvs_version=2012

Now open ./foo.sln in VS2012 and build the solution, or alternatively build with 
msbuild via command line:

>C:\Windows\Microsoft.NET\Framework\v4.0.30319\msbuild.exe foo.sln

Then run the generated executables:

>Default\foo-test.exe
>Default\googletest_sample1.exe

Note that many of the VS compiler & linker settings get defaults via
common.gypi in the root of your project. This common.gypi is included
by the gyp files of the individual modules, e.g. see 
bru_modules\googletest\googletest.gyp with its "includes" directive.

