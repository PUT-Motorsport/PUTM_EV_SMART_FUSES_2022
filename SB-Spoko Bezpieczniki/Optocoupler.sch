EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 20
Title "Wheel Speed Sensor digital input"
Date "2021-11-09"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR03
U 1 1 616CE551
P 6650 4500
AR Path="/616C0F0A/616CE551" Ref="#PWR03"  Part="1" 
AR Path="/619E05A1/616CE551" Ref="#PWR044"  Part="1" 
AR Path="/619CEBD1/616CE551" Ref="#PWR?"  Part="1" 
AR Path="/619CEBD5/616CE551" Ref="#PWR?"  Part="1" 
AR Path="/61B6A745/616CE551" Ref="#PWR033"  Part="1" 
AR Path="/61B6BD3C/616CE551" Ref="#PWR?"  Part="1" 
AR Path="/61B72450/616CE551" Ref="#PWR037"  Part="1" 
AR Path="/61B7897B/616CE551" Ref="#PWR041"  Part="1" 
AR Path="/61B7C076/616CE551" Ref="#PWR045"  Part="1" 
AR Path="/61B82715/616CE551" Ref="#PWR049"  Part="1" 
AR Path="/61B8593E/616CE551" Ref="#PWR053"  Part="1" 
F 0 "#PWR053" H 6650 4250 50  0001 C CNN
F 1 "GND" H 6655 4327 50  0000 C CNN
F 2 "" H 6650 4500 50  0001 C CNN
F 3 "" H 6650 4500 50  0001 C CNN
	1    6650 4500
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR02
U 1 1 616CEB8B
P 6650 3500
AR Path="/616C0F0A/616CEB8B" Ref="#PWR02"  Part="1" 
AR Path="/619E05A1/616CEB8B" Ref="#PWR038"  Part="1" 
AR Path="/619CEBD1/616CEB8B" Ref="#PWR?"  Part="1" 
AR Path="/619CEBD5/616CEB8B" Ref="#PWR?"  Part="1" 
AR Path="/61B6A745/616CEB8B" Ref="#PWR032"  Part="1" 
AR Path="/61B6BD3C/616CEB8B" Ref="#PWR?"  Part="1" 
AR Path="/61B72450/616CEB8B" Ref="#PWR036"  Part="1" 
AR Path="/61B7897B/616CEB8B" Ref="#PWR040"  Part="1" 
AR Path="/61B7C076/616CEB8B" Ref="#PWR044"  Part="1" 
AR Path="/61B82715/616CEB8B" Ref="#PWR048"  Part="1" 
AR Path="/61B8593E/616CEB8B" Ref="#PWR052"  Part="1" 
F 0 "#PWR052" H 6650 3350 50  0001 C CNN
F 1 "+3V3" H 6665 3673 50  0000 C CNN
F 2 "" H 6650 3500 50  0001 C CNN
F 3 "" H 6650 3500 50  0001 C CNN
	1    6650 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 3500 6650 3800
Wire Wire Line
	6650 4500 6650 4450
Wire Wire Line
	6950 4150 7650 4150
$Comp
L Device:C_Small C2
U 1 1 616D3668
P 6850 3800
AR Path="/616C0F0A/616D3668" Ref="C2"  Part="1" 
AR Path="/619E05A1/616D3668" Ref="C19"  Part="1" 
AR Path="/619CEBD1/616D3668" Ref="C?"  Part="1" 
AR Path="/619CEBD5/616D3668" Ref="C?"  Part="1" 
AR Path="/61B6A745/616D3668" Ref="C16"  Part="1" 
AR Path="/61B6BD3C/616D3668" Ref="C?"  Part="1" 
AR Path="/61B72450/616D3668" Ref="C18"  Part="1" 
AR Path="/61B7897B/616D3668" Ref="C20"  Part="1" 
AR Path="/61B7C076/616D3668" Ref="C22"  Part="1" 
AR Path="/61B82715/616D3668" Ref="C24"  Part="1" 
AR Path="/61B8593E/616D3668" Ref="C26"  Part="1" 
F 0 "C26" V 6621 3800 50  0000 C CNN
F 1 "100nF" V 6712 3800 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6850 3800 50  0001 C CNN
F 3 "~" H 6850 3800 50  0001 C CNN
	1    6850 3800
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 616D3DA7
P 7000 3800
AR Path="/616C0F0A/616D3DA7" Ref="#PWR04"  Part="1" 
AR Path="/619E05A1/616D3DA7" Ref="#PWR045"  Part="1" 
AR Path="/619CEBD1/616D3DA7" Ref="#PWR?"  Part="1" 
AR Path="/619CEBD5/616D3DA7" Ref="#PWR?"  Part="1" 
AR Path="/61B6A745/616D3DA7" Ref="#PWR034"  Part="1" 
AR Path="/61B6BD3C/616D3DA7" Ref="#PWR?"  Part="1" 
AR Path="/61B72450/616D3DA7" Ref="#PWR038"  Part="1" 
AR Path="/61B7897B/616D3DA7" Ref="#PWR042"  Part="1" 
AR Path="/61B7C076/616D3DA7" Ref="#PWR046"  Part="1" 
AR Path="/61B82715/616D3DA7" Ref="#PWR050"  Part="1" 
AR Path="/61B8593E/616D3DA7" Ref="#PWR054"  Part="1" 
F 0 "#PWR054" H 7000 3550 50  0001 C CNN
F 1 "GND" V 7005 3672 50  0000 R CNN
F 2 "" H 7000 3800 50  0001 C CNN
F 3 "" H 7000 3800 50  0001 C CNN
	1    7000 3800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6750 3800 6650 3800
Connection ~ 6650 3800
Wire Wire Line
	6650 3800 6650 3850
Wire Wire Line
	7000 3800 6950 3800
Text Notes 4200 5250 0    50   ~ 0
PR08-2DP AUTONICS -> PNP / NO \nhttps://www.instrumart.com/assets/Autonics_PR_Series_Proximity_Sensors_datasheet.pdf\nhttps://www.tme.eu/pl/details/pr08-2dp/czujniki-indukcyjne-walcowe-dc/autonics/
$Comp
L Device:D_Schottky_Small D1
U 1 1 618E4C31
P 5850 4200
AR Path="/616C0F0A/618E4C31" Ref="D1"  Part="1" 
AR Path="/619E05A1/618E4C31" Ref="D3"  Part="1" 
AR Path="/619CEBD1/618E4C31" Ref="D?"  Part="1" 
AR Path="/619CEBD5/618E4C31" Ref="D?"  Part="1" 
AR Path="/61B6A745/618E4C31" Ref="D12"  Part="1" 
AR Path="/61B6BD3C/618E4C31" Ref="D?"  Part="1" 
AR Path="/61B72450/618E4C31" Ref="D13"  Part="1" 
AR Path="/61B7897B/618E4C31" Ref="D14"  Part="1" 
AR Path="/61B7C076/618E4C31" Ref="D15"  Part="1" 
AR Path="/61B82715/618E4C31" Ref="D16"  Part="1" 
AR Path="/61B8593E/618E4C31" Ref="D17"  Part="1" 
F 0 "D17" V 5850 4270 50  0000 L CNN
F 1 "D_SCH" V 5895 4270 50  0001 L CNN
F 2 "Diode_SMD:D_0603_1608Metric" V 5850 4200 50  0001 C CNN
F 3 "~" V 5850 4200 50  0001 C CNN
	1    5850 4200
	0    1    1    0   
$EndComp
Wire Wire Line
	5350 4050 5450 4050
Connection ~ 5350 4050
Wire Wire Line
	5350 4100 5350 4050
Wire Wire Line
	5250 4050 5350 4050
Wire Wire Line
	5350 4300 5350 4350
$Comp
L Device:C_Small C1
U 1 1 616CCBB0
P 5350 4200
AR Path="/616C0F0A/616CCBB0" Ref="C1"  Part="1" 
AR Path="/619E05A1/616CCBB0" Ref="C14"  Part="1" 
AR Path="/619CEBD1/616CCBB0" Ref="C?"  Part="1" 
AR Path="/619CEBD5/616CCBB0" Ref="C?"  Part="1" 
AR Path="/61B6A745/616CCBB0" Ref="C15"  Part="1" 
AR Path="/61B6BD3C/616CCBB0" Ref="C?"  Part="1" 
AR Path="/61B72450/616CCBB0" Ref="C17"  Part="1" 
AR Path="/61B7897B/616CCBB0" Ref="C19"  Part="1" 
AR Path="/61B7C076/616CCBB0" Ref="C21"  Part="1" 
AR Path="/61B82715/616CCBB0" Ref="C23"  Part="1" 
AR Path="/61B8593E/616CCBB0" Ref="C25"  Part="1" 
F 0 "C25" H 5442 4246 50  0000 L CNN
F 1 "10nF" H 5442 4155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5350 4200 50  0001 C CNN
F 3 "~" H 5350 4200 50  0001 C CNN
	1    5350 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 616CC3EC
P 5600 4050
AR Path="/616C0F0A/616CC3EC" Ref="R2"  Part="1" 
AR Path="/619E05A1/616CC3EC" Ref="R23"  Part="1" 
AR Path="/619CEBD1/616CC3EC" Ref="R?"  Part="1" 
AR Path="/619CEBD5/616CC3EC" Ref="R?"  Part="1" 
AR Path="/61B6A745/616CC3EC" Ref="R8"  Part="1" 
AR Path="/61B6BD3C/616CC3EC" Ref="R?"  Part="1" 
AR Path="/61B72450/616CC3EC" Ref="R10"  Part="1" 
AR Path="/61B7897B/616CC3EC" Ref="R12"  Part="1" 
AR Path="/61B7C076/616CC3EC" Ref="R14"  Part="1" 
AR Path="/61B82715/616CC3EC" Ref="R16"  Part="1" 
AR Path="/61B8593E/616CC3EC" Ref="R18"  Part="1" 
F 0 "R18" V 5393 4050 50  0000 C CNN
F 1 "4k7" V 5484 4050 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5530 4050 50  0001 C CNN
F 3 "~" H 5600 4050 50  0001 C CNN
	1    5600 4050
	0    1    1    0   
$EndComp
Wire Wire Line
	5350 4350 5850 4350
Wire Wire Line
	5850 4350 5850 4300
Wire Wire Line
	5750 4050 5850 4050
Wire Wire Line
	5850 4100 5850 4050
Connection ~ 5850 4050
Wire Wire Line
	5850 4050 6350 4050
Wire Wire Line
	4950 4050 4850 4050
Wire Wire Line
	6350 4250 6150 4250
Wire Wire Line
	6150 4250 6150 4350
Wire Wire Line
	6150 4350 5850 4350
Connection ~ 5850 4350
Text Notes 4150 3650 0    50   ~ 0
+24V sensor\n3mA @ 24V\nVmax (28 - 1.1) / 9400 Ohm = 2.8mA\nVmin (24 - 1.1) / 9400 Ohm = 2.4mA
$Comp
L power:GND #PWR01
U 1 1 619B83B9
P 5850 4500
AR Path="/616C0F0A/619B83B9" Ref="#PWR01"  Part="1" 
AR Path="/619E05A1/619B83B9" Ref="#PWR037"  Part="1" 
AR Path="/619CEBD1/619B83B9" Ref="#PWR?"  Part="1" 
AR Path="/619CEBD5/619B83B9" Ref="#PWR?"  Part="1" 
AR Path="/61B6A745/619B83B9" Ref="#PWR031"  Part="1" 
AR Path="/61B6BD3C/619B83B9" Ref="#PWR?"  Part="1" 
AR Path="/61B72450/619B83B9" Ref="#PWR035"  Part="1" 
AR Path="/61B7897B/619B83B9" Ref="#PWR039"  Part="1" 
AR Path="/61B7C076/619B83B9" Ref="#PWR043"  Part="1" 
AR Path="/61B82715/619B83B9" Ref="#PWR047"  Part="1" 
AR Path="/61B8593E/619B83B9" Ref="#PWR051"  Part="1" 
F 0 "#PWR051" H 5850 4250 50  0001 C CNN
F 1 "GND" H 5855 4327 50  0000 C CNN
F 2 "" H 5850 4500 50  0001 C CNN
F 3 "" H 5850 4500 50  0001 C CNN
	1    5850 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 4500 5850 4350
Text HLabel 4850 4050 0    50   Input ~ 0
IN
Text HLabel 7650 4150 2    50   Output ~ 0
~OUT
$Comp
L Isolator:H11L2 U2
U 1 1 616C86DF
P 6650 4150
AR Path="/616C0F0A/616C86DF" Ref="U2"  Part="1" 
AR Path="/619E05A1/616C86DF" Ref="U8"  Part="1" 
AR Path="/619CEBD1/616C86DF" Ref="U?"  Part="1" 
AR Path="/619CEBD5/616C86DF" Ref="U?"  Part="1" 
AR Path="/61B6A745/616C86DF" Ref="U5"  Part="1" 
AR Path="/61B6BD3C/616C86DF" Ref="U?"  Part="1" 
AR Path="/61B72450/616C86DF" Ref="U6"  Part="1" 
AR Path="/61B7897B/616C86DF" Ref="U7"  Part="1" 
AR Path="/61B7C076/616C86DF" Ref="U8"  Part="1" 
AR Path="/61B82715/616C86DF" Ref="U9"  Part="1" 
AR Path="/61B8593E/616C86DF" Ref="U10"  Part="1" 
F 0 "U10" H 6994 4196 50  0000 L CNN
F 1 "VOH1016AB-T2" H 6994 4105 50  0000 L CNN
F 2 "Package_DIP:SMDIP-6_W7.62mm" H 6560 4150 50  0001 C CNN
F 3 "https://pl.mouser.com/datasheet/2/427/VISH_S_A0007770803_1-2570127.pdf" H 6560 4150 50  0001 C CNN
	1    6650 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 61932E16
P 5100 4050
AR Path="/616C0F0A/61932E16" Ref="R1"  Part="1" 
AR Path="/619E05A1/61932E16" Ref="R22"  Part="1" 
AR Path="/619CEBD1/61932E16" Ref="R?"  Part="1" 
AR Path="/619CEBD5/61932E16" Ref="R?"  Part="1" 
AR Path="/61B6A745/61932E16" Ref="R7"  Part="1" 
AR Path="/61B6BD3C/61932E16" Ref="R?"  Part="1" 
AR Path="/61B72450/61932E16" Ref="R9"  Part="1" 
AR Path="/61B7897B/61932E16" Ref="R11"  Part="1" 
AR Path="/61B7C076/61932E16" Ref="R13"  Part="1" 
AR Path="/61B82715/61932E16" Ref="R15"  Part="1" 
AR Path="/61B8593E/61932E16" Ref="R17"  Part="1" 
F 0 "R17" V 4893 4050 50  0000 C CNN
F 1 "4k7" V 4984 4050 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5030 4050 50  0001 C CNN
F 3 "~" H 5100 4050 50  0001 C CNN
	1    5100 4050
	0    1    1    0   
$EndComp
$EndSCHEMATC
