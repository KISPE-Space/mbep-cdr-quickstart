cmake_minimum_required(VERSION 3.10)

project(satll-sim C)

add_executable(satll-sim
    main.c    
    inc/ecss_crc.c
    inc/space_packet.c
    inc/pus.c
    inc/telemetry.c
    inc/telecommand.c
    inc/udp.c
)