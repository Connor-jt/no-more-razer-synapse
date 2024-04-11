# random razer device project

this is designed to remove the need for the annoying razer synapse backround processes, by doing literally all i use razer synapse for: sending cool colors to my keyboard and other devices

its basically just an api for formatting RGB data to send to the devices, which means you can just plop in a new effect code file and write your own logic for whatever effect you want

this is designed more for developers than end users, as all effects have to be programmed into the main.cpp file, but doing so is rather easy and intuitive, and coding new effects is also very simple

WARNING: there is the possiblity that devices may begin to function differently than expected if exposed to any data posted by this tool, i only reverse engineered so much logic from the official razer tool

for example, my tartarusV2 now seems to be slightly incompatible with the official razer synapse tool lighting effects (the lighting turns off after about 5 seconds of razer sending it lighting data, which is pretty funny)


supported devices: Blackwidow Chroma, TartarusV2, Goliathus Extended
