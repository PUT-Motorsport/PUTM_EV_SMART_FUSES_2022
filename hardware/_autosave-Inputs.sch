EESchema Schematic File Version 5
EELAYER 36 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 14 24
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
NoConn ~ 9310 5370
Wire Wire Line
	1630 5880 1880 5880
Wire Wire Line
	1680 2940 1930 2940
Wire Wire Line
	1680 4450 1930 4450
Wire Wire Line
	2780 5880 2880 5880
Wire Wire Line
	2830 2940 2930 2940
Wire Wire Line
	2830 4450 2930 4450
Wire Wire Line
	4870 4460 4620 4460
Wire Wire Line
	4900 2940 4650 2940
Wire Wire Line
	5770 4460 5870 4460
Wire Wire Line
	5800 2940 5900 2940
Wire Wire Line
	9110 4870 9210 4870
Wire Wire Line
	9110 5270 9310 5270
Wire Wire Line
	9210 4870 9210 4770
Wire Wire Line
	9210 5170 9110 5170
Wire Wire Line
	9210 5170 9210 5520
Wire Wire Line
	9310 4970 9110 4970
Wire Wire Line
	9310 5070 9110 5070
Wire Wire Line
	9310 5370 9110 5370
Text Notes 4020 1710 2    118  ~ 24
Inputs
Text Notes 9210 4520 2    118  ~ 24
SWD
Text GLabel 1630 5880 0    50   Input ~ 0
HVD
Text GLabel 1680 2940 0    50   Input ~ 0
TSMS
Text GLabel 1680 4450 0    50   Input ~ 0
AMS
Text GLabel 2880 5880 2    50   Input ~ 0
N_HVD
Text GLabel 2930 2940 2    50   Input ~ 0
N_TSMS
Text GLabel 2930 4450 2    50   Input ~ 0
N_AMS
Text GLabel 4620 4460 0    50   Input ~ 0
SPARE
Text GLabel 4650 2940 0    50   Input ~ 0
TD
Text GLabel 5870 4460 2    50   Input ~ 0
N_SPARE
Text GLabel 5900 2940 2    50   Input ~ 0
N_TD
Text GLabel 9310 4970 2    50   Input ~ 0
SWCLK
Text GLabel 9310 5070 2    50   Input ~ 0
SWDIO
Text GLabel 9310 5270 2    50   Input ~ 0
N_RST
$Comp
L power:+3V3 #PWR025
U 1 1 00000000
P 9210 4770
F 0 "#PWR025" H 9210 4620 50  0001 C CNN
F 1 "+3V3" H 9225 4943 50  0000 C CNN
F 2 "" H 9210 4770 50  0001 C CNN
F 3 "" H 9210 4770 50  0001 C CNN
	1    9210 4770
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR026
U 1 1 00000000
P 9210 5520
F 0 "#PWR026" H 9210 5270 50  0001 C CNN
F 1 "GND" H 9215 5347 50  0000 C CNN
F 2 "" H 9210 5520 50  0001 C CNN
F 3 "" H 9210 5520 50  0001 C CNN
	1    9210 5520
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 00000000
P 8910 5070
F 0 "J1" H 8760 5070 50  0000 C CNN
F 1 "SWD" V 8860 5020 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 8910 5070 50  0001 C CNN
F 3 "~" H 8910 5070 50  0001 C CNN
	1    8910 5070
	1    0    0    -1  
$EndComp
$Sheet
S 1930 4350 900  200 
U 00000000
F0 "AMS" 50
F1 "Optocoupler.sch" 50
F2 "IN" I L 1930 4450 50 
F3 "~{OUT}" O R 2830 4450 50 
$EndSheet
$Sheet
S 1880 5780 900  200 
U 00000000
F0 "HVD" 50
F1 "Optocoupler.sch" 50
F2 "IN" I L 1880 5880 50 
F3 "~{OUT}" O R 2780 5880 50 
$EndSheet
$Sheet
S 4870 4360 900  200 
U 00000000
F0 "Safety" 50
F1 "Optocoupler.sch" 50
F2 "IN" I L 4870 4460 50 
F3 "~{OUT}" O R 5770 4460 50 
$EndSheet
$Sheet
S 1930 2840 900  200 
U 00000000
F0 "TSMS Control" 50
F1 "Optocoupler.sch" 50
F2 "IN" I L 1930 2940 50 
F3 "~{OUT}" O R 2830 2940 50 
$EndSheet
$Sheet
S 4900 2840 900  200 
U 00000000
F0 "Termic discharge" 50
F1 "Optocoupler.sch" 50
F2 "IN" I L 4900 2940 50 
F3 "~{OUT}" O R 5800 2940 50 
$EndSheet
$EndSCHEMATC
