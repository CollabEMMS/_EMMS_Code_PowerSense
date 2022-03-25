# EMMS_Code_PowerSense
MPLAB X Project
- Chip - PIC18F25K22
- Compiler - XC8 1.45

# ToDo
- nothing at this time

# Changelog
### NEXT
- Parameter length increased to account for NULL_CHAR
- main loop oneshots might not run as expected
  - if a previous oneshot lined up with a subsequent one it might prevent the subsequent one from firing if the runtime took too long
  - change to use explicit time to run so the function is never skipped

### v2.1.2
March 20, 2022<br />
Tom Austin
#### Changes
- Cleanup - removed some items no longer used
  - PSVersion


### v2.1.1
March 19, 2022<br />
Tom Austin
#### Changes
- New versioning and changelog scheme
## 
### Pre v2.1.1
#### Previous changes are not currently listed