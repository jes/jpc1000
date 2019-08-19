EESchema Schematic File Version 4
LIBS:mainboard-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x11_Female J1
U 1 1 5D5D4D6D
P 3500 3300
F 0 "J1" H 3392 2575 50  0000 C CNN
F 1 "Conn_01x11_Female" H 3392 2666 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x11_P2.54mm_Vertical" H 3500 3300 50  0001 C CNN
F 3 "~" H 3500 3300 50  0001 C CNN
	1    3500 3300
	-1   0    0    1   
$EndComp
$Comp
L MCU_Module:Arduino_Nano_v2.x A1
U 1 1 5D5D5E79
P 6150 2850
F 0 "A1" H 6150 1761 50  0000 C CNN
F 1 "Arduino_Nano_v2.x" H 6150 1670 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 6300 1900 50  0001 L CNN
F 3 "https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf" H 6150 1850 50  0001 C CNN
	1    6150 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3800 3850 3800
Wire Wire Line
	5150 3800 5150 3950
Wire Wire Line
	6150 3950 6150 3850
Wire Wire Line
	6250 3850 6150 3850
Connection ~ 6150 3850
Wire Wire Line
	3700 3700 5050 3700
Wire Wire Line
	5050 3700 5050 4200
Wire Wire Line
	5050 4200 7000 4200
Wire Wire Line
	7000 4200 7000 1700
Wire Wire Line
	7000 1700 6350 1700
Wire Wire Line
	6350 1700 6350 1800
Wire Wire Line
	3700 3600 5600 3600
Wire Wire Line
	5650 3600 5650 3550
Wire Wire Line
	3700 3500 5450 3500
Wire Wire Line
	5450 3500 5450 3350
Wire Wire Line
	5450 3350 5650 3350
Wire Wire Line
	3700 3400 5300 3400
Wire Wire Line
	5300 3400 5300 2550
Wire Wire Line
	5300 2550 5650 2550
Wire Wire Line
	3700 3300 5200 3300
Wire Wire Line
	5200 3300 5200 2750
Wire Wire Line
	5200 2750 5650 2750
Wire Wire Line
	3700 3200 4850 3200
Wire Wire Line
	4850 3200 4850 2650
Wire Wire Line
	4850 2650 5650 2650
Wire Wire Line
	3700 3100 5450 3100
Wire Wire Line
	5450 3100 5450 2950
Wire Wire Line
	5450 2950 5650 2950
Wire Wire Line
	3700 3000 5550 3000
Wire Wire Line
	5550 3050 5650 3050
Wire Wire Line
	5650 2900 5650 2850
Wire Wire Line
	3700 2900 5650 2900
Wire Wire Line
	5550 3000 5550 3050
Wire Wire Line
	3700 2800 5500 2800
Wire Wire Line
	5500 2800 5500 3150
Wire Wire Line
	5500 3150 5650 3150
Wire Wire Line
	5650 3250 4200 3250
Wire Wire Line
	4200 3250 4200 1750
$Comp
L Relay_SolidState:34.81-8240 U1
U 1 1 5D5E4379
P 4300 1450
F 0 "U1" V 4346 1270 50  0000 R CNN
F 1 "34.81-8240" V 4255 1270 50  0000 R CNN
F 2 "jes:Omron-G3MB-202P" H 4100 1250 50  0001 L CIN
F 3 "https://gfinder.findernet.com/public/attachments/34/EN/S34USAEN.pdf" H 4275 1450 50  0001 L CNN
	1    4300 1450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4400 1750 4400 2450
Wire Wire Line
	4400 2450 3200 2450
Wire Wire Line
	3200 2450 3200 4000
Wire Wire Line
	3200 4000 3850 4000
Wire Wire Line
	3850 4000 3850 3800
Connection ~ 3850 3800
Wire Wire Line
	3850 3800 5150 3800
$Comp
L Connector:Screw_Terminal_01x02 J2
U 1 1 5D5E6E15
P 4300 800
F 0 "J2" V 4264 612 50  0000 R CNN
F 1 "Screw_Terminal_01x02" V 4173 612 50  0000 R CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 4300 800 50  0001 C CNN
F 3 "~" H 4300 800 50  0001 C CNN
	1    4300 800 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4300 1000 4300 1150
Wire Wire Line
	4300 1150 4200 1150
Wire Wire Line
	4400 1000 4400 1150
$Comp
L Converter_ACDC:HLK-PM01 PS1
U 1 1 5D5E9CE9
P 7350 1250
F 0 "PS1" H 7350 933 50  0000 C CNN
F 1 "HLK-PM01" H 7350 1024 50  0000 C CNN
F 2 "Converter_ACDC:Converter_ACDC_HiLink_HLK-PMxx" H 7350 950 50  0001 C CNN
F 3 "http://www.hlktech.net/product_detail.php?ProId=54" H 7750 900 50  0001 C CNN
	1    7350 1250
	-1   0    0    1   
$EndComp
Wire Wire Line
	6950 1350 6700 1350
Wire Wire Line
	6350 1350 6350 1700
Connection ~ 6350 1700
Wire Wire Line
	5150 3950 6150 3950
Connection ~ 6150 3950
Wire Wire Line
	6150 3950 6950 3950
$Comp
L Connector:Screw_Terminal_01x02 J4
U 1 1 5D5EEE01
P 8300 1200
F 0 "J4" H 8380 1192 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 8380 1101 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 8300 1200 50  0001 C CNN
F 3 "~" H 8300 1200 50  0001 C CNN
	1    8300 1200
	-1   0    0    1   
$EndComp
Wire Wire Line
	8100 1200 7750 1200
Wire Wire Line
	7750 1200 7750 1150
Wire Wire Line
	7750 1350 8100 1350
Wire Wire Line
	8100 1350 8100 1300
Wire Wire Line
	5100 1700 5100 1800
Wire Wire Line
	5100 1800 6350 1800
Connection ~ 6350 1800
Wire Wire Line
	6350 1800 6350 1850
Wire Wire Line
	5300 1700 5300 1500
Wire Wire Line
	6950 1500 6950 3950
Wire Wire Line
	5400 1700 5400 3450
Wire Wire Line
	5400 3450 5650 3450
Wire Wire Line
	5500 1700 5500 2450
Wire Wire Line
	5500 2450 5650 2450
Wire Wire Line
	5600 1700 5600 3600
Connection ~ 5600 3600
Wire Wire Line
	5600 3600 5650 3600
$Comp
L Connector:Conn_01x06_Female J3
U 1 1 5D5F3BFD
P 5300 1500
F 0 "J3" V 5238 1112 50  0000 R CNN
F 1 "Conn_01x06_Female" V 5147 1112 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical" H 5300 1500 50  0001 C CNN
F 3 "~" H 5300 1500 50  0001 C CNN
	1    5300 1500
	0    -1   -1   0   
$EndComp
NoConn ~ 5200 1700
NoConn ~ 5650 2250
NoConn ~ 5650 2350
NoConn ~ 6650 2250
NoConn ~ 6650 2350
NoConn ~ 6650 2650
NoConn ~ 6050 1850
NoConn ~ 6250 1850
NoConn ~ 6650 2850
NoConn ~ 6650 2950
NoConn ~ 6650 3050
NoConn ~ 6650 3150
NoConn ~ 6650 3250
NoConn ~ 6650 3350
NoConn ~ 6650 3450
NoConn ~ 6650 3550
Wire Wire Line
	5300 1500 6800 1500
Wire Wire Line
	6800 1500 6800 1150
Wire Wire Line
	6800 1150 6900 1150
Connection ~ 6800 1500
Wire Wire Line
	6800 1500 6950 1500
$Comp
L Device:CP C1
U 1 1 5D616E1F
P 6900 1300
F 0 "C1" H 7018 1346 50  0000 L CNN
F 1 "CP" H 7018 1255 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 6938 1150 50  0001 C CNN
F 3 "~" H 6900 1300 50  0001 C CNN
	1    6900 1300
	1    0    0    -1  
$EndComp
Connection ~ 6900 1150
Wire Wire Line
	6900 1150 6950 1150
Wire Wire Line
	6900 1450 6700 1450
Wire Wire Line
	6700 1450 6700 1350
Connection ~ 6700 1350
Wire Wire Line
	6700 1350 6350 1350
Wire Wire Line
	8500 1300 8500 1200
Wire Wire Line
	8100 1300 8500 1300
Wire Wire Line
	8100 1200 8100 1000
Wire Wire Line
	8100 1000 8500 1000
Wire Wire Line
	8500 1000 8500 1100
$EndSCHEMATC
