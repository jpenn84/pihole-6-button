
# pihole-6-button

An IOT appliance which controls the PiHole ad blocker using physical buttons.

## Background

I use PiHole at home to block ads and tracking. This works almost flawlessly for our internet needs, but a glaring exception has arisen. My kids like to play mobile games which give in-game rewards if they watch an ad.

Because I don't want to give them access to the PiHole admin interface, they have to ask me to disable blocking temporarily in order to watch those ads.

Due to work and other obligations, I'm not always available to drop what I'm doing and disable ad blocking temporarily, which had led to some frustration in the household.

This inspired me to create a device that sends HTTP requests to enable/disable blocking and displaying the status

## Features
- Blue LED indicates Blocking is on
- Red LED indicates blocking is off
- Flashing LED: Indicates transitioning to that status. 
  E.g., flashing blue indicates transitioning to blocking enabled
- Red button to disable blocking
- Blue button to enable blocking
- Tamper-resistant Reset button

## Operation

### Power-on order of operations:
1. Connect to WiFi
2. Get a Session ID
3. Get the current blocking status and display
4. Enable button input

### Loop order of operations
1. Check Session ID (`sid`) validity.
    1. Request a new `sid` if needed.
2. Check buttons
    1. Check for Enabled button press.
       If pressed:
        1. If Enabled, ignore
        2. If Disabled, begin the Enable procedure
    2. Else, check for Disabled button press.
       If pressed:
        1. If Disabled, ignore
        2. If Enabled, begin the Disable procedure
3. Else, query blocking status
    1. If enabled, turn on blue LED and turn off red LED
    2. if disabled, turn on red LED and turn off blue LED

### Enable/disable procedure
1. Send the request to enable/disable blocking for 5 minutes
    1. If this fails due to expired session, get a new Session ID and try again
2. Turn off the LED opposite of the request and flash the LED corresponding to the request.
3. Return to the loop to handle updated status

## Possible future enhancements
- CdS photocell to check ambient light levels and adjust brightness
    - LEDs would have to be on an Analog pin
    - Would need adjustments to determine best brightness
- Additional LEDs for WiFi status

## License

[GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

##
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-red.svg)](https://choosealicense.com/licenses/gpl-3.0/)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/jpenn84/pihole-6-button?label=Open%20issues)
![GitHub Release Date](https://img.shields.io/github/release-date/jpenn84/pihole-6-button?label=Release%20Date&color=Blue)
