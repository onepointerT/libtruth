#
# Copyright 2025 The OnePointer Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required( VERSION 3.22 )

project( libtruth CXX C )

set( PROJECT_NAME "libtruth" )
set( PROJECT_DESCRIPTION "Basic and general concepts for truth evaluation with C++ from OnePointer UG." )
set( PROJECT_VERSION "0.1.2" )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_C_STANDARD 23 )

add_definitions( -DTRUTH_HAVE_INI_WRITE )

include_directories( . )
include_directories( ./truth )
include_directories( ./truth/oneptr/mINI/src/ )
include_directories( ./truth/oneptr/log/src )

add_subdirectory( truth/oneptr/log )
add_subdirectory( truth/oneptr/mINI )

file( GLOB ONEPTR_TRUTH_SRC truth/truth*.c* )
add_library( oneptr_truth ${ONEPTR_TRUTH_SRC} )
target_link_libraries( oneptr_truth logcpp mINI )

file( GLOB ONEPTR_BOOL_SRC truth/bool*.cpp )
add_library( oneptr_bool ${ONEPTR_BOOL_SRC} )
target_link_libraries( oneptr_bool oneptr_truth )
