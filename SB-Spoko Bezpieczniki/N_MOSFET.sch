EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 17 20
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
L Device:Q_NMOS_DGS Q?
U 1 1 61F87DF2
P 6350 4050
AR Path="/61F87DF2" Ref="Q?"  Part="1" 
AR Path="/61F846F6/61F87DF2" Ref="Q1"  Part="1" 
AR Path="/6231E241/61F87DF2" Ref="Q2"  Part="1" 
AR Path="/62432F8F/61F87DF2" Ref="Q3"  Part="1" 
AR Path="/625EC444/61F87DF2" Ref="Q4"  Part="1" 
AR Path="/6262347B/61F87DF2" Ref="Q5"  Part="1" 
F 0 "Q2" V 6700 4000 50  0000 L CNN
F 1 "Q_NMOS_DGS" V 6600 3800 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6550 4150 50  0001 C CNN
F 3 "~" H 6350 4050 50  0001 C CNN
	1    6350 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 4450 6450 4250
Wire Wire Line
	6150 4050 6050 4050
$Comp
L Device:R_Small R?
U 1 1 61F87DFA
P 6050 4250
AR Path="/61F87DFA" Ref="R?"  Part="1" 
AR Path="/61F846F6/61F87DFA" Ref="R22"  Part="1" 
AR Path="/6231E241/61F87DFA" Ref="R26"  Part="1" 
AR Path="/62432F8F/61F87DFA" Ref="R29"  Part="1" 
AR Path="/625EC444/61F87DFA" Ref="R32"  Part="1" 
AR Path="/6262347B/61F87DFA" Ref="R35"  Part="1" 
F 0 "R26" V 6150 4250 50  0000 L CNN
F 1 "10k" V 5950 4200 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6050 4250 50  0001 C CNN
F 3 "~" H 6050 4250 50  0001 C CNN
	1    6050 4250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6050 4450 6050 4350
Wire Wire Line
	6050 4150 6050 4050
Wire Wire Line
	6450 4450 6450 4550
Wire Wire Line
	6050 4450 6450 4450
Connection ~ 6450 4450
$Comp
L Device:R_Small R?
U 1 1 61F87E05
P 5850 4050
AR Path="/61F87E05" Ref="R?"  Part="1" 
AR Path="/61F846F6/61F87E05" Ref="R21"  Part="1" 
AR Path="/6231E241/61F87E05" Ref="R25"  Part="1" 
AR Path="/62432F8F/61F87E05" Ref="R28"  Part="1" 
AR Path="/625EC444/61F87E05" Ref="R31"  Part="1" 
AR Path="/6262347B/61F87E05" Ref="R34"  Part="1" 
F 0 "R25" V 5950 4050 50  0000 L CNN
F 1 "100" V 5750 4000 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 5850 4050 50  0001 C CNN
F 3 "~" H 5850 4050 50  0001 C CNN
	1    5850 4050
	0    -1   1    0   
$EndComp
Wire Wire Line
	6050 4050 5950 4050
$Comp
L Device:R_Small R?
U 1 1 61F87E0C
P 6450 3650
AR Path="/61F87E0C" Ref="R?"  Part="1" 
AR Path="/61F846F6/61F87E0C" Ref="R23"  Part="1" 
AR Path="/6231E241/61F87E0C" Ref="R27"  Part="1" 
AR Path="/62432F8F/61F87E0C" Ref="R30"  Part="1" 
AR Path="/625EC444/61F87E0C" Ref="R33"  Part="1" 
AR Path="/6262347B/61F87E0C" Ref="R36"  Part="1" 
F 0 "R27" V 6550 3650 50  0000 L CNN
F 1 "0" V 6350 3650 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6450 3650 50  0001 C CNN
F 3 "~" H 6450 3650 50  0001 C CNN
	1    6450 3650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6450 3750 6450 3850
$Comp
L power:GND #PWR?
U 1 1 61F87E13
P 6450 4550
AR Path="/61F87E13" Ref="#PWR?"  Part="1" 
AR Path="/61F846F6/61F87E13" Ref="#PWR0106"  Part="1" 
AR Path="/6231E241/61F87E13" Ref="#PWR0107"  Part="1" 
AR Path="/62432F8F/61F87E13" Ref="#PWR0124"  Part="1" 
AR Path="/625EC444/61F87E13" Ref="#PWR0125"  Part="1" 
AR Path="/6262347B/61F87E13" Ref="#PWR0126"  Part="1" 
F 0 "#PWR0107" H 6450 4300 50  0001 C CNN
F 1 "GND" H 6455 4377 50  0000 C CNN
F 2 "" H 6450 4550 50  0001 C CNN
F 3 "" H 6450 4550 50  0001 C CNN
	1    6450 4550
	-1   0    0    -1  
$EndComp
Connection ~ 6050 4050
Wire Wire Line
	6450 3550 6450 3450
Wire Wire Line
	5750 4050 5650 4050
Text HLabel 6450 3450 1    50   Input ~ 0
IN
Text HLabel 5650 4050 0    50   Input ~ 0
CRL
$EndSCHEMATC
