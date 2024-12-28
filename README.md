# Esphome Radsens Component
## About this repo
This is a full esphome native component for the ClimateGuard Radsens.
This was motivated by the coming ["custom component-pocolypse"](https://esphome.io/guides/contributing#a-note-about-custom-components) and I still suffer from occasional
hangs in the original module (the esp remains pingable but nothing else responds).
External components are quite a bit better integrated with esphome and easier for users to use and allow other i2c
devices on the same esp32.  Esphome actually calls out Wire as a library which doesn't play well with others.

## What is a Radsens?
Radsens is a geiger counter built by ClimateGuard.  Their pages can be found [here](https://climateguard.info/radsens/).
I ordered mine before the war with Ukraine and I don't entirely understand the relation between countries
 but its worth noting it appears to be a Kazakhstan company.

* [tindie carries the RadSens](https://www.tindie.com/products/climateguard/dosimeter-with-i2c-radsens-2-arduino/)

## Setup
### Simple Example

``` yaml
external_components:
  - source: github://krbaker/esphome-radsens@v1.0.0
    components: [ radsens ]
    refresh: 600s

i2c:
  - id: bus_a
    frequency: 400kHz

radsens:

sensor:
  - platform: radsens
    dynamic_intensity:
      name: "Dynamic Intensity"
    static_intensity:
      name: "Static Intensity"
    counts_per_minute:
      name: "Counts Per Minute"
```


### Full Configuration
A full configuration can be found [here](geiger.yaml) but I recommend reading the following to undestand it.

| field | notes |
|-------|-------|
| external_components | This is not yet in the core esphome so you need to pull it in this way |
| external_components:source | This should be pointed to this github repo github://krbaker/esphome-radsens likely you want to take the current release found [here](https://github.com/krbaker/esphome-radsens/releases) using @\<tag\>
| external_components:source:components | There is one and only component here [ radsens ] |
| external_components:source:refresh | Probably don't need any more than 600s here |
| radsens | The radens component must be enabled to install the component |
| radsens:sensativity | This field is not required.  If it us unset or set to 0 the component will not set anything.  The default according to the docs is 105 and that is what mine was shipped with.  You probably do not need this |
| sensor:radsens | Needed to enable the sensors |
| sensor:radsens:dynamic_intensity | The 'dynamic_intensity' sensor which is an average intensity over a varying time window |
| sensor:radsens:static_intensity | The 'static_intensity' sensor which is an average intensity over the last 5min |
| sensor:radsens:counts_per_mintute | The number of detected impulses since the last update, scaled to a minute window |
| switch:radsens | Needed to enable the switches |
| switch:radsens:control_led | Controls if the blue led which flashes when an impulse is detected (default on) |
| switch:radsens:control_high_voltage | Controls if the high voltage generator is on.  When turning off it takes a while to change readings (~5min for me) but it will go to zero (default on) |
| switch:radsens:control_low_power | Sets a low power mode which turns off the tube power from time to time (default off) |

## Notes
  * I did see a failure to communicate with the device when setting the high voltage switch at startup but can't repro. 
  If you see this file a bug.
  * The endienness is different for some different i2c values, I think I got all of these right but dont be confused by that in code.
  This was a little surprising at first but I found it matched the arduino code.
  * Reading the dynamic and static intensity also reset the CPM counter.  Given I read them in the update loop I just read all three
  sequentialy with CPM first.  The library for arduino keeps a CPM counter and reads the CPM value before reading anything
  that seems to reset the CPM value.  There is a little bit of time that is probably lost but that is probably still true in
  the arduino code.
  * This is my first external component, sensors & switches in the same component took some time but I think are correct.
  if you see something wrong file an issue or a PR.
  * The switch restoration I have the least confidence in but it is working in my testing.
  * It might make sense to convert the exposure numbers from rostom to millisevert
  * The [english documents](https://github.com/climateguard/RadSens/blob/master/extras/datasheets/RadSens_datasheet_ENG.pdf) are not complete.  I used a translated Russian docs ([4v0](Radsens_datasheet_RU_1v3-EN.pdf) ,[1v3](Radsens_datasheet_RU_4v0-EN.pdf)) and the [arduino library](https://github.com/climateguard/RadSens/tree/master) as sources.


## Other RadSens software
* https://github.com/climateguard/RadSens/tree/master
* https://github.com/maaad/RadSens1v2