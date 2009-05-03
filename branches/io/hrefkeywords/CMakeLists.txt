set(HREFTEST_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/hreftest.py PARENT_SCOPE)
set(HREFKEYWORDS_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/hrefkeywords.py PARENT_SCOPE)

install(FILES latexstubs.py DESTINATION bin)
install(PROGRAMS hrefkeywords.py hreftest.py texpp.py DESTINATION bin)

