set(dicomlib_src_files
  main.cpp
  lib/Buffer.cpp
  lib/Decoder.cpp
  lib/Encoder.cpp
  lib/File.cpp
  lib/Tag.cpp
  lib/UID.cpp
  lib/UIDs.cpp
  lib/ValueToStream.cpp
  lib/Exceptions.cpp
  lib/Utility.cpp
  lib/FileMetaInformation.cpp
  lib/TransferSyntax.cpp
  lib/DataDictionary.cpp
  lib/Dumper.cpp
  lib/GroupLength.cpp
  lib/AssociationRejection.cpp
  lib/Cdimse.cpp
  lib/ServiceBase.cpp
  lib/pdata.cpp
  lib/aaac.cpp
  lib/aarj.cpp
  lib/aarq.cpp
  lib/CommandSets.cpp
  lib/ClientConnection.cpp
  lib/PresentationContexts.cpp
  lib/QueryRetrieve.cpp
  lib/Version.cpp
  lib/ViewCodeSequence.cpp
)

set(dicom_header_files
  lib/Buffer.hpp
  lib/Decoder.hpp
  lib/Encoder.hpp
  lib/File.hpp
  lib/Tag.hpp
  lib/Types.hpp
  lib/UID.hpp
  lib/UIDs.hpp
  lib/Value.hpp
  lib/ValueToStream.hpp
  lib/VR.hpp
  lib/Exceptions.hpp
  lib/DataSet.hpp
  lib/Utility.hpp
  lib/FileMetaInformation.hpp
  lib/TransferSyntax.hpp
  lib/ImplementationUID.hpp
  lib/DataDictionary.hpp
  lib/Dumper.hpp
  lib/GroupLength.hpp
  lib/dicomlib.hpp
  lib/AssociationRejection.hpp
  lib/Cdimse.hpp
  lib/ServiceBase.hpp
  lib/pdata.hpp
  lib/aaac.hpp
  lib/aarj.hpp
  lib/aarq.hpp
  lib/CommandSets.hpp
  lib/ClientConnection.hpp
  lib/PresentationContexts.hpp
  lib/QueryRetrieve.hpp
  lib/Version.hpp
  lib/ViewCodeSequence.hpp
)

cmake_minimum_required(VERSION 2.8)
project(dicomlib)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Boost 1.60.0 REQUIRED COMPONENTS thread system date_time filesystem)
#find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
#include_directories(${FLTK_INCLUDE_DIR})

#add_library(dicomlib SHARED ${dicomlib_src_files} ${dicom_header_files})

add_executable(dicomlib ${dicomlib_src_files} ${dicom_header_files})

target_link_libraries(dicomlib ${Boost_LIBRARIES})
#target_link_libraries(dicomlib ${FLTK_LIBRARIES})
target_link_libraries(dicomlib ${OPENGL_LIBRARIES})
