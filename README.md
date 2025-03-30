# RobotSync_via_RF

Diploma thesis - 2016

Short Description:

Four µCs of type Atmega32u4. Two µCs for one roboter respectively. One µC does control two DC-motors, while the other does the communication over RF.

Used RF-Device: NRF024L01+ - Nordic Semiconductors (Breakout from Sparkfun)
Used Port Expander: MCP23017

Brief indroduction to program flow:
1) µC1 sends information via I2C on PE1
2) µC2 reads the date just written and sends it via SPI on one RF-chip
3) µC3 gets an interrupt from another RF-chip and thus fetches the data and writes it onto PE2
4) µC4 gets hold of the data and controls the DC Motors accordingly to it

Notes about functions of the RF-chip:
Bidirectional automatic packet handling up to 2Mbit/s



