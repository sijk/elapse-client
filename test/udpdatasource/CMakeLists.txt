find_package(Qt5Network)

add_elapse_test(
    SOURCES     udpdatasource_test.cpp
                ${ELAPSE_ROOT_SOURCE_DIR}/coreplugin/udpdatasource.cpp
    LIBRARIES   elements
                Qt5::Network
    INCLUDES    ${ELAPSE_ROOT_SOURCE_DIR}/coreplugin
)
