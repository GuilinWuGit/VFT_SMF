# Configuration Reference

This document provides a complete reference for all configuration options in VFT_SMF (Virtual Flight Test Simulation and Modeling Framework).

## 📋 Configuration Overview

VFT_SMF uses JSON-based configuration files for flexible and human-readable settings. The main configuration file is located at:
```
config/SimulationConfig.json
```

## 🏗️ Main Configuration Structure

```json
{
  "simulation": {
    "time_step": 0.01,
    "max_time": 3600.0,
    "real_time": false
  },
  "output": {
    "directory": "output",
    "enabled": true,
    "frequency": 1
  },
  "logging": {
    "level": "INFO",
    "file": "simulation.log"
  },
  "agents": {
    "pilot": { /* pilot configuration */ },
    "aircraft": { /* aircraft configuration */ },
    "environment": { /* environment configuration */ },
    "atc": { /* ATC configuration */ },
    "flight_dynamics": { /* flight dynamics configuration */ }
  },
  "events": {
    "enabled": true,
    "queue_size": 1000
  },
  "visualization": {
    "enabled": true,
    "real_time": false
  }
}
```

## ⏱️ Simulation Configuration

### Time Settings
```json
{
  "simulation": {
    "time_step": 0.01,           // Simulation time step (seconds)
    "max_time": 3600.0,          // Maximum simulation time (seconds)
    "real_time": false,          // Run in real-time mode
    "speed_multiplier": 1.0      // Simulation speed multiplier
  }
}
```

**Parameters:**
- `time_step`: Time increment for each simulation step (0.001 - 0.1 seconds)
- `max_time`: Maximum duration of simulation (1.0 - 86400.0 seconds)
- `real_time`: Enable real-time synchronization
- `speed_multiplier`: Simulation speed relative to real-time (0.1 - 10.0)

## 📊 Output Configuration

### Data Recording
```json
{
  "output": {
    "directory": "output",           // Output directory path
    "enabled": true,                 // Enable data recording
    "frequency": 1,                  // Recording frequency (every N steps)
    "format": "CSV",                 // Output format (CSV, JSON, BINARY)
    "compression": false,            // Enable data compression
    "include_events": true,          // Include event data
    "include_performance": true      // Include performance metrics
  }
}
```

**Parameters:**
- `directory`: Path for output files (relative or absolute)
- `enabled`: Enable/disable data recording
- `frequency`: Record data every N simulation steps (1 - 100)
- `format`: Output file format
- `compression`: Enable data compression for large datasets
- `include_events`: Include event log in output
- `include_performance`: Include performance metrics

### Output Files
The following files are generated:
- `simulation_data.csv`: Main simulation data
- `events_log.txt`: Event timeline
- `performance_metrics.json`: Performance statistics
- `configuration_backup.json`: Configuration snapshot

## 📝 Logging Configuration

### Log Settings
```json
{
  "logging": {
    "level": "INFO",                 // Log level (DEBUG, INFO, WARNING, ERROR)
    "file": "simulation.log",        // Log file path
    "console": true,                 // Enable console output
    "max_file_size": "10MB",         // Maximum log file size
    "backup_count": 5,               // Number of backup files
    "format": "detailed"             // Log format (simple, detailed)
  }
}
```

**Log Levels:**
- `DEBUG`: Detailed debugging information
- `INFO`: General information messages
- `WARNING`: Warning messages
- `ERROR`: Error messages only

## 👥 Agent Configuration

### Pilot Agent
```json
{
  "agents": {
    "pilot": {
      "enabled": true,
      "skill_level": "expert",       // Skill level (novice, intermediate, expert)
      "fatigue_model": true,         // Enable fatigue modeling
      "reaction_time": 0.2,          // Reaction time (seconds)
      "decision_model": "rule_based" // Decision model type
    }
  }
}
```

### Aircraft Agent
```json
{
  "agents": {
    "aircraft": {
      "enabled": true,
      "type": "B737-800",            // Aircraft type
      "mass": 70000.0,               // Aircraft mass (kg)
      "fuel_capacity": 20000.0,      // Fuel capacity (kg)
      "systems_modeling": "detailed" // Systems modeling level
    }
  }
}
```

### Environment Agent
```json
{
  "agents": {
    "environment": {
      "enabled": true,
      "weather_model": "realistic",  // Weather model type
      "wind_enabled": true,          // Enable wind effects
      "turbulence_enabled": true,    // Enable turbulence
      "terrain_enabled": true        // Enable terrain effects
    }
  }
}
```

### ATC Agent
```json
{
  "agents": {
    "atc": {
      "enabled": true,
      "communication_model": "realistic", // Communication model
      "separation_standards": "ICAO",     // Separation standards
      "traffic_density": "medium"         // Traffic density level
    }
  }
}
```

### Flight Dynamics Agent
```json
{
  "agents": {
    "flight_dynamics": {
      "enabled": true,
      "aerodynamic_model": "high_fidelity", // Aerodynamic model
      "engine_model": "detailed",           // Engine model
      "control_surfaces": true,             // Control surface modeling
      "stability_derivatives": true         // Stability derivatives
    }
  }
}
```

## ⚡ Event Configuration

### Event System
```json
{
  "events": {
    "enabled": true,
    "queue_size": 1000,              // Event queue size
    "priority_levels": 5,            // Number of priority levels
    "timeout": 1.0,                  // Event timeout (seconds)
    "logging": true                  // Enable event logging
  }
}
```

## 🎨 Visualization Configuration

### Visualization Settings
```json
{
  "visualization": {
    "enabled": true,
    "real_time": false,              // Real-time visualization
    "update_frequency": 30,          // Update frequency (Hz)
    "window_size": [1024, 768],      // Window size [width, height]
    "fullscreen": false,             // Fullscreen mode
    "anti_aliasing": true            // Enable anti-aliasing
  }
}
```

## 🌐 Network Configuration

### Distributed Simulation
```json
{
  "network": {
    "enabled": false,
    "bind_address": "0.0.0.0",       // Bind address
    "port": 8080,                    // Network port
    "timeout": 5000,                 // Connection timeout (ms)
    "max_connections": 10,           // Maximum connections
    "encryption": false              // Enable encryption
  }
}
```

## 🔧 Performance Configuration

### Performance Settings
```json
{
  "performance": {
    "thread_count": 0,               // Number of threads (0 = auto)
    "memory_limit": "8GB",           // Memory limit
    "cache_size": "1GB",             // Cache size
    "optimization_level": "balanced" // Optimization level
  }
}
```

**Optimization Levels:**
- `speed`: Maximum performance, higher resource usage
- `balanced`: Balanced performance and resource usage
- `memory`: Minimum memory usage, lower performance

## 🔒 Security Configuration

### Security Settings
```json
{
  "security": {
    "access_control": false,         // Enable access control
    "encryption": false,             // Enable data encryption
    "audit_logging": true,           // Enable audit logging
    "session_timeout": 3600          // Session timeout (seconds)
  }
}
```

## 📁 File Paths Configuration

### Path Settings
```json
{
  "paths": {
    "scenarios": "ScenarioExamples",     // Scenarios directory
    "tools": "tools",                    // Tools directory
    "templates": "templates",            // Template files
    "temp": "temp"                       // Temporary files
  }
}
```

## 🔄 Environment Variables

### Supported Environment Variables
```bash
VFT_SMF_HOME=C:\path\to\VFT_SMF
VFT_SMF_DATA=C:\path\to\VFT_SMF\data
VFT_SMF_CONFIG=C:\path\to\VFT_SMF\config
VFT_SMF_LOG=C:\path\to\VFT_SMF\log
```

## 📋 Configuration Validation

### Validation Rules
The configuration system validates:
- **Data Types**: Correct data types for each parameter
- **Value Ranges**: Parameters within acceptable ranges
- **Dependencies**: Required parameters for enabled features
- **File Paths**: Valid and accessible file paths

### Error Handling
- **Missing Parameters**: Use default values
- **Invalid Values**: Log warnings and use defaults
- **File Errors**: Create directories if missing
- **Validation Errors**: Detailed error messages

## 🔧 Configuration Examples

### Minimal Configuration
```json
{
  "simulation": {
    "time_step": 0.01,
    "max_time": 300.0
  },
  "output": {
    "enabled": true,
    "directory": "output"
  }
}
```

### High-Performance Configuration
```json
{
  "simulation": {
    "time_step": 0.005,
    "max_time": 7200.0,
    "real_time": false
  },
  "performance": {
    "thread_count": 8,
    "optimization_level": "speed"
  },
  "output": {
    "enabled": true,
    "frequency": 5,
    "compression": true
  }
}
```

### Debug Configuration
```json
{
  "logging": {
    "level": "DEBUG",
    "console": true
  },
  "simulation": {
    "time_step": 0.01,
    "max_time": 60.0
  },
  "output": {
    "enabled": true,
    "frequency": 1
  }
}
```

---

*For advanced configuration options, see the [Developer Guide](../DeveloperGuide/Development_Setup.md).*
