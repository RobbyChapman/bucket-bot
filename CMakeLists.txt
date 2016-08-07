cmake_minimum_required(VERSION 3.5)
project(bucket_bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    controller/N64Controller.c
    controller/N64Controller.h
    examples/combined.c
    examples/motor.c
    examples/utils.c
    motor/MotorController.c
    motor/MotorController.h
    receiver/N64_DTO.h
    receiver/XbeeReceiver.c
    receiver/XbeeReceiver.h
    utils/BotUtils.c
    utils/BotUtils.h)

add_executable(bucket_bot ${SOURCE_FILES})