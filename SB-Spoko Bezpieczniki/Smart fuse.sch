EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 12 20
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 7400 7500 0    50   ~ 0
Bezpiecznik
Text Label 4750 3300 0    50   ~ 0
VOUT_0
Text Label 4750 3600 0    50   ~ 0
VOUT_1
Text Label 4750 4200 0    50   ~ 0
VOUT_3
Text Label 4750 4500 0    50   ~ 0
VOUT_4
Text Label 4750 4800 0    50   ~ 0
VOUT_5
Text HLabel 1400 3300 0    50   Output ~ 0
VOUT_0
Text HLabel 1400 3400 0    50   Output ~ 0
VOUT_1
Text HLabel 1400 3500 0    50   Output ~ 0
VOUT_2
Text HLabel 1400 3600 0    50   Output ~ 0
VOUT_3
Text HLabel 1400 3700 0    50   Output ~ 0
VOUT_4
Text HLabel 1400 3800 0    50   Output ~ 0
VOUT_5
Text HLabel 1400 4000 0    50   BiDi ~ 0
SCK
Text HLabel 1400 4100 0    50   Input ~ 0
SDI
Text HLabel 1400 4200 0    50   Output ~ 0
SDO
Text HLabel 1400 4300 0    50   Input ~ 0
CS
Wire Wire Line
	6300 2700 6400 2700
$Comp
L Device:C C1
U 1 1 6197C194
P 6150 2700
AR Path="/6196D4C3/6197C194" Ref="C1"  Part="1" 
AR Path="/619937A9/6197C194" Ref="C?"  Part="1" 
AR Path="/61993E75/6197C194" Ref="C?"  Part="1" 
AR Path="/6198E0B4/6197C194" Ref="C?"  Part="1" 
AR Path="/6198E79F/6197C194" Ref="C?"  Part="1" 
AR Path="/619FB125/6197C194" Ref="C?"  Part="1" 
AR Path="/619FBE4E/6197C194" Ref="C?"  Part="1" 
AR Path="/619FCD59/6197C194" Ref="C?"  Part="1" 
AR Path="/61A7011D/6197C194" Ref="C30"  Part="1" 
AR Path="/61A74311/6197C194" Ref="C31"  Part="1" 
AR Path="/61DC1767/6197C194" Ref="C32"  Part="1" 
F 0 "C32" V 5898 2700 50  0000 C CNN
F 1 "100nF" V 5989 2700 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6188 2550 50  0001 C CNN
F 3 "~" H 6150 2700 50  0001 C CNN
	1    6150 2700
	0    1    1    0   
$EndComp
Wire Wire Line
	6200 3300 6400 3300
Wire Wire Line
	6200 3400 6400 3400
Wire Wire Line
	6200 3500 6400 3500
Wire Wire Line
	6200 3600 6400 3600
Text Label 1750 3300 2    50   ~ 0
VOUT_0
Text Label 1750 3400 2    50   ~ 0
VOUT_1
Text Label 1750 3500 2    50   ~ 0
VOUT_2
Text Label 1750 3600 2    50   ~ 0
VOUT_3
Text Label 1750 3700 2    50   ~ 0
VOUT_4
Text Label 1750 3800 2    50   ~ 0
VOUT_5
Text Label 1750 4000 2    50   ~ 0
SCK
Text Label 1750 4100 2    50   ~ 0
SDI
Text Label 1750 4200 2    50   ~ 0
SDO
Text Label 1750 4300 2    50   ~ 0
CS
Text HLabel 1400 3200 0    50   Input ~ 0
VIN
Text Label 1750 3200 2    50   ~ 0
VIN
Text Label 5600 2750 3    50   ~ 0
VIN
Wire Wire Line
	5100 3300 5100 3400
Wire Wire Line
	5100 3400 5200 3400
Connection ~ 5100 3300
Wire Wire Line
	5100 3300 5200 3300
Wire Wire Line
	5200 3700 5100 3700
Wire Wire Line
	5100 3700 5100 3600
Wire Wire Line
	5100 3600 5200 3600
Wire Wire Line
	5200 4000 5100 4000
Wire Wire Line
	5100 4000 5100 3900
Wire Wire Line
	5100 3900 5200 3900
Wire Wire Line
	5200 4300 5100 4300
Wire Wire Line
	5100 4300 5100 4200
Wire Wire Line
	5100 4200 5200 4200
Wire Wire Line
	5200 4600 5100 4600
Wire Wire Line
	5100 4600 5100 4500
Wire Wire Line
	5100 4500 5200 4500
Wire Wire Line
	5200 4900 5100 4900
Wire Wire Line
	5100 4900 5100 4800
Wire Wire Line
	5100 4800 5200 4800
Connection ~ 5100 3600
Connection ~ 5100 3900
Connection ~ 5100 4200
Connection ~ 5100 4500
Connection ~ 5100 4800
Wire Wire Line
	5700 5400 5700 5300
$Comp
L power:GND #PWR063
U 1 1 61970F0F
P 5700 5400
AR Path="/6196D4C3/61970F0F" Ref="#PWR063"  Part="1" 
AR Path="/619937A9/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/61993E75/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/6198E0B4/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/6198E79F/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/619FB125/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/619FBE4E/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/619FCD59/61970F0F" Ref="#PWR?"  Part="1" 
AR Path="/61A7011D/61970F0F" Ref="#PWR066"  Part="1" 
AR Path="/61A74311/61970F0F" Ref="#PWR069"  Part="1" 
AR Path="/61DC1767/61970F0F" Ref="#PWR072"  Part="1" 
F 0 "#PWR072" H 5700 5150 50  0001 C CNN
F 1 "GND" H 5705 5227 50  0000 C CNN
F 2 "" H 5700 5400 50  0001 C CNN
F 3 "" H 5700 5400 50  0001 C CNN
	1    5700 5400
	1    0    0    -1  
$EndComp
Text Label 4750 3900 0    50   ~ 0
VOUT_2
Wire Wire Line
	4750 4800 5100 4800
Wire Wire Line
	4750 4500 5100 4500
Wire Wire Line
	4750 4200 5100 4200
Wire Wire Line
	4750 3900 5100 3900
Wire Wire Line
	4750 3600 5100 3600
Wire Wire Line
	4750 3300 5100 3300
Text Label 6400 3600 2    50   ~ 0
CS
Text Label 6400 3500 2    50   ~ 0
SDO
Text Label 6400 3400 2    50   ~ 0
SDI
Text Label 6400 3300 2    50   ~ 0
SCK
Wire Wire Line
	1400 4000 1750 4000
Wire Wire Line
	1400 4100 1750 4100
Wire Wire Line
	1400 4200 1750 4200
Wire Wire Line
	1400 4300 1750 4300
Wire Wire Line
	1400 3800 1750 3800
Wire Wire Line
	1400 3700 1750 3700
Wire Wire Line
	1400 3600 1750 3600
Wire Wire Line
	1400 3500 1750 3500
Wire Wire Line
	1400 3400 1750 3400
Wire Wire Line
	1400 3300 1750 3300
Wire Wire Line
	1400 3200 1750 3200
Wire Wire Line
	5600 2750 5600 2900
Wire Wire Line
	5800 2700 6000 2700
Wire Wire Line
	5800 2600 5800 2700
Connection ~ 5800 2700
Wire Wire Line
	5800 2700 5800 2900
Wire Wire Line
	6400 2700 6400 2800
$Comp
L power:GND #PWR065
U 1 1 61AABA98
P 6400 2800
AR Path="/6196D4C3/61AABA98" Ref="#PWR065"  Part="1" 
AR Path="/619937A9/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/61993E75/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/6198E0B4/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/6198E79F/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/619FB125/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/619FBE4E/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/619FCD59/61AABA98" Ref="#PWR?"  Part="1" 
AR Path="/61A7011D/61AABA98" Ref="#PWR068"  Part="1" 
AR Path="/61A74311/61AABA98" Ref="#PWR071"  Part="1" 
AR Path="/61DC1767/61AABA98" Ref="#PWR074"  Part="1" 
F 0 "#PWR074" H 6400 2550 50  0001 C CNN
F 1 "GND" H 6405 2627 50  0000 C CNN
F 2 "" H 6400 2800 50  0001 C CNN
F 3 "" H 6400 2800 50  0001 C CNN
	1    6400 2800
	1    0    0    -1  
$EndComp
$Comp
L VN9D30Q100F:VN9D30Q100F U?
U 1 1 61B9BFD9
P 5700 3000
AR Path="/619FB125/61B9BFD9" Ref="U?"  Part="1" 
AR Path="/619FBE4E/61B9BFD9" Ref="U?"  Part="1" 
AR Path="/6196D4C3/61B9BFD9" Ref="U12"  Part="1" 
AR Path="/61A7011D/61B9BFD9" Ref="U13"  Part="1" 
AR Path="/61A74311/61B9BFD9" Ref="U14"  Part="1" 
AR Path="/61DC1767/61B9BFD9" Ref="U15"  Part="1" 
F 0 "U15" H 5200 3200 50  0000 C CNN
F 1 "SMART FUSE" H 5200 3100 50  0000 C CNN
F 2 "VN9D30Q100F:QFN-32_EP_6x6_Pitch0.5mm" H 6100 3150 50  0001 C CNN
F 3 "https://www.st.com/resource/en/datasheet/vn9d30q100f.pdf" H 6100 3150 50  0001 C CNN
	1    5700 3000
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR064
U 1 1 622064ED
P 5800 2600
AR Path="/6196D4C3/622064ED" Ref="#PWR064"  Part="1" 
AR Path="/61A7011D/622064ED" Ref="#PWR067"  Part="1" 
AR Path="/61A74311/622064ED" Ref="#PWR070"  Part="1" 
AR Path="/61DC1767/622064ED" Ref="#PWR073"  Part="1" 
F 0 "#PWR073" H 5800 2450 50  0001 C CNN
F 1 "+3V3" H 5815 2773 50  0000 C CNN
F 2 "" H 5800 2600 50  0001 C CNN
F 3 "" H 5800 2600 50  0001 C CNN
	1    5800 2600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
