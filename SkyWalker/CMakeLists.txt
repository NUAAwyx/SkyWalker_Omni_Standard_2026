cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and CPP C languages

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

enable_language(C ASM CXX)

# 本CMakeLists.txt仿照CubeMX生成的CMakeLists.txt编写

# 以下为设置CMake的设置部分
# 设定头文件路径
set(SkyWalker_Include_Dirs
        # BSP部分
        ${CMAKE_CURRENT_SOURCE_DIR}/BSP/SPI
        ${CMAKE_CURRENT_SOURCE_DIR}/BSP/USART
        ${CMAKE_CURRENT_SOURCE_DIR}/BSP/CAN

        # Modules部分
        # Device
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/BMI088
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/VOFA
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Motor/DJI
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Motor/DM
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/DR16
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Referee
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Communication

        # Algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Algorithm/EKF
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Algorithm/PID

        # Applications部分
        ${CMAKE_CURRENT_SOURCE_DIR}/Applications/Chassis
        ${CMAKE_CURRENT_SOURCE_DIR}/Applications/Gimbal
        ${CMAKE_CURRENT_SOURCE_DIR}/Applications/AimBooster

        # Task部分
        ${CMAKE_CURRENT_SOURCE_DIR}/Task
)

#设定各个模块源文件路径
#Task
set(SkyWalker_Task_Src
        ${CMAKE_CURRENT_SOURCE_DIR}/Task/skywalker_task.cpp
)

# Applications
set(SkyWalker_Applications_Src
        ${CMAKE_CURRENT_SOURCE_DIR}/Applications/Chassis/app_chassis.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Applications/Gimbal/app_gimbal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Applications/AimBooster/app_aimbooster.cpp

)

# Modules
set(SkyWalker_Modules_Src
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/BMI088/dvc_bmi088.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/VOFA/dvc_vofa.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Motor/DJI/dvc_motor_dji.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Motor/DM/dvc_motor_dm.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/DR16/dvc_dr16.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Referee/dvc_referee.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Device/Communication/dvc_communication.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Algorithm/EKF/alg_ekf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Algorithm/PID/alg_pid.cpp
)

# BSP
set(SkyWalker_BSP_Src
        ${CMAKE_CURRENT_SOURCE_DIR}/BSP/SPI/bsp_spi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BSP/USART/bsp_usart.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BSP/CAN/bsp_can.cpp
)

# 设定需要链接的库的路径和库文件
# 链接现成的库文件，比如第三方.a文件
set(SkyWalker_LINK_DIRS
)
# 链接自己项目内的库文件
set(SkyWalker_LINK_LIBS
)

# 设置部分结束

# 以下为CMake的库的创建与链接部分
# 以下为创建库的部分
# 创建XTYF静态库（不需要连接头文件目录）
add_library(SkyWalker STATIC)
target_sources(SkyWalker PRIVATE
        ${SkyWalker_BSP_Src}
        ${SkyWalker_Modules_Src}
        ${SkyWalker_Applications_Src}
        ${SkyWalker_Task_Src}
)

# 创建SkyWalker接口库
add_library(SkyWalker_Interface INTERFACE)
target_include_directories(SkyWalker_Interface INTERFACE ${SkyWalker_Include_Dirs})
# 创建库部分结束
# 以下为链接库的部分
# 链接SkyWalker静态库与接口库（以减少主项目编译时间）
target_link_libraries(SkyWalker PUBLIC SkyWalker_Interface stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SkyWalker_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${SkyWalker_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# 创建库与链接库部分结束

# 以下为验证C/C++标准版本部分
# C 标准检查
if(CMAKE_C_STANDARD AND CMAKE_C_STANDARD VERSION_LESS 11)
    message(FATAL_ERROR "C code requires C11 or higher (requested: ${CMAKE_C_STANDARD})")
endif()

# C++ 标准检查
if(CMAKE_CXX_STANDARD AND CMAKE_CXX_STANDARD VERSION_LESS 14)
    message(FATAL_ERROR "C++ code requires C++14 or higher (requested: ${CMAKE_CXX_STANDARD})")
endif()
# 验证部分结束
