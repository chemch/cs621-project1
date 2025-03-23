# CS621 - Project 1 - End to End Detection of Network Compression

## About

This project was built by Chase Chemero for the Spring 2025 USFCA Semester (CS621).

All required features were implemented in this application as laid out in the project assignment specification and the grading rubric. 

Project Motivation: https://www.cs.usfca.edu/vahab/resources/compression_detection.pdf

## Overview

This project implements two applications to detect compression on network paths:

	1.	Detection within a Cooperative Environment i.e. Client and Server working together.
	2.	Standalone Client Side Detection i.e. Client must determine on its own.

The goal is to determine if compression is applied to traffic by measuring differences in packet timing.

## Project Structure

cs621-project1/
├── part1-cooperative/      # Client/Server application
    ├── p1_app/             # C files that are built for client/server apps
    ├── p1_configuration/   # C files for handling configuration
    ├── p1_error/           # C files for error handling
    ├── p1_json/            # C files for reading jsons and handling jsons
    ├── p1_transport/       # C files for UDP and TCP (Transport Layer) functions
    ├── Makefile            # Part 1 Makefile
├── part2-standalone/       # Standalone detection tool
    ├── p2_app/             # C files that are built for client/server apps
    ├── p2_configuration/   # C files for handling configuration
    ├── p2_error/           # C files for error handling
    ├── p2_flow/            # C files for trigggering to end flows (Head Syn, UDP Train, Tail Syn)
    ├── p2_json/            # C files for reading jsons and handling jsons
    ├── p2_transport/       # C files for UDP and TCP (Transport Layer) functions
    ├── Makefile            # Part 1 Makefile
└── README.md               # Project documentation

## How to Build/Run

To build and run follow these steps:

### Part 1 Instructions

1. CD into the Part1 folder
2. Run the Makefile for both the Server and the Client

    - sudo make

3. Start the Server Side

    - sudo ./bin/compdetect_server 7777

4. Start the Client Side

    - sudo ./bin/compdetect_client config_1.json

### Part 2 

1. CD into the Part2 folder
2. Run the Makefile for both the Server and the Client

    - sudo make

3. Start the Client Side (Server must be running)

    - sudo ./bin/compdetect config_1.json

<b>NOTE: There are multiple config files to try different setting combinations<b>

## References

For this project I would like to cite the following sources which I used to understand examples and/or help me refine different components within the application:

- p1/p2_json: Credit to Dave Gamble who produced the JSON library
- p2_flow: Credit to Beej's Guide to C Programming for MultiThreading Examples
    - https://beej.us/guide/bgc/html/split/multithreading.html#multithreading
- p2_transport/cRawTCP.c: https://www.tenouk.com/Module43a.html & https://docs.oracle.com/cd/E36784_01/html/E36875/setsockopt-3socket.html
- Copilot: Helped me with auto-completions, generating documentation and beautifying the code
- p2_transport/cRawTCP.c: Credit to David Hoze for his CheckSum Implementation:
    - https://gist.github.com/david-hoze/0c7021434796997a4ca42d7731a7073a
- p2_transport/cRawTCP.c: Referenced some good examples from Maxxor on how to manually craft TCP packets or listen for RSTs
    - https://github.com/MaxXor/raw-sockets-example/blob/master/rawsockets.c


## Example Log:

### Part 1 Example Log:

CONFIG SENT SUCCESSFULLY TO 192.168.19.130:7777 

***PRE-PROBE PHASE COMPLETED SUCCESSFULLY!***

***PROBE PHASE STARTED***

SLEEPING 3 SECONDS PRIOR TO LOW ETROPY TRAIN...

SOCKET SET UP SUCCESSFULLY.

SENT LOW ENTROPY PACKETS SUCCESSFULLY.

SLEEPING 15 SECONDS PRIOR TO HIGH ENTROPY TRAIN...

SENT HIGH ENTROPY PACKETS SUCCESSFULLY.

***PROBE PHASE COMPLETED SUCCESSFULLY!***

***POST-PROBE PHASE STARTED***

SLEEPING 30 SECONDS BEFORE POST PROBE PHASE TRANSMISSION...

No compression detected.

---

### Part 2 Example Log:


*****************STARTING A FULL FLOW (HEAD SYN, UDP TRAIN, TAIL SYN)*****************

SENDING HEAD SYN TO 192.168.19.130:9999...

RECEIVED RST FROM PORT 9999 AT 1742706558.980419

SENDING LOW ENTROPY UDP TRAIN...

SENDING TAIL SYN TO 192.168.19.130:8888...

RECEIVED RST FROM PORT 8888 AT 1742706559.120241

RST ARRIVAL DELTA: 0.139822 SECONDS

*****************SLEEPING FOR 15 SECONDS BEFORE HIGH ENTROPY TRAIN...*****************


*****************STARTING A FULL FLOW (HEAD SYN, UDP TRAIN, TAIL SYN)*****************

SENDING HEAD SYN TO 192.168.19.130:9999...

RECEIVED RST FROM PORT 9999 AT 1742706574.122878

SENDING HIGH ENTROPY UDP TRAIN...

SENDING TAIL SYN TO 192.168.19.130:8888...

RECEIVED RST FROM PORT 8888 AT 1742706574.242270

RST ARRIVAL DELTA: 0.119392 SECONDS

DELTA DIFFERENCE (HIGH - LOW): -0.020430 SECONDS

*****************COMPRESSION CALC RESULT*****************

No compression was detected.

# Project Requirements:

## Part 1: Rubric Items

1. Config File: part1/p1_configuration
2. TCP Probing Phase: part1/compdetect_client.c
3. All Packets Have Correct Size: Pcap shows each packet with length of 100 bytes 
4. DF Bit Set: Pcap shows DF Bit = 1
5. Packet IDs are Set/Retrieved: Log shows Packet IDs retrieved on Server Side and Pcap Shows incrementing ID in first 2 bytes
6. Low/High Entropy Set: Pcap shows all zero for low entropy and random values for High Train
7. Destination Port/Source Port: Set in cUDP.c in function: transmit_udp_train
8. Error Handling: p1_error/*

## Part 2: Rubric Items

1. TTL is Set: Set on line 32 on cUDP.c in part2/transport/
2. Head/Syn Packets: Have TCP Dest Port/IP from the config and use an ephemeral Source Port per specs
3. Capturing RSTs: Set in cRawTCP.c. Both Socket and RST listener have timeouts and will print required error message if nothing is captured. 
4. Timeout Set: Line 172 in cRawTCP.c is set. 
5. Error Handling: p2_error/*

