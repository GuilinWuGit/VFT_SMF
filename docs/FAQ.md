# Frequently Asked Questions (FAQ)

This FAQ addresses the most common questions about VFT_SMF (Virtual Flight Test Simulation and Modeling Framework).

## 🚀 Getting Started

### Q: What is VFT_SMF?
**A**: VFT_SMF is a comprehensive open-source flight simulation platform designed for virtual flight testing experiments. It features a sophisticated digital twin architecture with event-driven multi-threaded simulation capabilities.

### Q: What are the system requirements?
**A**: 
- **OS**: Windows 10/11 (64-bit)
- **CPU**: Intel i5 or AMD equivalent (2.0 GHz+)
- **RAM**: 8 GB minimum (16 GB recommended)
- **Storage**: 2 GB available space
- **Compiler**: C++17 compatible (Visual Studio 2019/2022 or MinGW-w64)

### Q: How do I install VFT_SMF?
**A**: See the [Installation Guide](UserGuide/Installation_Guide.md) for detailed instructions. The quick version:
```bash
git clone https://github.com/GuilinWuGit/VFT_SMF.git
cd VFT_SMF/ScenarioExamples/B737_Taxi
.\build.bat
.\run_simulation_and_visualize.bat
```

## 🔧 Technical Questions

### Q: What aircraft are supported?
**A**: Currently, VFT_SMF supports:
- **Boeing 737-800**: Complete digital twin implementation
- **Extensible**: Easy to add new aircraft types through the plugin architecture

### Q: Can I run simulations on Linux or macOS?
**A**: Currently, VFT_SMF is primarily designed for Windows. Linux and macOS support are planned for future releases.

### Q: How accurate are the flight dynamics?
**A**: VFT_SMF uses high-fidelity aerodynamic models and flight dynamics calculations. The accuracy depends on:
- Aircraft model fidelity
- Environmental conditions
- Pilot input modeling
- System simulation detail

### Q: What's the maximum simulation time?
**A**: The maximum simulation time is configurable (default: 3600 seconds). For longer simulations, you can modify the `max_simulation_time` parameter in the configuration file.

## 🎮 Usage Questions

### Q: How do I create a custom scenario?
**A**: 
1. Copy an existing scenario from `ScenarioExamples/`
2. Modify `input/FlightPlan.json` for your flight plan
3. Adjust `config/SimulationConfig.json` for simulation parameters
4. Run the simulation with your custom scenario

### Q: Can I modify aircraft parameters?
**A**: Yes, aircraft parameters can be modified through:
- Configuration files in the scenario directory
- Aircraft model files in `src/B_AircraftAgentModel/`
- Runtime parameter adjustment (for supported parameters)

### Q: How do I visualize simulation results?
**A**: VFT_SMF includes several visualization tools:
```bash
# Flight state visualization
.\tools\visualize_flight_state.exe

# Force analysis
.\tools\visualize_aircraft_net_force.exe

# Event monitoring
.\tools\visualize_triggered_events.exe
```

### Q: Can I export data for external analysis?
**A**: Yes, VFT_SMF exports data in multiple formats:
- **CSV**: Flight data and performance metrics
- **JSON**: Configuration and state data
- **TXT**: Event logs and system messages
- **Custom formats**: Through the plugin system

## 🛠️ Development Questions

### Q: How do I contribute to VFT_SMF?
**A**: 
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

See the [Contributing Guidelines](DeveloperGuide/Contributing_Guidelines.md) for detailed information.

### Q: How do I add a new aircraft type?
**A**: 
1. Create a new aircraft model in `src/B_AircraftAgentModel/`
2. Implement the required interfaces
3. Add aircraft-specific parameters
4. Create configuration files
5. Add to the aircraft registry

See the [Module Development Guide](DeveloperGuide/Module_Development.md) for detailed instructions.

### Q: Can I create custom agents?
**A**: Yes, VFT_SMF supports custom agent development:
- Inherit from the base `Agent` class
- Implement required virtual methods
- Register your agent with the simulation manager
- Configure agent parameters

### Q: How do I debug simulation issues?
**A**: 
1. Check the log files in the output directory
2. Use the built-in debugging tools
3. Enable verbose logging in configuration
4. Use the visualization tools to inspect state
5. Check the [Troubleshooting Guide](Troubleshooting.md)

## 📊 Performance Questions

### Q: How fast can VFT_SMF run?
**A**: VFT_SMF can run at:
- **Real-time**: 1x simulation speed
- **Faster than real-time**: Up to 10x (depending on hardware)
- **Slower than real-time**: For detailed analysis

### Q: How many aircraft can be simulated simultaneously?
**A**: The number depends on:
- Hardware capabilities
- Simulation complexity
- Agent model fidelity
- Typically 10-50 aircraft on modern hardware

### Q: Can I run distributed simulations?
**A**: Yes, VFT_SMF supports distributed simulation across multiple nodes for large-scale scenarios.

### Q: How much memory does VFT_SMF use?
**A**: Memory usage depends on:
- Number of aircraft
- Simulation duration
- Data recording level
- Typically 2-8 GB for standard scenarios

## 🔒 Security and Safety

### Q: Is VFT_SMF safe to use?
**A**: Yes, VFT_SMF includes comprehensive safety measures:
- Input validation
- State consistency checking
- Error recovery mechanisms
- Comprehensive logging

### Q: Can I use VFT_SMF for training?
**A**: Yes, VFT_SMF is suitable for:
- Flight training simulation
- Research and development
- System testing and validation
- Educational purposes

### Q: Is my data secure?
**A**: VFT_SMF includes security features:
- Local data storage (no cloud transmission)
- Configurable access controls
- Audit logging
- Data encryption options

## 🤝 Community and Support

### Q: Where can I get help?
**A**: 
- **Documentation**: Check the [User Guide](UserGuide/) and [Developer Guide](DeveloperGuide/)
- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share experiences
- **Wiki**: Community-maintained documentation

### Q: How do I report a bug?
**A**: 
1. Search existing issues to avoid duplicates
2. Create a new issue with:
   - Clear description of the problem
   - Steps to reproduce
   - System information
   - Error messages/logs

### Q: Can I request new features?
**A**: Yes, feature requests are welcome! Please:
1. Check existing issues and discussions
2. Create a detailed feature request
3. Explain the use case and benefits
4. Consider contributing the implementation

### Q: Is VFT_SMF free to use?
**A**: Yes, VFT_SMF is open-source software released under the MIT License, making it free for both personal and commercial use.

## 📈 Future Development

### Q: What's planned for future releases?
**A**: Planned features include:
- Additional aircraft types
- Enhanced visualization tools
- Linux/macOS support
- Cloud-based simulation
- Advanced AI agents
- Real-time hardware-in-the-loop support

### Q: How often is VFT_SMF updated?
**A**: VFT_SMF follows a regular release schedule:
- **Major releases**: Every 6 months
- **Minor releases**: Every 2-3 months
- **Bug fixes**: As needed

### Q: Can I influence the development roadmap?
**A**: Yes! Community feedback is essential:
- Participate in discussions
- Submit feature requests
- Contribute code
- Share use cases and requirements

---

*Can't find your question here? Check the [Troubleshooting Guide](Troubleshooting.md) or ask in the [GitHub Discussions](https://github.com/GuilinWuGit/VFT_SMF/discussions).*
