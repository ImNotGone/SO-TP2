SO-TP2
======

Using [x64BareBones][x64barebones] as our base this project we will be building a kernel with `Memory Management`, `Processes`, `Sheduling`, `IPC` & `Synchronization` 

Table of Contents
-----------------

 * [Requirements](#requirements)
 * [Build](#build)
 * [Usage](#usage)

Requirements
------------

This project requires the following to be built & run
 * [Docker][docker]
 * [Agodio-Image][agodio] or [ImNotGone-Image][imnotgone] (based on agodio's image)
 * [QEMU][qemu]

Additional information on how to install docker & agodio's image [here][aquili-docker], to use ImNotGone's image just replace `agodio/itba-so:1.0` with `imnotgone/itba-so` everywhere you see it

To start the container and execute code inside it, run the following command:
```sh
docker run -v "${PWD}:/root" --privileged --rm -ti $SO_IMAGE
```
This will let you run commands on your image of choice nedded for this project to be built & run.

Where the /root folder will attach to your current directory & `$SO_IMAGE` is your image of choice (`agodio/itba-so:1.0` or `imnotgone/itba-so`).

(make sure to be in /root when trying to build using make)

Build
-----
To build you must be inside the docker image in the projects directoy.

First step is to build the Toolchain
```sh
cd Toolchain
make all
cd ..
```

Then you can build the kernel
```sh
make all
```

Usage
-----
outside the container, run: 
```sh
./run.sh
```


[x64barebones]: https://bitbucket.org/RowDaBoat/x64barebones/wiki/Home
[docker]: https://www.docker.com/
[agodio]: https://hub.docker.com/r/agodio/itba-so/
[imnotgone]: https://hub.docker.com/r/imnotgone/itba-so/
[qemu]: https://www.qemu.org/
[aquili-docker]: https://github.com/alejoaquili/ITBA-72.11-SO/blob/main/docker/README.md/