{
    "includes": [
        "bru_common.gypi",
        "bru_overrides.gypi"
    ],
    "targets": [
        {
            "target_name": "foo",
            "type": "static_library",
            "sources": [
                "src/addressbook.pb.cc" # generated by protoc action below
            ],
            
            # this scary looking action regenerates the *.pb.cc & h files
            # addressbook.proto. This action was copied from 
            # bru:library/protoc/*.gyp
            "actions": [
                {
                    "action_name": "protoc",
                    "inputs": [
                        "src/addressbook.proto"
                    ],
                    "outputs": [
                        "src/addressbook.pb.cc",
                        "src/addressbook.pb.h"
                    ],
                    "action": [
                        "<(PRODUCT_DIR)/protoc",
                        "<@(_inputs)",
                        "--cpp_out=."
                    ],
                    "message": "invoking protoc"
                }
            ],

            "include_dirs": [
                "include",
                # only needed since that's where protoc dumps addressbook.pb.h
                # which addressbook.pb.h includes "src/addressbook.pb.h".
                # May wanna copy ths file into ./include instead?
                "./"
            ],
            "dependencies": [
                "bru_modules/protobuf/protobuf.gyp:*",
                "bru_modules/zeromq/zeromq.gyp:*"
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "include",
                    "./"
                ]
            }
        },
        {
            "target_name": "foo-test",
            "type": "executable",
            "sources": [
                "test/test_addressbook.cpp"
            ],
            "dependencies": [
                "foo",
                "bru_modules/protobuf/protobuf.gyp:*",
                "bru_modules/googletest/googletest.gyp:*"
            ]
        }
    ]
}