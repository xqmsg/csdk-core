**XQ C Library **

This is the XQ SDK for native applications and embedded devices.

1. Install Prerequisites (Certain distros will have some of these installed already):

```shell
sudo apt install openssl curl libcurl4-openssl-dev
```

2. You will need to add your XQ API key to the `xq.ini` file in the `config` folder.
3. Build the XQ library itself:

```shell
mkdir build
cd build
cmake .. && make
```

