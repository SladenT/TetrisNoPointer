# Argument 4 is VERBOSE, determines whether we print as we match.  Defaults to false
# Argument 5 is an exclude list
# Argument 6 is recursive include.  It defaults to false
function(glob_sources project srcdirs srcmatcher)

# Set Variables
set(verbose false)
if(${ARGC} GREATER_EQUAL 4)
    if(${ARGV3})
        set(verbose true)
    endif()
endif()

set(globvar GLOB)
if(${ARGC} GREATER_EQUAL 6)
    if(${ARGV5})
        set(globvar GLOB_RECURSE)
        if(${verbose})
            message(STATUS "Globbing sources recursively...")
        endif()
    elseif(${verbose})
        message(STATUS "Globbing sources...")
    endif()
elseif(${verbose})
    message(STATUS "Globbing sources...")
endif()

# Glob files
set(filelist)
foreach(dir ${srcdirs})
    file(GLOB
         filesindir
         ${dir}${srcmatcher}
         LIST_DIRECTORIES false
         CONFIUGURE_DEPENDS)
    LIST(APPEND filelist ${filesindir})
endforeach()

if(${verbose})
    foreach(srcfile ${filelist})
        message(STATUS "Globbing file: ${srcfile}")
    endforeach()
endif()

# Set Sources
target_sources(
    ${project}
    PRIVATE
        "${filelist}")
        
endfunction()
