# SqueezeClock

Bedside clock that integrates with Logitech Media Server providing capacitve touch snooze button. PoC implemented on Particle Photon with Sparkfun battery shield, adafruit capacitve touch and adafruit I2C 7 Segment display.

https://store.particle.io/</br>
https://www.sparkfun.com/products/13626</br>
https://www.adafruit.com/product/879</br>
https://www.adafruit.com/products/1602</br>

Sublime build (/Users/sam/Library/Application Support/Sublime Text 2/Packages/User/particle.sublime-build):

{
	"cmd": ["/usr/local/bin/particle", "flash", "<photonid>","~/Squeezeclock"],
	"path": "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:~/photon/gcc-arm-none-eabi-4_9-2015q3/bin/"
}