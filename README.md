# CoolCar
This is the CoolCar project

# Phase 1
Hardware:
- Quicrun 1060 brushed ESC. 
  - PWM frequency 50 Hz (200ms)
  - It seems it wants 13 ms for neutral position. I tested with 10 as minimum value (maximum speed backward) and 20 as maximum (maximum speed forward) 
  - Pinout: 
    - blue and yellow cables to the motor.
    - black and red connected to the power supply (battery). Be careful in not inverting them (black is the ground). The 
      power supply should generate 7.2V and the current absorbed by the motor can also reach 2.5-3A.
    - The three small cables are connected to the driver. Black one is the ground, red one has to be connected to 5V and the white one
      to the PWM generator.
 
- Servo motor to drive the car direction.
  - PWM frequency 50 Hz (200ms)
  - It should work with 15 ms as central position, 20 and 10 ms for extremals. I tested it and it works but needs calibration yet (TODO).
  
- Rotative potentiometer.
  - Watching it with pins in front of you:
    - Right pin on 3.3 V
    - Middle pin is the output (to the ADC)
    - Left pin on ground.

- STM32F4-discovery
  - TIM6: it drives the HighResolutionTimer and can be used as a synchronisation point for the execution cycle.
  - TIM4: drives the PWM generator. Now only the channel 1 (pin D12 connected also to green led) is activated.
  - ADC1: reads the speed reference. Now only the channel 8 is activated (pin B0). It acquires data transferring data using the DMA,
    but now after each conversion i put a sleep when i should poll on a flag asserting that the DMA has finished its work (TODO).
  - UART4: pins A0=Tx and A1=Rx (maybe is better another UART because the user button is connected to A0 and we want to use it (TODO)).
    It is used as the error stream flushing error messages.
  - USB-OTG-FS: uses the pins A11 and A12. This stream is used to get the RT diagnostic (much faster than UART).
 
Software:
   

