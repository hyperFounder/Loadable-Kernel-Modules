## Device Drivers

A Loadable kernel module is called a driver. It's kind of like an API that allows the communication between user space programs and the kernel.

- Device drivers are kernel modules (i.e: software code). They act as an interface between hardware devices and the rest of the operating system, enabling communication between OS and hardware.
- - In other words, the device driver provides the communication on behalf of a user program.
  - ```/dev``` directory, allow user space to communicate with kernel space through read and write operations, much like regular file ```I/O```.

### Character Devices: Transfer data one char at a time without buffering.

- Devices are divided into two types: character devices and block devices.
- Block devices have a buffer for requests, so they can choose the best order in which to respond to the requests.
- - They also transfer data in fixed-size blocks (e.g., sectors)

---
- ```/dev``` directory: contains device files which provides the means to communicate with the hardware.
- Example: The ```es1370.ko``` sound card device driver might connect the ```/dev/sound device file``` to the ```Ensoniq IS1370``` sound card.
- - So, a userspace program like mp3blaster can use ```/dev/sound``` without ever knowing what kind of sound card is installed.

### Distinguishing between character and block devices
- Let’s look at some device files. ```c``` indicates that this is a **character** device file.
- - A ```b``` would indicate a **block** device file.
```
ryan@ryan-VirtualBox:~/Desktop/shared$ ls -l /dev/zero
crw-rw-rw- 1 root root 1, 5 Apr  5 01:33 /dev/zero
ryan@ryan-VirtualBox:~/Desktop/shared$ 
```
