EESchema Schematic File Version 4
LIBS:frontpanel-cache
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
L Connector:Conn_01x07_Female J2
U 1 1 5D5B1BD4
P 3550 2900
F 0 "J2" V 3715 2880 50  0000 C CNN
F 1 "Conn_01x07_Female" V 3624 2880 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x07_P2.54mm_Vertical" H 3550 2900 50  0001 C CNN
F 3 "~" H 3550 2900 50  0001 C CNN
	1    3550 2900
	0    -1   -1   0   
$EndComp
Text Label 2150 3200 0    50   ~ 0
gnd
Text Label 2150 3300 0    50   ~ 0
vcc
Text Label 2150 3400 0    50   ~ 0
clk
Text Label 2150 3500 0    50   ~ 0
mosi
Wire Wire Line
	3250 3100 3250 3200
Wire Wire Line
	3350 3300 2500 3300
Wire Wire Line
	3450 3400 2500 3400
Wire Wire Line
	3450 3100 3450 3400
Wire Wire Line
	3550 3100 3550 3500
Wire Wire Line
	3550 3500 2500 3500
Wire Wire Line
	3250 3200 2750 3200
Wire Wire Line
	5150 2650 2750 2650
Wire Wire Line
	2750 2650 2750 3200
Connection ~ 2750 3200
Wire Wire Line
	2750 3200 2500 3200
Wire Wire Line
	4600 4450 5150 4450
Connection ~ 4600 4450
Wire Wire Line
	4600 4300 4600 4450
Wire Wire Line
	4050 4450 4600 4450
Connection ~ 4050 4450
Wire Wire Line
	4050 4300 4050 4450
Connection ~ 5150 4300
Wire Wire Line
	5150 4300 5150 2650
Wire Wire Line
	4750 4300 4750 3900
Wire Wire Line
	4200 4300 4200 4000
Wire Wire Line
	3650 4300 3650 4100
Wire Wire Line
	3100 4300 3100 4200
Wire Wire Line
	5150 4450 5150 4300
Wire Wire Line
	3500 4450 4050 4450
Wire Wire Line
	3500 4300 3500 4450
$Comp
L Switch:SW_Push SW1
U 1 1 5D5B290B
P 4950 4300
F 0 "SW1" H 4950 4585 50  0000 C CNN
F 1 "SW_Push" H 4950 4494 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 4950 4500 50  0001 C CNN
F 3 "~" H 4950 4500 50  0001 C CNN
	1    4950 4300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5D5B271B
P 4400 4300
F 0 "SW2" H 4400 4585 50  0000 C CNN
F 1 "SW_Push" H 4400 4494 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 4400 4500 50  0001 C CNN
F 3 "~" H 4400 4500 50  0001 C CNN
	1    4400 4300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5D5B24DB
P 3850 4300
F 0 "SW3" H 3850 4585 50  0000 C CNN
F 1 "SW_Push" H 3850 4494 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 3850 4500 50  0001 C CNN
F 3 "~" H 3850 4500 50  0001 C CNN
	1    3850 4300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5D5B2051
P 3300 4300
F 0 "SW4" H 3300 4585 50  0000 C CNN
F 1 "SW_Push" H 3300 4494 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 3300 4500 50  0001 C CNN
F 3 "~" H 3300 4500 50  0001 C CNN
	1    3300 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 3100 3850 3800
Wire Wire Line
	3750 3100 3750 3700
Wire Wire Line
	4750 3900 2500 3900
Wire Wire Line
	4200 4000 2500 4000
Wire Wire Line
	3650 4100 2500 4100
Wire Wire Line
	3100 4200 2500 4200
Wire Wire Line
	3850 3800 2500 3800
Wire Wire Line
	3750 3700 2500 3700
Text Label 2150 4200 0    50   ~ 0
cnclbtn
Text Label 2150 4100 0    50   ~ 0
okbtn
Text Label 2150 4000 0    50   ~ 0
dnbtn
Text Label 2150 3900 0    50   ~ 0
upbtn
Text Label 2150 3800 0    50   ~ 0
cs
Text Label 2150 3700 0    50   ~ 0
dc
Text Label 2150 3600 0    50   ~ 0
res
Wire Wire Line
	3350 3100 3350 3300
Wire Wire Line
	3650 3100 3650 3600
Wire Wire Line
	3650 3600 2500 3600
$Comp
L Connector:Conn_01x11_Male J1
U 1 1 5D5AB70F
P 2300 3700
F 0 "J1" H 2408 4381 50  0000 C CNN
F 1 "Conn_01x11_Male" H 2408 4290 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x11_P2.54mm_Vertical" H 2300 3700 50  0001 C CNN
F 3 "~" H 2300 3700 50  0001 C CNN
	1    2300 3700
	1    0    0    -1  
$EndComp
$EndSCHEMATC
