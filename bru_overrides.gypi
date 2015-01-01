# this overrides the defaults specified in bru_common.gyp. To override these
# don't edit bru_common.gyp, but bru_overrides.gyp instead.
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
}