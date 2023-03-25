# mpu - dmtk program manager

A custom taskbar.

## Compiling

With Xlib installed :
```
$ ./build.sh
```

## Usage

mpu needs a file containing a list of application in ~/.config/mpu. For example:
```
$ mkdir -p ~/.config/mpu
$ cp sample.tbl ~/.config/mpu/
```
Then, you can run mpu by using:
```
$ ./mpu sample.tbl
```
Using your DE config, you can bind the Super key to:
```
$ mpu -r
```
for showing/hiding mpu.
