# NAM JUCE: Neural Amp Modeler JUCE Implementation

A JUCE implementation of Steven Atkinson's [NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin). This Repository is still a work-in-proress, but the basic functionality is there.

</br>

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/580466872169005066/1109348678021414963/namjuce.png" />
</p>

</br>

## Building with CMake

</br>

### Prerequisites


A global installation of [JUCE](https://github.com/juce-framework/JUCE) is required in order to build the project using CMake. After cloning the JUCE repo, a global installation can be built by running:

```bash
cmake -B cmake-build-install -DCMAKE_INSTALL_PREFIX=/path/to/JUCE/install
cmake --build cmake-build-install --target install
```

### Building on Windows

```bash
git submodule update --init --recursive
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/JUCE/install
cmake --build build --config Release
```

### Building on MacOS/Linux

```bash
git submodule update --init --recursive
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/JUCE/install -DCMAKE_BUILD_TYPE=Release
cmake --build build
```