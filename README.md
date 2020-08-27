# Network Simulator

  * `plan/` -- Original project plan.

  * `doc/` -- Project documentation, including examples and latex files.

  * `src/nwsim` -- All source files, including CMake

## Build nwsim
```bash
cd src/nwsim
cmake CMakeLists.txt
make
cd ../../bin
```

## Run nwsim
```bash
./bin/nwsim-cli
```
Use the `help` command to get help on possible commands. Example input can be found in `./doc/examples/input.txt`