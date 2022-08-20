# BrownNoise

> An app for desigining resistor network impedance in op-amp circuits based on desired noise floor

[![Release version](https://img.shields.io/github/v/release/makuke1234/BrownNoise?display_name=release&include_prereleases)](https://github.com/makuke1234/BrownNoise/releases/latest)
[![Total downloads](https://img.shields.io/github/downloads/makuke1234/BrownNoise/total)](https://github.com/makuke1234/BrownNoise/releases)
![C version](https://img.shields.io/badge/version-C99-blue.svg)
[![wakatime](https://wakatime.com/badge/github/makuke1234/BrownNoise.svg)](https://wakatime.com/badge/github/makuke1234/BrownNoise)
![SLOC count](https://img.shields.io/badge/SLOC-1000-blue.svg)

**Please note that the application is currently under heavy development.**

As the name suggests, there's something up with Brownian motion. The original objective of this application is to calculate thermal noise, but since thermal noise is itself inherently random it suggest Brownian motion. Thus the name BrownNoise was born.

This application allows users to enter in desired operating temperature and op-amp characteristics (especially noise voltage) of the circuit in mind. Also the desired noise floor voltage should be entered. Then the application calculates the optimal input impedance for the op amp. The program also supports capacitive loading calculations helping to choose the optimal capacitor for an integrator or differentiator type op-amp circuit.

### Normal feedback mode
![Normal feedback](./img/normal.png)

### Optimal impedance calculations based on SNR
![Integrator](./img/snrimpedance.png)


# Obtaining

64-bit (x64) Windows binaries can be obtained [here](https://github.com/makuke1234/BrownNoise/releases).


# Changelog

* 0.6.2
	* [x] Color-coding to show which values affect which results
* 0.6.1
	* [x] Calculations as flexible as possible
* 0.6
	* [x] SNR calculations based on noise floor and input level
	* [x] Optimal impedance calculations based on SNR and input level
* 0.5
	* [x] Add decibel support as the noise voltage
* 0.4
	* [x] Tweak UI element spacing
* 0.3
	* [x] Fix StretchBlt horrible quality -> images now clear
	* [x] RMS-noise as default unit
	* [x] Better reset function
* 0.2
	* [x] Compressed the images -> replaced BMP with PNG
* 0.1
	* [x] Initial working beta release


# License

This project uses the MIT license.


# Credits

Resistor icon:<br>
<a href="https://www.flaticon.com/free-icons/resistor" title="resistor icons">Resistor icons created by Freepik - Flaticon</a>
