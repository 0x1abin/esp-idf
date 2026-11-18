# SPDX-License-Identifier: Apache-2.0
file(READ "${INPUT}" linker_script)
string(REGEX MATCH "__HEAP_SIZE *= *0x00000C00;" vendor_heap "${linker_script}")
if(NOT vendor_heap)
    message(FATAL_ERROR "Unknown SiFli HCPU linker template. Check the SDK version.")
endif()
string(REPLACE "${vendor_heap}" "__HEAP_SIZE = 0x00018000;" linker_script "${linker_script}")
string(REPLACE ". = . + __HEAP_SIZE;"
    "__sf32lb57_heap_start = .;\n    . = . + __HEAP_SIZE;"
    linker_script "${linker_script}")
string(APPEND linker_script
    "\nASSERT(__HeapLimit - __sf32lb57_heap_start == __HEAP_SIZE, \"Invalid SF32LB57 heap bounds\")\n")
set(temporary "${OUTPUT}.tmp")
file(WRITE "${temporary}" "${linker_script}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${temporary}" "${OUTPUT}" COMMAND_ERROR_IS_FATAL ANY)
file(REMOVE "${temporary}")
