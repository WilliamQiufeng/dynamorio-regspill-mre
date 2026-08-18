cmake_minimum_required(VERSION 3.14)

project(dynamorio_regspill_mre C ASM)

if(NOT DEFINED DynamoRIO_DIR)
  set(DynamoRIO_DIR "$ENV{DYNAMORIO_ROOT}/cmake" CACHE PATH
      "DynamoRIO cmake package directory")
endif()

find_package(DynamoRIO REQUIRED)

add_library(regspill_mre SHARED regspill_mre.c)
configure_DynamoRIO_client(regspill_mre)
use_DynamoRIO_extension(regspill_mre drmgr)
use_DynamoRIO_extension(regspill_mre drreg)

add_executable(cbr_stolen cbr_stolen.S)
target_link_options(cbr_stolen PRIVATE -nostdlib -static)
