# RoombaIR 1.0.1

Simple solution to send IR remote commands to a Roomba using a [Nanode V5](https://wiki.london.hackspace.org.uk/view/Project:Nanode) via a basic web page.

Tested and in "production" use since 2014 with an [iRobot Roomba 620](http://www.irobot.com/). Should work with all 500/600/700 series Roomba devices.

You need to connect an infrared LED with the correct limiting resistor to Digital Pin 3 and point it towards the IR receiver on your Roomba. I've glued it to the top of the charging station and added a reflector. Works a charm since 4 years.

- Uses **send_roomba** function from [Super Simple Arduino Powered Roomba Scheduler](http://www.instructables.com/id/Super-Simple-Arduino-Powered-Roomba-Scheduler/step3/Arduino-Script/)
- HTTP "server" based on BackSoon example from the [Ethercard](https://github.com/jcw/ethercard) library 
- Infrared library [IRremote](https://github.com/shirriff/Arduino-IRremote)

**License:**

The MIT License (MIT)

Copyright (c) 2014 Jan Wildeboer and others

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
