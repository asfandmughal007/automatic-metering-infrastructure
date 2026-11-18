# Known issues in the original FYP firmware

The code is kept as submitted in 2016. These defects were found by running it against a simulated
50 Hz mains signal (`tests/host/`) and by code inspection. The tests mark the first four as
`XFAIL`: they flip to a failure once the bug is fixed, so the test must then be updated.

| # | File | Defect | Evidence |
|---|------|--------|----------|
| 1 | `prototype_meter.ino` `max_of_voltage()` | `x*(357/1023)` is integer division = 0, so voltage always reads 0 (cast to `float`). | tested (XFAIL) |
| 2 | `prototype_meter.ino` `loop()` | `float power = power + ...` declares a new variable inside the `while`, so the outer `power` stays 0 and `kwh` is always 0. | tested (XFAIL) |
| 3 | `ami_meter.ino` `power_fact_ahmed()` | `cos(2*3.14*f*b)` uses `b` in milliseconds as if seconds; also `volt_cal()`/`curr_cal()` are truncated to `int`. For a 30 deg lag it returned 0.137 (expected 0.866); for 0 and 60 deg it returned 1.0. | tested (XFAIL) |
| 4 | `ami_meter.ino` `encode_dec()` | Returns a pointer to a local array (undefined behaviour), and uses `x / 255` for the high byte while `decode_dec()` uses 256: 230.00 V (23000) decodes as 23256. | inspection + arithmetic |
| 5 | `ami_meter.ino` `rms_volt()` | Takes the peak of the raw ADC value without removing the DC bias, so it is only right if the front end is unipolar. | inspection |
| 6 | `ami_meter.ino` `kwh_cal()` | Stores kWh in one EEPROM byte as an integer, so it wraps at 256 kWh and loses fractions. Never restored at boot. | inspection |
| 7 | both | Blocking 1 s / 10 000-sample loops; no filtering, which the original `problems.txt` also notes (current spikes). | inspection |
| 8 | `ami_meter.ino` | `receive_xbee()`/`process_data()` decode a frame but discard the values. | inspection |

Not available in this repository: the C# Control Unit (GUI) source. Only screenshots survive in `docs/images/gui/`.
