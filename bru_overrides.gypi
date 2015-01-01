# this overrides the defaults specified in bru_common.gyp. To override these
# don't edit bru_common.gyp, but bru_overrides.gyp instead.
{
    # this forces the use of clang without having to export CXX=clang++ on
    # the command line.
    # Note that this forces clang for all targets: upstream deps and foo.gyp
    # local targets.
    "make_global_settings": [
        ["CXX", "/usr/bin/clang++"],
        ["CC", "/usr/bin/clang++"],
        ["LINK", "/usr/bin/clang++"]
    ],
    
    "target_defaults": {
        "cflags": [
            # this enables C++11 for clang
            # would it be OK to compile upstream dependencies with C++98,
            # but downstream ones with C++11? Unsure, I guess not.
            "-std=c++11",
            
            # use clang's address sanitizer across all modules
            # (note you need the matching ldflags for this cflag)
            "-fsanitize=address"
            
            # this would enable clang's cxxlib, but we can just the default 
            # gcc stdlib instead
            #"-stdlib=libc++""
        ],

        "ldflags": [
            "-fsanitize=address"
        ]
    }
}