#!/bin/bash
name=TP2SO
# abro docker y ejecuto los make necesarios
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name $name $SO_IMAGE
docker exec -it $name make clean        --directory=/root/Toolchain
docker exec -it $name make all          --directory=/root/Toolchain
docker exec -it $name make clean        --directory=/root
if [ "$1" == "gdb" ]
then
    docker exec -it $name make gdb      --directory=/root
else
    docker exec -it $name make all      --directory=/root
fi
# al cambiar los permisos a 006 anda sin pedir sudo la ejecucion de qemu
docker exec -it $name chmod 006 /root/Image/x64BareBonesImage.qcow2
docker stop $name
docker rm $name

# --rtc base=localtime para q este correctamente seteado el RTC
if [ "$1" == "gdb" ]
then
    # modo debug (hay que hacer attach mediante gdb en otra imagen)
    qemu-system-x86_64 -S -s --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -monitor stdio
else
    # abro la imagen en qemu
    qemu-system-x86_64 --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512
fi
