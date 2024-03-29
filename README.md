# NAM JUCE: Neural Amp Modeler JUCE Implementation

A JUCE implementation of Steven Atkinson's [NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin). This Repository is still a work-in-proress, but the basic functionality is there.

</br>

<p align="center">
  <img src="https://loudsample.com/images/assets/nam/Screenshot_20240226_125626.png" />
</p>


## Building with CMake


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
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/JUCE/install
cmake --build build
```

## Building with the Projucer

The project can also be build with the Projucer instead of CMake. Exporters for all platforms are set within the .jucer file including relative paths for all libraries/dependencies so no additional configurations are needed.

The git submodules still need to be initialized before attempting to build. Initialize by running:

```bash
git submodule update --init --recursive
```

## Supported Platforms

- Windows
- MacOS
- Linux

## Supported Formats

- VST3
- AU
- Standalone Application

Note: The Standalone application doesn't support ASIO by default. For ASIO support the `JUCE_ASIO` flag must be set in the `juce_audio_devices` module. 

Additionally, a path to Steingberg's ASIO SDK needs to be provided to CMake/Projucer.

More plugin formats like LV2 and VST(Legacy) can be built by providing the appropriate SDKs.

