# this overrides the defaults specified in bru_common.gyp. To override these
# don't edit bru_common.gyp, but bru_overrides.gyp instead.
# These settings here apply to both the compilation of upstream dependencies
# as well as the compilation of ./src itself.
{
    # this forces the use of clang without having to export CXX=clang++ on
    # the command line.
    # Note that this forces clang for all targets: upstream deps and foo.gyp
    # local targets.
    # P.S.: this works in c9.io but fails for travis-ci, probably because
    # travis-ci installs clang elsewhere?
    #"make_global_settings": [
    #    ["CXX", "/usr/bin/clang++"],
    #    ["CC", "/usr/bin/clang"],
    #    ["LINK", "/usr/bin/clang++"]
    #],
    # So set these env vars via command line or ~/.bashrc instead:
    #   > export CXX=`which clang++`
    #   > export CC=`which clang`
    #   > export LINK=`which clang++`
    # For travis-ci these exports are in .travis.yml

    "target_defaults": {
        "cflags": [
            # on the current clang (3.4) travis-ci build VMs I get errors
            # for std::chrono::steady_clock, which in c9.io (with clang 3.5) 
            # compiles fine. So let's see if specifying libc++ fixes the 
            # travis-ci error:
            #   "-stdlib=libc++" # the stdlib from http://libcxx.llvm.org/
            # Other options are Apache's libstdcxx, GNU's libstdc++, STLport, ...:
            #   "-stdlib=libstdc++"
        ]
    }
}