# VFT_SMF V3 - Virtual Flight Test Simulation and Modeling Framework

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

## 🚀 Overview

VFT_SMF V3 is a comprehensive open-source flight simulation platform designed for virtual flight testing experiments. It features a sophisticated digital twin architecture with event-driven multi-threaded simulation, supporting large-scale distributed simulation systems for comprehensive virtual flight testing scenarios.

## ✨ Key Features

- **🔄 Digital Twin Architecture**: Complete digital twin implementation with DataTwin, ModelTwin, and ServiceTwin layers
- **⚡ Event-Driven Multi-threaded Simulation**: High-performance event-driven architecture with strict synchronization control
- **👥 5-Agent Modeling System**: Comprehensive agent models including Pilot, Aircraft, Environment, ATC, and Flight Dynamics
- **🎯 Centralized Event Management**: Centralized event monitoring and controller management systems
- **📊 Real-time Data Recording**: Comprehensive data recording and visualization capabilities
- **🔧 Modular Design**: Highly modular architecture supporting easy extension and customization

## 🏗️ Architecture

### Core Components

```
VFT_SMF V3/
├── src/
│   ├── A_PilotAgentModel/          # Pilot agent models
│   ├── B_AircraftAgentModel/       # Aircraft digital twin models
│   ├── C_EnvironmentAgentModel/    # Environment simulation
│   ├── D_ATCAgentModel/           # Air Traffic Control models
│   ├── E_FlightDynamics/          # Flight dynamics simulation
│   ├── F_ScenarioModelling/       # Flight plan parsing and scenario management
│   └── G_SimulationManager/       # Core simulation management
├── ScenarioExamples/              # Pre-configured simulation scenarios
├── tools/                         # Data visualization tools
└── codetest/                      # Unit and integration tests
```

### Digital Twin Architecture

- **DataTwin**: Physical and mental data representation
- **ModelTwin**: Decision-making and control algorithms
- **ServiceTwin**: State management and service interfaces

## 🚁 Supported Aircraft

- **Boeing 737**: Complete B737 digital twin implementation
- **Extensible**: Easy to add new aircraft types

## 📋 Prerequisites

- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **Windows 10/11** (primary platform)
- **Git** for version control
- **MATLAB** (optional, for advanced visualization)

## 🛠️ Installation

### Quick Start

1. **Clone the repository**:
   ```bash
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```

2. **Navigate to a scenario**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   ```

3. **Compile the project**:
   ```bash
   .\build.bat
   ```

4. **Run the simulation**:
   ```bash
   .\run_simulation_and_visualize.bat
   ```

### Detailed Installation

See [Installation Guide](docs/installation.md) for detailed setup instructions.

## 🎮 Usage

### Running Simulations

1. **Choose a scenario** from `ScenarioExamples/`
2. **Configure parameters** in `config/SimulationConfig.json`
3. **Define flight plan** in `input/FlightPlan.json`
4. **Run simulation** using the provided batch scripts

### Example Scenarios

- **B737_Taxi**: Taxi simulation with ground control
- **B737_LevelFlight**: Level flight simulation
- **Climb**: Climb performance analysis

### Data Visualization

The platform includes comprehensive visualization tools:

- **Flight State Visualization**: Real-time aircraft state monitoring
- **Net Force Analysis**: Force and moment analysis
- **Event Monitoring**: Event trigger visualization
- **Controller Status**: Controller execution status tracking

## 🧪 Testing

Run the test suite:

```bash
cd codetest
.\build_tests.bat
```

## 📊 Performance

- **Real-time Simulation**: Sub-millisecond synchronization
- **Multi-threaded Architecture**: Optimal CPU utilization
- **Memory Efficient**: Optimized data structures and buffering
- **Scalable**: Support for distributed simulation systems

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- **VFT_SMF Development Team** - *Initial work*

## 🙏 Acknowledgments

- Academic research community
- Open source contributors
- Flight simulation enthusiasts

## 📚 Documentation

- [Architecture Guide](docs/architecture.md)
- [API Reference](docs/api.md)
- [Troubleshooting](docs/troubleshooting.md)

## 🔗 Links

- **Repository**: https://github.com/GuilinWuGit/VFT_SMF
- **Issues**: https://github.com/GuilinWuGit/VFT_SMF/issues
- **Discussions**: https://github.com/GuilinWuGit/VFT_SMF/discussions

## 📈 Project Status

- **Version**: 3.0.0
- **Status**: Active Development
- **Last Updated**: January 2024

---

⭐ **If you find this project useful, please give it a star!**
