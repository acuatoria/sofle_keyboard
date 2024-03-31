## My sofle keyboard configuration.

It includes a matrix rain effect in the OLED screen, a custom layout and a custom keymap.

To compile it, you need to have the qmk firmware installed. You can find the instructions [here](https://beta.docs.qmk.fm/tutorial/newbs_getting_started).

The folder sofle of this repository should be copied to the keyboards folder of the qmk firmware in order to get the matrix rain effect working.


To get it running on your keyboard, you need to run the followings commands in the qmk folder:

```bash
qmk compile acuatoriav4.json -kb sofle -km acuatoria
```

To flash the firmware, you need to run the following command:

```bash
qmk flash sofle_rev1_acuatoriav4.hex
```

