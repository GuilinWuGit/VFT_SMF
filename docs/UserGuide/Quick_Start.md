# Quick Start Guide

Get up and running with VFT_SMF in minutes! This guide will walk you through running your first flight simulation.

## 🚀 Prerequisites

Before starting, ensure you have:
- ✅ VFT_SMF installed (see [Installation Guide](Installation_Guide.md))
- ✅ A C++17 compatible compiler
- ✅ At least 8 GB RAM available

## ⚡ Quick Start (5 Minutes)

### Step 1: Navigate to a Scenario
```bash
cd ScenarioExamples/B737_Taxi
```

### Step 2: Build the Simulation
```bash
.\build.bat
```

### Step 3: Run the Simulation
```bash
.\run_simulation_and_visualize.bat
```

### Step 4: View Results
The simulation will automatically:
- Run the B737 taxi scenario
- Generate visualization data
- Display results in the console

## 📊 Understanding the Output

### Console Output
You should see output similar to:
```
VFT_SMF Simulation Started
================================
Time: 0.00s - Aircraft: B737-800
Position: Lat=40.7128, Lon=-74.0060, Alt=0m
Speed: 0.0 m/s, Heading: 0.0°
Status: On Ground, Engine: Off

Time: 0.01s - Event: Engine Start Initiated
...

Simulation Completed Successfully
Total Time: 120.0s
Data saved to: output/simulation_data.csv
```

### Generated Files
- `output/simulation_data.csv` - Flight data
- `output/events_log.txt` - Event timeline
- `output/performance_metrics.json` - Performance statistics

## 🎮 Interactive Features

### Real-time Monitoring
During simulation, you can:
- **Pause**: Press `P` to pause/resume
- **Step**: Press `S` for step-by-step execution
- **Stop**: Press `Q` to quit

### Data Visualization
After simulation, visualize results:
```bash
# View flight path
.\tools\visualize_flight_state.exe

# Analyze forces
.\tools\visualize_aircraft_net_force.exe

# Monitor events
.\tools\visualize_triggered_events.exe
```

## 🔧 Customizing Your First Simulation

### Modify Configuration
Edit `config/SimulationConfig.json`:
```json
{
  "simulation_time_step": 0.01,
  "max_simulation_time": 300.0,
  "output_directory": "output",
  "log_level": "INFO"
}
```

### Change Flight Plan
Edit `input/FlightPlan.json`:
```json
{
  "aircraft_type": "B737-800",
  "departure": {
    "airport": "KJFK",
    "runway": "04L"
  },
  "arrival": {
    "airport": "KJFK", 
    "runway": "22R"
  },
  "waypoints": [
    {"lat": 40.7128, "lon": -74.0060, "alt": 0},
    {"lat": 40.7130, "lon": -74.0058, "alt": 0}
  ]
}
```

## 📈 Next Steps

### Explore Different Scenarios
```bash
# Level flight simulation
cd ../B737_LevelFlight
.\run_simulation_and_visualize.bat

# Create custom scenario
cd ..
.\create_new_scenario.bat
```

### Learn Advanced Features
- **[Scenario Management](Scenario_Management.md)** - Create custom scenarios
- **[Visualization Tools](Visualization_Tools.md)** - Advanced data analysis
- **[Configuration Reference](Configuration_Reference.md)** - All options explained

## 🆘 Troubleshooting

### Common Issues

#### "Build failed"
```bash
# Check compiler
g++ --version
# Reinstall Visual Studio C++ workload if needed
```

#### "Configuration file not found"
```bash
# Ensure you're in the correct directory
pwd
# Check file exists
dir config\SimulationConfig.json
```

#### "Memory allocation failed"
- Close other applications
- Increase virtual memory
- Use smaller simulation time

### Getting Help
- Check [Troubleshooting Guide](../Troubleshooting.md)
- Search [GitHub Issues](https://github.com/GuilinWuGit/VFT_SMF/issues)
- Ask in [Discussions](https://github.com/GuilinWuGit/VFT_SMF/discussions)

## 🎯 Success Checklist

- ✅ Successfully built the project
- ✅ Ran first simulation
- ✅ Viewed output data
- ✅ Understood basic configuration
- ✅ Explored visualization tools

**Congratulations!** You're now ready to explore the full capabilities of VFT_SMF.

---

*Need more help? Check out the [Examples Collection](Examples_Collection.md) for ready-to-use scenarios.*
