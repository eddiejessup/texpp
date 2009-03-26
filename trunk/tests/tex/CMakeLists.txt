add_executable(test_tex test_tex.cc)

add_executable(test_source test_source.cc)
target_link_libraries(test_source libtexpp)

file(GLOB TEX_TEST_FILES "*.tex")
foreach(_tex_file ${TEX_TEST_FILES})
    get_filename_component(_tex_file_name ${_tex_file} NAME_WE)

    add_test(test_tex_${_tex_file_name} ${EXECUTABLE_OUTPUT_PATH}/test_tex
         ${TEX_EXECUTABLE} ${TEXPP_EXECUTABLE} ${_tex_file})
    set_property(TEST test_tex_${_tex_file_name} PROPERTY TIMEOUT 2)

    add_test(test_source_${_tex_file_name} ${EXECUTABLE_OUTPUT_PATH}/test_source
         ${_tex_file})
    set_property(TEST test_source_${_tex_file_name} PROPERTY TIMEOUT 2)
endforeach(_tex_file ${TEX_TEST_FILES})

