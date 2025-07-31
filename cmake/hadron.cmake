function(add_hadron_support target_name)
    target_compile_options(${target_name} BEFORE PUBLIC -include ${CMAKE_SOURCE_DIR}/libraries/hadron.h)

    if (WIN32)
        target_link_libraries(${target_name} ${CMAKE_SOURCE_DIR}/libraries/libhadron_win.a)
    elseif (UNIX)
        target_link_libraries(${target_name} ${CMAKE_SOURCE_DIR}/libraries/libhadron_unix.a)
    endif()

    target_link_libraries(${target_name} m ${CMAKE_SOURCE_DIR}/libraries/DbgHelp.Lib)

    if (WIN32)
        add_custom_command(TARGET ${target_name}
                POST_BUILD
                COMMAND ../cv2pdb.exe $<TARGET_FILE:${target_name}> $<TARGET_FILE_DIR:${target_name}>/tempPdb.exe $<TARGET_FILE_DIR:${target_name}>/${target_name}.pdb
        )
    endif()
endfunction()
