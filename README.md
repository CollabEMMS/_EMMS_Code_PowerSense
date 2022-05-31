# EMMS_Code_PowerSense
MPLAB X Project
- Chip - PIC18F25K22
- Compiler - XC8 1.45

# ToDo
- nothing at this time

# Changelog
### NEXT
- none yet

### v3.2.1
May 31, 2022<br />
Tom Austin
TEST VERSION
#### Changes
- pulse watchdog timer to reset PIC
  - RESET

### v3.2.0
May 14, 2022<br />
Tom Austin
#### Release

### v3.1.7
May 14, 2022<br />
Tom Austin
#### Changes
- update power and energy calculations
- fix some str to ul conversions for energy used
  - some were using atol which could lead to problems

### v3.1.6
April 16, 2022<br />
Tom Austin
#### Changes
- Calibration Factors now stored in Command Board
  - storing the calibration factor in the Power Sense made it difficult to read it back out
- Code cleanup - removed high frequency ModInfo data set


### v3.1.5
April 5, 2022<br />
Tom Austin
#### Changes
- Pulse time variables, HF and LF, are now static (retain values)
  - by not retaining values they would revert back to 0 and cause choosing between HF and LF to mess up.
- Fixed typo where calibration 2 would write to calibration 1 eeprom slot
- changed strtoul to explicitly use base 10 instead of self determination (0)
  - made no difference in program operation, but ensured interpretation of the parameter as base 10 during debug session

### v3.1.4
March 29, 2022<br />
Tom Austin
#### Changes
- Power reduction (when no power is used) was not working
  - fixed
  - added LED 3 flash to show that a pulse has not been received and power reduction is active

### v3.1.3
March 26, 2022<br />
Tom Austin
#### Changes
- Parameter length increased to account for NULL_CHAR
- main loop oneshots might not run as expected
  - if a previous oneshot lined up with a subsequent one it might prevent the subsequent one from firing if the runtime took too long
  - change to use explicit time to run so the function is never skipped
- lots of compile warnings "pointer to const unsigned char -> pointer to unsigned char"
  - updated functions to use "const char*" instead of just "char*"
  - char* can be changed to const char* but not vice versa
  - the program ran fine with the issue, but this makes the compile cleaner and makes sure everything works
- added calibration_1 and calibration_2
  - these are used for the HF and LF pulse outputs for now
  - they can be redesignated as necessary
  - shown in the ModInfo
  - stored in local eeprom (yay)
- tweaked the Watts calculations
- tweaked how to choose between LF and HF watts
- cleaned out lots of commented obsolete old code
- test pulse code exists but is commented for normal use


### v3.1.2
March 20, 2022<br />
Tom Austin
#### Changes
- Cleanup - removed some items no longer used
  - PSVersion


### v3.1.1
March 19, 2022<br />
Tom Austin
#### Changes
- New versioning and changelog scheme
## 
### Pre v3.1.1
#### Previous changes are not currently listed
