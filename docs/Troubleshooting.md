# Troubleshooting Guide

This guide helps you resolve common issues with VFT_SMF (Virtual Flight Test Simulation and Modeling Framework).

## 🔍 Quick Diagnosis

### Check System Status
```bash
# Verify installation
cd VFT_SMF
dir

# Check compiler
g++ --version

# Verify scenario files
cd ScenarioExamples/B737_Taxi
dir
```

## 🚨 Common Issues and Solutions

### Build Issues

#### "Compiler not found" Error
**Symptoms**: Build fails with compiler-related errors
**Solutions**:
1. **Install Visual Studio C++ workload**:
   - Open Visual Studio Installer
   - Modify your installation
   - Add "Desktop development with C++" workload

2. **Check PATH environment**:
   ```bash
   echo %PATH%
   # Ensure Visual Studio or MinGW is in PATH
   ```

3. **Use Developer Command Prompt**:
   - Open "Developer Command Prompt for VS"
   - Navigate to project directory
   - Run build commands

#### "Missing libraries" Error
**Symptoms**: Linker errors about missing libraries
**Solutions**:
1. **Install required dependencies**:
   ```bash
   # For Visual Studio
   vcpkg install jsoncpp
   vcpkg install eigen3
   ```

2. **Check library paths**:
   - Verify library installation directories
   - Update build scripts with correct paths

3. **Rebuild dependencies**:
   ```bash
   .\build_dependencies.bat
   ```

#### "Permission denied" Error
**Symptoms**: Access denied when building or running
**Solutions**:
1. **Run as Administrator**:
   - Right-click Command Prompt
   - Select "Run as administrator"

2. **Check file permissions**:
   ```bash
   icacls . /grant Users:F /T
   ```

3. **Disable antivirus temporarily**:
   - Add project directory to antivirus exclusions

### Runtime Issues

#### "Configuration file not found"
**Symptoms**: Simulation fails to start, missing config files
**Solutions**:
1. **Check file paths**:
   ```bash
   dir config\SimulationConfig.json
   dir input\FlightPlan.json
   ```

2. **Copy from template**:
   ```bash
   copy config\SimulationConfig.template.json config\SimulationConfig.json
   copy input\FlightPlan.template.json input\FlightPlan.json
   ```

3. **Verify working directory**:
   ```bash
   cd ScenarioExamples\B737_Taxi
   pwd
   ```

#### "Memory allocation failed"
**Symptoms**: Out of memory errors during simulation
**Solutions**:
1. **Close other applications**:
   - Free up RAM by closing unnecessary programs
   - Restart computer if needed

2. **Reduce simulation scope**:
   - Decrease `max_simulation_time` in config
   - Reduce number of aircraft
   - Lower data recording frequency

3. **Increase virtual memory**:
   - System Properties → Advanced → Performance Settings
   - Virtual Memory → Change → Custom size

#### "Graphics initialization failed"
**Symptoms**: Visualization tools fail to start
**Solutions**:
1. **Update graphics drivers**:
   - Download latest drivers from manufacturer website
   - Restart computer after installation

2. **Check DirectX compatibility**:
   ```bash
   dxdiag
   # Verify DirectX 11+ support
   ```

3. **Use software rendering**:
   - Modify visualization config to use CPU rendering
   - Disable hardware acceleration if needed

### Performance Issues

#### Slow Simulation Performance
**Symptoms**: Simulation runs slower than expected
**Solutions**:
1. **Optimize configuration**:
   ```json
   {
     "simulation_time_step": 0.02,
     "log_level": "WARNING",
     "data_recording_frequency": 10
   }
   ```

2. **Check system resources**:
   - Monitor CPU and memory usage
   - Close background applications
   - Ensure adequate cooling

3. **Use release build**:
   ```bash
   .\build_release.bat
   ```

#### High Memory Usage
**Symptoms**: Excessive memory consumption
**Solutions**:
1. **Reduce data recording**:
   - Lower recording frequency
   - Limit recorded variables
   - Use data compression

2. **Optimize agent models**:
   - Simplify complex calculations
   - Use more efficient algorithms
   - Implement data pooling

3. **Monitor memory leaks**:
   - Use memory profiling tools
   - Check for resource cleanup issues

### Data and Output Issues

#### "No output files generated"
**Symptoms**: Simulation runs but no data files created
**Solutions**:
1. **Check output directory**:
   ```bash
   dir output\
   # Ensure directory exists and is writable
   ```

2. **Verify data recording settings**:
   ```json
   {
     "output_directory": "output",
     "enable_data_recording": true,
     "data_recording_frequency": 1
   }
   ```

3. **Check file permissions**:
   - Ensure write access to output directory
   - Disable read-only attributes

#### "Corrupted data files"
**Symptoms**: Output files are incomplete or corrupted
**Solutions**:
1. **Check disk space**:
   ```bash
   dir
   # Ensure adequate free space
   ```

2. **Verify simulation completion**:
   - Check for error messages
   - Ensure simulation ran to completion
   - Review log files

3. **Use data validation**:
   - Enable data integrity checks
   - Use checksums for verification

### Network and Communication Issues

#### "Distributed simulation connection failed"
**Symptoms**: Multi-node simulation fails to connect
**Solutions**:
1. **Check network configuration**:
   - Verify IP addresses and ports
   - Check firewall settings
   - Test network connectivity

2. **Configure network settings**:
   ```json
   {
     "network": {
       "bind_address": "0.0.0.0",
       "port": 8080,
       "timeout": 5000
     }
   }
   ```

3. **Use local simulation**:
   - Test with single-node simulation first
   - Verify local functionality before distributed setup

## 🔧 Advanced Troubleshooting

### Debug Mode
Enable debug logging for detailed information:
```json
{
  "log_level": "DEBUG",
  "enable_debug_output": true,
  "debug_file": "debug.log"
}
```

### System Diagnostics
Run system diagnostics:
```bash
# Check system information
systeminfo

# Verify C++ runtime
dir C:\Windows\System32\msvcp140.dll

# Test compiler
echo #include <iostream> > test.cpp
echo int main() { std::cout << "Hello World"; return 0; } >> test.cpp
g++ test.cpp -o test.exe
.\test.exe
```

### Performance Profiling
Use built-in profiling tools:
```bash
# Enable performance monitoring
.\tools\performance_monitor.exe

# Generate performance report
.\tools\generate_performance_report.bat
```

## 📞 Getting Help

### Before Asking for Help
1. **Check this troubleshooting guide**
2. **Search existing issues** on GitHub
3. **Review log files** for error details
4. **Try minimal reproduction case**

### When Reporting Issues
Include the following information:
- **VFT_SMF version**: Check version in README
- **Operating system**: Windows version and build
- **Compiler version**: Output of `g++ --version`
- **Error messages**: Complete error text
- **Steps to reproduce**: Detailed reproduction steps
- **System specifications**: CPU, RAM, graphics card
- **Log files**: Relevant log output

### Support Channels
- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and help
- **Documentation**: Check User Guide and Developer Guide
- **Community**: Join user forums and mailing lists

## 🛠️ Recovery Procedures

### Complete Reinstallation
If all else fails:
1. **Backup your data**:
   ```bash
   xcopy output backup\output /E /I
   xcopy config backup\config /E /I
   ```

2. **Clean installation**:
   ```bash
   cd ..
   rmdir /S VFT_SMF
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```

3. **Restore configuration**:
   ```bash
   xcopy backup\config config\ /E /I
   ```

### Data Recovery
For corrupted simulation data:
1. **Check backup files**: Look for `.bak` or backup directories
2. **Use data repair tools**: Built-in data validation and repair
3. **Partial recovery**: Extract valid portions of data files

---

*Still having issues? Check the [FAQ](FAQ.md) or create a new [GitHub Issue](https://github.com/GuilinWuGit/VFT_SMF/issues) with detailed information.*
