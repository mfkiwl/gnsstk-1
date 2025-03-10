#Tests for GNSSCore Classes

add_executable(Convhelp_T Convhelp_T.cpp)
target_link_libraries(Convhelp_T gnsstk)
add_test(NAME GNSSCore_Convhelp COMMAND $<TARGET_FILE:Convhelp_T>)

add_executable(IonoModel_T IonoModel_T.cpp)
target_link_libraries(IonoModel_T gnsstk)
add_test(NAME GNSSCore_IonoModel COMMAND $<TARGET_FILE:IonoModel_T>)

add_executable(IonoModelStore_T IonoModelStore_T.cpp)
target_link_libraries(IonoModelStore_T gnsstk)
add_test(NAME GNSSCore_IonoModelStore COMMAND $<TARGET_FILE:IonoModelStore_T>)

add_executable(ObsID_T ObsID_T.cpp)
target_link_libraries(ObsID_T gnsstk)
add_test(NAME GNSSCore_ObsID COMMAND $<TARGET_FILE:ObsID_T>)

add_executable(ObsIDInitializer_T ObsIDInitializer_T.cpp)
target_link_libraries(ObsIDInitializer_T gnsstk)
add_test(NAME GNSSCore_ObsIDInitializer COMMAND $<TARGET_FILE:ObsIDInitializer_T>)

add_executable(Position_T Position_T.cpp)
target_link_libraries(Position_T gnsstk)
add_test(NAME GNSSCore_Position COMMAND $<TARGET_FILE:Position_T>)

add_executable(TropModel_T TropModel_T.cpp)
target_link_libraries(TropModel_T gnsstk)
add_test(NAME GNSSCore_TropModel COMMAND $<TARGET_FILE:TropModel_T>)

add_executable(WxObsMap_T WxObsMap_T.cpp)
target_link_libraries(WxObsMap_T gnsstk)
add_test(NAME GNSSCore_WxObsMap COMMAND $<TARGET_FILE:WxObsMap_T>)

add_executable(Xvt_T Xvt_T.cpp)
target_link_libraries(Xvt_T gnsstk)
add_test(NAME GNSSCore_Xvt COMMAND $<TARGET_FILE:Xvt_T>)

###############################################################################
add_executable(Tropdump Tropdump.cpp)
target_link_libraries(Tropdump gnsstk)

add_test(NAME TropModel_Global
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Global
         -DTESTNAME=TropModel_Global
         -DARGS=--trop\ Global
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Global PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_Saas
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Saas
         -DTESTNAME=TropModel_Saas
         -DARGS=--trop\ Saas
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Saas PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_GCAT
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_GCAT
         -DTESTNAME=TropModel_GCAT
         -DARGS=--trop\ GCAT
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_GCAT PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_GG
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_GG
         -DTESTNAME=TropModel_GG
         -DARGS=--trop\ GG
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_GG PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_GGHeight
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_GGHeight
         -DTESTNAME=TropModel_GGHeight
         -DARGS=--trop\ GGHeight
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_GGHeight PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_MOPS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_MOPS
         -DTESTNAME=TropModel_MOPS
         -DARGS=--trop\ MOPS
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_MOPS PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_NB
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_NB
         -DTESTNAME=TropModel_NB
         -DARGS=--trop\ NB
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_NB PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_Simple
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Simple
         -DTESTNAME=TropModel_Simple
         -DARGS=--trop\ Simple
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Simple PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_Zero
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GNSSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GNSSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Zero
         -DTESTNAME=TropModel_Zero
         -DARGS=--trop\ Zero
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Zero PROPERTY LABELS Tropmodel)

add_executable(SatMetaDataStore_T SatMetaDataStore_T.cpp)
target_link_libraries(SatMetaDataStore_T gnsstk)
add_test(NAME GNSSCore_SatMetaDataStore COMMAND $<TARGET_FILE:SatMetaDataStore_T>)

add_executable(RinexObsID_T RinexObsID_T.cpp)
target_link_libraries(RinexObsID_T gnsstk)
add_test(NAME GNSSCore_RinexObsID COMMAND $<TARGET_FILE:RinexObsID_T>)

add_executable(RinexSatID_T RinexSatID_T.cpp)
target_link_libraries(RinexSatID_T gnsstk)
add_test(NAME GNSSCore_RinexSatID COMMAND $<TARGET_FILE:RinexSatID_T>)

add_executable(SatID_T SatID_T.cpp)
target_link_libraries(SatID_T gnsstk)
add_test(NAME GNSSCore_SatID COMMAND $<TARGET_FILE:SatID_T>)

add_executable(XmitAnt_T XmitAnt_T.cpp)
target_link_libraries(XmitAnt_T gnsstk)
add_test(NAME GNSSCore_XmitAnt COMMAND $<TARGET_FILE:XmitAnt_T>)

add_executable(FreqConv_T FreqConv_T.cpp)
target_link_libraries(FreqConv_T gnsstk)
add_test(NAME GNSSCore_FreqConv COMMAND $<TARGET_FILE:FreqConv_T>)

add_executable(AngleReduced_T AngleReduced_T.cpp)
target_link_libraries(AngleReduced_T gnsstk)
add_test(NAME GNSSCore_AngleReduced COMMAND $<TARGET_FILE:AngleReduced_T>)

add_executable(Angle_T Angle_T.cpp)
target_link_libraries(Angle_T gnsstk)
add_test(NAME GNSSCore_Angle COMMAND $<TARGET_FILE:Angle_T>)

add_executable(AngleType_T AngleType_T.cpp)
target_link_libraries(AngleType_T gnsstk)
add_test(NAME GNSSCore_AngleType COMMAND $<TARGET_FILE:AngleType_T>)

###############################################################################
###############################################################################
