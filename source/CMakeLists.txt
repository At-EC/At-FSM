target_sources(fsm_kernel
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/hsm.c
    ${CMAKE_CURRENT_LIST_DIR}/psm.c
)

target_include_directories(fsm_kernel
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(fsm_kernel kernel_include)
