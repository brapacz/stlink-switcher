# Stlink switcher

This project exist to utilize a single st-link programmer, tiny STM32F042F6P6 and USB to serial adapter to program multiple MCU with single `make flash` command, instead of switching cables between breadboards back and forth. SWDIO and GND pins are connected to every single MCU in form of a daisy chain, while SWDCLK is connected to relays, which are conecting it to choosen MCU. To choose selected MCU you just have to send number between 0 and 3 to serial port.

**In it's essence, it's a relay board with port0 as default controller by serial port.**

I used simple relays controller by transistors and tiny MCU because I had it on hand, but if I had to do it again, a would rather take 4 relays module and simplest Nucleo or even an Arduino board. Plus of the STM32F042F6P6 is that it could be used with USB wired directly to the MCU and could save me using an USB to serial adapter.

**Important:** Relay 0 is default, and it must be connected to normally open side, while other must be normally close. I used it to program st-link switcher while is connected to st-link via Relay 0.

## Build and upload

connect st-link to STM32F042F6P6, then execute ```make flash reset``` and it should work.

TODO: add connection diagram.

## Example usage

```bash
> ls /dev/serial/by-path/*
/dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.3:2.0-port0

# connect serial-port adapter

> ls /dev/serial/by-path/*
/dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.2.4.2:1.0-port0  /dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.3:2.0-port0

# /dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.2.4.2:1.0-port0 is a new device

# set baudrate to 115200, just once
stty -F /dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.2.4.2:1.0-port0 115200

# Change device. sleep 0.2 may require some tweaking
echo 1 > /dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.2.4.2:1.0-port0
```

Do not switch to relay that don't have connected MCU, st-link may need a power off and on after that to work correctly again.
Here is a loop example:

```bash
for ID in $(seq 0 3); do
  echo $ID > /dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4.2.4.2:1.0-port0
  echo "Using port $i"
  sleep 0.2 # sometimes st-link doesn't recognize MCU right away, tiny delay fixes that
  st-info --probe
  echo
done
```

output:

```bash
Using port 0
Found 1 stlink programmers
  version:    V2J35S7
  serial:     382719002B135937334D4E00
  flash:      32768 (pagesize: 1024)
  sram:       6144
  chipid:     0x0445
  descr:      F04x

Using port 1
Found 1 stlink programmers
  version:    V2J35S7
  serial:     382719002B135937334D4E00
  flash:      32768 (pagesize: 1024)
  sram:       10240
  chipid:     0x0412
  descr:      F1 Low-density device

Using port 2
Found 1 stlink programmers
  version:    V2J35S7
  serial:     382719002B135937334D4E00
  flash:      32768 (pagesize: 1024)
  sram:       10240
  chipid:     0x0412
  descr:      F1 Low-density device

Using port 3
Found 1 stlink programmers
  version:    V2J35S7
  serial:     382719002B135937334D4E00
  flash:      32768 (pagesize: 1024)
  sram:       10240
  chipid:     0x0412
  descr:      F1 Low-density device
```

Of course `st-info` can be replaced with `st-flash write build/my-app-$ID.bin 0x08000000` or something similar, to write multiple firmwares to multiple MCUs at the same time :-)

TODO:

- [ ] add photo
- [ ] add connection diagram
- [ ] add link to repository that utilizes this
