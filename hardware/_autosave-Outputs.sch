EESchema Schematic File Version 5
EELAYER 36 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 18 24
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
Wire Wire Line
	1960 3100 2060 3100
Wire Wire Line
	1960 3200 2060 3200
Wire Wire Line
	1990 4230 2090 4230
Wire Wire Line
	1990 4330 2090 4330
Wire Wire Line
	1990 5530 2090 5530
Wire Wire Line
	1990 5630 2090 5630
Wire Wire Line
	4790 4220 4890 4220
Wire Wire Line
	4790 4320 4890 4320
Wire Wire Line
	4810 3090 4910 3090
Wire Wire Line
	4810 3190 4910 3190
Wire Wire Line
	8330 2970 8230 2970
Wire Wire Line
	8330 3070 8230 3070
Wire Wire Line
	8410 4240 8260 4240
Wire Wire Line
	8410 4340 8260 4340
Wire Wire Line
	8420 5530 8270 5530
Wire Wire Line
	8420 5630 8270 5630
Wire Wire Line
	9730 3020 9630 3020
Wire Wire Line
	9760 4290 9660 4290
Wire Wire Line
	9770 5580 9670 5580
Text Notes 3960 1790 2    118  ~ 24
Outputs
Text Notes 8190 1760 0    118  ~ 24
Outputs Control
Text GLabel 1960 3100 0    50   Input ~ 0
BREAK_LIGHT_IN
Text GLabel 1960 3200 0    50   Input ~ 0
BREAK_LIGHT_GATE
Text GLabel 1990 4230 0    50   Input ~ 0
LED_1_IN
Text GLabel 1990 4330 0    50   Input ~ 0
LED_1_CRL_OUT
Text GLabel 1990 5530 0    50   Input ~ 0
LED_2_IN
Text GLabel 1990 5630 0    50   Input ~ 0
LED_2_CRL_OUT
Text GLabel 4790 4220 0    50   Input ~ 0
BUZZER_IN
Text GLabel 4790 4320 0    50   Input ~ 0
BUZZER_CRL_OUT
Text GLabel 4810 3090 0    50   Input ~ 0
RTDS_IN
Text GLabel 4810 3190 0    50   Input ~ 0
RTDS_GATE
Text GLabel 8230 2970 0    50   Input ~ 0
BUZZER_CONTROL
Text GLabel 8230 3070 0    50   Input ~ 0
EN_MOSFET
Text GLabel 8260 4240 0    50   Input ~ 0
LED_1_CONTROL
Text GLabel 8260 4340 0    50   Input ~ 0
EN_MOSFET
Text GLabel 8270 5530 0    50   Input ~ 0
LED_2_CONTROL
Text GLabel 8270 5630 0    50   Input ~ 0
EN_MOSFET
Text GLabel 9730 3020 2    50   Input ~ 0
BUZZER_CRL_OUT
Text GLabel 9760 4290 2    50   Input ~ 0
LED_1_CRL_OUT
Text GLabel 9770 5580 2    50   Input ~ 0
LED_2_CRL_OUT
$Sheet
S 2060 3000 1100 300 
U 00000000
F0 "BREAK_LIGHT mosfet" 50
F1 "N_MOSFET.sch" 50
F2 "IN" I L 2060 3100 50 
F3 "CRL" I L 2060 3200 50 
$EndSheet
$Sheet
S 8330 2870 1300 300 
U 00000000
F0 "BUZZER_CONTROL_OUTPUT" 50
F1 "Digital_Output.sch" 50
F2 "input" I L 8330 2970 50 
F3 "output" O R 9630 3020 50 
F4 "~{enable}" I L 8330 3070 50 
$EndSheet
$Sheet
S 4890 4120 1100 300 
U 00000000
F0 "Buzzer mosfet" 50
F1 "N_MOSFET.sch" 50
F2 "IN" I L 4890 4220 50 
F3 "CRL" I L 4890 4320 50 
$EndSheet
$Sheet
S 2090 4130 1100 300 
U 00000000
F0 "LED_1 mosfet" 50
F1 "N_MOSFET.sch" 50
F2 "IN" I L 2090 4230 50 
F3 "CRL" I L 2090 4330 50 
$EndSheet
$Sheet
S 8410 4140 1250 300 
U 00000000
F0 "LED_1_CONTROL_OUTPUT" 50
F1 "Digital_Output.sch" 50
F2 "input" I L 8410 4240 50 
F3 "output" O R 9660 4290 50 
F4 "~{enable}" I L 8410 4340 50 
$EndSheet
$Sheet
S 2090 5430 1100 300 
U 00000000
F0 "LED_2 mosfet" 50
F1 "N_MOSFET.sch" 50
F2 "IN" I L 2090 5530 50 
F3 "CRL" I L 2090 5630 50 
$EndSheet
$Sheet
S 8420 5430 1250 300 
U 00000000
F0 "LED_2_CONTROL_OUTPUT" 50
F1 "Digital_Output.sch" 50
F2 "input" I L 8420 5530 50 
F3 "output" O R 9670 5580 50 
F4 "~{enable}" I L 8420 5630 50 
$EndSheet
$Sheet
S 4910 2990 1100 300 
U 00000000
F0 "RTDS mosfet" 50
F1 "N_MOSFET.sch" 50
F2 "IN" I L 4910 3090 50 
F3 "CRL" I L 4910 3190 50 
$EndSheet
$EndSCHEMATC
