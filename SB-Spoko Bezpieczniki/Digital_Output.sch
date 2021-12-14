EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 14 20
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 4100 4400 0    50   Input ~ 0
input
Text HLabel 5950 4450 2    50   Output ~ 0
output
Wire Notes Line
	7100 4450 7400 4450
Wire Notes Line width 14
	7400 4400 7400 4500
Wire Notes Line width 14
	7500 4400 7450 4400
Wire Notes Line width 14
	7450 4400 7450 4500
Wire Notes Line width 14
	7450 4500 7500 4500
Wire Notes Line
	7450 5000 7550 5000
Wire Notes Line
	7400 4950 7600 4950
Wire Notes Line
	7350 4900 7650 4900
Wire Notes Line
	7300 4450 7300 4550
Wire Notes Line
	7250 4550 7250 4750
Wire Notes Line
	7250 4750 7350 4750
Wire Notes Line
	7350 4750 7350 4550
Wire Notes Line
	7250 4550 7350 4550
Wire Notes Line
	7300 4750 7300 4800
Wire Notes Line
	7300 4800 7500 4800
Wire Notes Line
	7500 4500 7500 4900
Wire Notes Line
	7550 4200 7550 4050
Wire Notes Line
	7550 4050 7450 4050
Wire Notes Line
	7450 4050 7450 4200
Wire Notes Line
	7500 4200 7500 4400
Wire Notes Line
	7450 4200 7550 4200
Wire Notes Line
	7500 4050 7500 3800
Text Notes 7350 4750 1    50   ~ 0
10k
Text Notes 7550 4200 1    50   ~ 0
0R
Text Notes 7600 4500 0    50   ~ 0
NMOS\nBUK7D25-40E
Text Notes 7400 3100 0    50   ~ 0
+24V
Wire Notes Line
	7500 3100 7500 3250
Wire Notes Line
	7400 3250 7600 3250
Wire Notes Line
	7600 3250 7600 3800
Wire Notes Line
	7600 3800 7400 3800
Wire Notes Line
	7400 3800 7400 3250
Text Notes 7550 3800 1    100  Italic 0
device
Text Notes 7200 5200 0    50   ~ 0
On SB board
$Comp
L power:+5V #PWR060
U 1 1 61A9FB37
P 4800 3850
AR Path="/61A904FC/61A9FB37" Ref="#PWR060"  Part="1" 
AR Path="/61A6BCFB/61A9FB37" Ref="#PWR064"  Part="1" 
AR Path="/61E969CF/61A9FB37" Ref="#PWR0112"  Part="1" 
AR Path="/61E9FD3E/61A9FB37" Ref="#PWR0120"  Part="1" 
AR Path="/61ECEEF9/61A9FB37" Ref="#PWR0116"  Part="1" 
F 0 "#PWR0116" H 4800 3700 50  0001 C CNN
F 1 "+5V" H 4815 4023 50  0000 C CNN
F 2 "" H 4800 3850 50  0001 C CNN
F 3 "" H 4800 3850 50  0001 C CNN
	1    4800 3850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR061
U 1 1 61A9FD58
P 4800 4850
AR Path="/61A904FC/61A9FD58" Ref="#PWR061"  Part="1" 
AR Path="/61A6BCFB/61A9FD58" Ref="#PWR065"  Part="1" 
AR Path="/61E969CF/61A9FD58" Ref="#PWR0113"  Part="1" 
AR Path="/61E9FD3E/61A9FD58" Ref="#PWR0121"  Part="1" 
AR Path="/61ECEEF9/61A9FD58" Ref="#PWR0117"  Part="1" 
F 0 "#PWR0117" H 4800 4600 50  0001 C CNN
F 1 "GND" H 4805 4677 50  0000 C CNN
F 2 "" H 4800 4850 50  0001 C CNN
F 3 "" H 4800 4850 50  0001 C CNN
	1    4800 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 4850 4800 4800
Wire Wire Line
	4800 4800 4900 4800
Wire Wire Line
	4900 4800 4900 4750
Connection ~ 4800 4800
Wire Wire Line
	4800 4800 4800 4750
Wire Wire Line
	4100 4400 4200 4400
Text HLabel 4100 4500 0    50   Input ~ 0
~enable
Wire Wire Line
	4100 4500 4500 4500
$Comp
L Device:LED D5
U 1 1 61AA37F7
P 4200 4750
AR Path="/61A904FC/61AA37F7" Ref="D5"  Part="1" 
AR Path="/61A6BCFB/61AA37F7" Ref="D6"  Part="1" 
AR Path="/61E969CF/61AA37F7" Ref="D23"  Part="1" 
AR Path="/61E9FD3E/61AA37F7" Ref="D25"  Part="1" 
AR Path="/61ECEEF9/61AA37F7" Ref="D24"  Part="1" 
F 0 "D24" V 4239 4632 50  0000 R CNN
F 1 "LED_GREEN" V 4148 4632 50  0000 R CNN
F 2 "Diode_SMD:D_0603_1608Metric" H 4200 4750 50  0001 C CNN
F 3 "~" H 4200 4750 50  0001 C CNN
	1    4200 4750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R52
U 1 1 61AA3B2C
P 4200 5150
AR Path="/61A904FC/61AA3B2C" Ref="R52"  Part="1" 
AR Path="/61A6BCFB/61AA3B2C" Ref="R54"  Part="1" 
AR Path="/61E969CF/61AA3B2C" Ref="R41"  Part="1" 
AR Path="/61E9FD3E/61AA3B2C" Ref="R45"  Part="1" 
AR Path="/61ECEEF9/61AA3B2C" Ref="R43"  Part="1" 
F 0 "R43" H 4270 5196 50  0000 L CNN
F 1 "1k" H 4270 5105 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4130 5150 50  0001 C CNN
F 3 "~" H 4200 5150 50  0001 C CNN
	1    4200 5150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR059
U 1 1 61AA3C66
P 4200 5350
AR Path="/61A904FC/61AA3C66" Ref="#PWR059"  Part="1" 
AR Path="/61A6BCFB/61AA3C66" Ref="#PWR063"  Part="1" 
AR Path="/61E969CF/61AA3C66" Ref="#PWR0114"  Part="1" 
AR Path="/61E9FD3E/61AA3C66" Ref="#PWR0122"  Part="1" 
AR Path="/61ECEEF9/61AA3C66" Ref="#PWR0118"  Part="1" 
F 0 "#PWR0118" H 4200 5100 50  0001 C CNN
F 1 "GND" H 4205 5177 50  0000 C CNN
F 2 "" H 4200 5350 50  0001 C CNN
F 3 "" H 4200 5350 50  0001 C CNN
	1    4200 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 5350 4200 5300
Wire Wire Line
	4200 4600 4200 4400
Wire Wire Line
	4200 5000 4200 4900
Connection ~ 4200 4400
Wire Wire Line
	4200 4400 4500 4400
$Comp
L Device:C_Small C29
U 1 1 61AA4F82
P 5050 4100
AR Path="/61A904FC/61AA4F82" Ref="C29"  Part="1" 
AR Path="/61A6BCFB/61AA4F82" Ref="C30"  Part="1" 
AR Path="/61E969CF/61AA4F82" Ref="C38"  Part="1" 
AR Path="/61E9FD3E/61AA4F82" Ref="C40"  Part="1" 
AR Path="/61ECEEF9/61AA4F82" Ref="C39"  Part="1" 
F 0 "C39" V 4821 4100 50  0000 C CNN
F 1 "100nF" V 4912 4100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5050 4100 50  0001 C CNN
F 3 "~" H 5050 4100 50  0001 C CNN
	1    5050 4100
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 3850 4800 4100
Wire Wire Line
	4800 4100 4950 4100
Wire Wire Line
	5150 4100 5200 4100
Connection ~ 4800 4100
Wire Wire Line
	4800 4100 4800 4150
$Comp
L power:GND #PWR062
U 1 1 61AA6A32
P 5200 4100
AR Path="/61A904FC/61AA6A32" Ref="#PWR062"  Part="1" 
AR Path="/61A6BCFB/61AA6A32" Ref="#PWR066"  Part="1" 
AR Path="/61E969CF/61AA6A32" Ref="#PWR0115"  Part="1" 
AR Path="/61E9FD3E/61AA6A32" Ref="#PWR0123"  Part="1" 
AR Path="/61ECEEF9/61AA6A32" Ref="#PWR0119"  Part="1" 
F 0 "#PWR0119" H 5200 3850 50  0001 C CNN
F 1 "GND" V 5205 3972 50  0000 R CNN
F 2 "" H 5200 4100 50  0001 C CNN
F 3 "" H 5200 4100 50  0001 C CNN
	1    5200 4100
	0    -1   -1   0   
$EndComp
Wire Notes Line width 14
	7500 4500 7500 4550
Wire Notes Line width 14
	7500 4350 7500 4400
Wire Notes Line
	7100 4400 7100 4500
Wire Notes Line
	7100 4500 6900 4500
Wire Notes Line
	6900 4500 6900 4400
Wire Notes Line
	6900 4400 7100 4400
Wire Notes Line
	6900 4450 6750 4450
Text Notes 6900 4500 0    50   ~ 0
100R
$Comp
L MCP14A0052T:MCP14A0052T U16
U 1 1 61E992D7
P 4800 4450
AR Path="/61E969CF/61E992D7" Ref="U16"  Part="1" 
AR Path="/61E9FD3E/61E992D7" Ref="U18"  Part="1" 
AR Path="/61ECEEF9/61E992D7" Ref="U17"  Part="1" 
F 0 "U17" H 5244 4496 50  0000 L CNN
F 1 "MCP14A0052T" H 5244 4405 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 4800 3850 50  0001 C CIN
F 3 "" H 4600 4700 50  0001 C CNN
	1    4800 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 4450 5950 4450
$EndSCHEMATC
