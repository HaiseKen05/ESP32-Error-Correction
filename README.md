# ESP32-Error-Correction
This project deals with Error Detection and Error Correction without the use of any error error correction logic such as: Hamming Code, Parity Checks, or CRC (Cyclic Redundancy Check). This is a fun project developed by a single developer who carries the weight of 9 dead-weight group members and with the help of ChatGPT. Yes you heard that right ChatGPT helped me write 99% of the code as I'm learning how to code microcontrollers.

# Process Flow
This is the workflow of the project. Starting with the Sender.

Sender
- It uses the Serial Monitor to send the message to the Receiver.
- It first sends the message as a string made by the user to the Receiver.
- After sending the initial message, it begins the process of converting each of the letters in the message to binary.
- It process the 8 bits, bit by bit. To allow the user to manually corrupt the bits in transmission.
- After sending the last 8 bits of the message it will be sent to the Receiver.

Receiver
- It receives the initial message sent by the Sender.
- it stores the initial message to be used later on.
- The Receiver then waits for the 8 bits to be sent and perform a conversion from binary to ASCII Characters (Mainly the A-Z, a-z, 1-0, and special characters such as !)
- Once the last 8 bits have been received. The Receiver will then reconstruct the message.
- After reconstructing the message, it will automatically perform a comparison between the initial message and the reconstructed message.
- If it detects that there are differences in the comparison, it will automatically begin the process of correcting the message. Once the process of correcting the message is complete, it will now output the corrected message.
- If it does not detect any differences in the comparison, it will display in the Serial Monitor that there was no differences whatsoever and proceeds to output the reconstructed message.
