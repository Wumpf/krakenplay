Krakenplay
==========

Motivation
========

Ever tried to connect more than 4 XBox controller? Or more than one keyboard? Or n mouses?
Most likely not. Even if enough ports are available, OSes usally supports only a limited number of devices.

Here comes the solution: Connect input devices from other computers via network!

Krakenplay is a framework on the base of OIS that allows you to connect an arbitrary number of input devices via network. Clients for Unix/Mac/Windows will feed data to an platform independent API which you can easily integrate into your application

Features
========
* Cross-platform input library with buildt-in network support
  * arbitrary amount of devices connected via local network!
* Network
  * UDP based, stable even with high packet loss
  * automatic connection establishment to an arbitrary amount of clients
* Different types of devices
  * Mouse, Gamepad, Keyboard

Usage
========
*a short explanation how to use the api in your own code will follow*

*DO NOT USE KRAKENPLAY-CLIENTS IN PUBLIC NETWORKS* - an attacker may intercept its packages to log your inputs.

Tested Systems & Compilers
========
Since there are no automatic build tests (yet?), platform/compiler specific errors may occur from time to time.
Bugfixes are very appreciated!
* Visual Studio 2013 x64/x86 (vs120)
* gcc 4.8 x64/x86

Missing Features / TODOs
========
In no specific order!

* More documentation on github frontpage
* Platform & compiler support
  * Mac/Clang support
* Language ports
  * C Interface
  * Other language ports? (C# probably)
* Standalone "server" without clients
* Replace OIS with own input abstraction implementations?
* Android client for touch input?
