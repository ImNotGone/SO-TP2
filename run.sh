#!/bin/bash
if [ -z $SO_IMAGE ]; then
    SO_IMAGE='agodio/itba-so:1.0'
fi

name=TP2SO
MM="BITMAP"
# abro docker y ejecuto los make necesarios
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name $name $SO_IMAGE
docker exec -it $name make clean        --directory=/root/Toolchain
docker exec -it $name make all          --directory=/root/Toolchain
docker exec -it $name make clean        --directory=/root
if [ "$1" == "gdb" ]; then
    docker exec -it $name make gdb MM=$MM     --directory=/root
elif [ "$1" == "pvs" ]; then
    docker exec -it $name make pvs MM=$MM     --directory=/root
elif [ "$1" == "cpp-check" ]; then
    docker exec -it $name make cpp-check --directory=/root
else
    docker exec -it $name make all MM=$MM --directory=/root
fi
# al cambiar los permisos a 006 anda sin pedir sudo la ejecucion de qemu
if [ "$1" != "cpp-check" ]; then
    docker exec -it $name chmod 006 /root/Image/x64BareBonesImage.qcow2
fi

docker stop $name
docker rm $name

# --rtc base=localtime para q este correctamente seteado el RTC
if [ "$1" == "gdb" ]; then
    # modo debug (hay que hacer attach mediante gdb en otra imagen)
    qemu-system-x86_64 -S -s --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -monitor stdio
    # qemu-system-x86_64 -S -s --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512 -d int 2>&1 | grep "v="
elif [ $# == 0 ]; then
    # abro la imagen en qemu
    qemu-system-x86_64 --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512
    # qemu-system-x86_64 --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512 -d int 2>&1 | grep "v="
fi

