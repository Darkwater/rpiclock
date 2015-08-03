# rpiclock

A shitty clock thing for the Raspberry Pi with the PiTFT touchscreen display.

Here's how I use it:

    while sleep 10s; do mpc -f '%artist%\b%title%' | head -n 1 | tr '\b' '\1' | ./rpiclock; done

Have fun.
