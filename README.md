# Can Receiver (testing helper)

Firmware to test can functionality of boards out of system.

It receives all messages on the can bus, ACKs them, and output the message
through a serial connection (in the usb port) to a pc.

## Usage

Upload to a samc21 dev board, and connect the can pins to device under test.  
Connect debugger usb port to a pc, and open a serial console.
