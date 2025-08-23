# Installation Guide

This guide will help you install VFT_SMF (Virtual Flight Test Simulation and Modeling Framework) on your system.

## 📋 Prerequisites

Before installing VFT_SMF, ensure your system meets the following requirements:

### System Requirements
- **Operating System**: Windows 10/11 (64-bit)
- **Processor**: Intel i5 or AMD equivalent (2.0 GHz or higher)
- **Memory**: 8 GB RAM minimum (16 GB recommended)
- **Storage**: 2 GB available disk space
- **Graphics**: DirectX 11 compatible graphics card

### Software Requirements
- **C++17 Compiler**: 
  - Visual Studio 2019/2022 with C++ workload
  - Or MinGW-w64 with GCC 8.0+
- **Git**: Version 2.20 or higher
- **CMake**: Version 3.15 or higher (optional, for advanced builds)
- **MATLAB**: R2019b or higher (optional, for advanced visualization)

## 🚀 Quick Installation

### Step 1: Clone the Repository
```bash
git clone https://github.com/GuilinWuGit/VFT_SMF.git
cd VFT_SMF
```

### Step 2: Navigate to a Scenario
```bash
cd ScenarioExamples/B737_Taxi
```

### Step 3: Build the Project
```bash
.\build.bat
```

### Step 4: Run Your First Simulation
```bash
.\run_simulation_and_visualize.bat
```

## 🔧 Detailed Installation

### Option 1: Using Pre-built Binaries (Recommended for Users)

1. **Download the latest release** from the [GitHub releases page](https://github.com/GuilinWuGit/VFT_SMF/releases)
2. **Extract the archive** to your desired installation directory
3. **Navigate to the scenario directory**:
   ```bash
   cd VFT_SMF/ScenarioExamples/B737_Taxi
   ```
4. **Run the simulation**:
   ```bash
   .\run_simulation_and_visualize.bat
   ```

### Option 2: Building from Source (For Developers)

#### Windows with Visual Studio

1. **Install Visual Studio 2019/2022** with C++ development tools
2. **Clone the repository**:
   ```bash
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```
3. **Open Developer Command Prompt** and navigate to the project
4. **Build the project**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   .\build.bat
   ```

#### Windows with MinGW-w64

1. **Install MinGW-w64** and add it to your PATH
2. **Clone the repository**:
   ```bash
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```
3. **Build the project**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   .\build.bat
   ```

## ⚙️ Configuration

### Environment Variables
Set the following environment variables (optional):
```bash
VFT_SMF_HOME=C:\path\to\VFT_SMF
VFT_SMF_DATA=C:\path\to\VFT_SMF\data
```

### Configuration Files
The main configuration file is located at:
```
config/SimulationConfig.json
```

Key configuration options:
- `simulation_time_step`: Time step for simulation (default: 0.01s)
- `max_simulation_time`: Maximum simulation duration (default: 3600s)
- `output_directory`: Directory for simulation outputs
- `log_level`: Logging level (DEBUG, INFO, WARNING, ERROR)

## 🧪 Verification

After installation, verify that everything is working:

1. **Run the test suite**:
   ```bash
   cd codetest
   .\build_tests.bat
   ```

2. **Check the output** - You should see:
   ```
   All tests passed!
   ```

3. **Run a sample simulation**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   .\run_simulation_and_visualize.bat
   ```

## 🔍 Troubleshooting

### Common Issues

#### Build Errors
- **"Compiler not found"**: Install Visual Studio with C++ workload
- **"Missing libraries"**: Ensure all dependencies are installed
- **"Permission denied"**: Run as administrator

#### Runtime Errors
- **"Configuration file not found"**: Check file paths in config
- **"Memory allocation failed"**: Increase available RAM
- **"Graphics initialization failed"**: Update graphics drivers

### Getting Help

If you encounter issues:
1. Check the [Troubleshooting Guide](../Troubleshooting.md)
2. Search existing [GitHub Issues](https://github.com/GuilinWuGit/VFT_SMF/issues)
3. Create a new issue with detailed error information

## 📚 Next Steps

After successful installation:
1. Read the [Quick Start Guide](Quick_Start.md)
2. Explore [Example Scenarios](Examples_Collection.md)
3. Learn about [Configuration Options](Configuration_Reference.md)

---

*For developers, see the [Development Setup Guide](../DeveloperGuide/Development_Setup.md) for advanced configuration.*
