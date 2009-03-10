add_executable(test_tex test_tex.cc)

file(GLOB TEX_TEST_FILES "*.tex")
foreach(_tex_file ${TEX_TEST_FILES})
    get_filename_component(_tex_file_name ${_tex_file} NAME_WE)
    add_test(test_tex_${_tex_file_name} ${EXECUTABLE_OUTPUT_PATH}/test_tex
         ${TEX_EXECUTABLE} ${TEXPP_EXECUTABLE} ${_tex_file})
    set_property(TEST test_tex_${_tex_file_name} PROPERTY TIMEOUT 2)
endforeach(_tex_file ${TEX_TEST_FILES})

