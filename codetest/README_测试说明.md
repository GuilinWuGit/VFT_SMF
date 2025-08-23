# VFT_SMF V3 测试框架说明

## 概述

本目录包含VFT_SMF V3项目的完整测试框架，包括单元测试、集成测试、性能测试和回归测试。

## 目录结构

```
codetest/
├── tests/                          # 测试源代码
│   ├── unit/                       # 单元测试
│   │   ├── aircraft/              # 飞机模型测试
│   │   ├── pilot/                 # 飞行员模型测试
│   │   ├── atc/                   # ATC模型测试
│   │   ├── environment/           # 环境模型测试
│   │   └── simulation/            # 仿真管理测试
│   ├── integration/               # 集成测试
│   ├── performance/               # 性能测试
│   └── regression/                # 回归测试
├── test_data/                     # 测试数据
├── test_output/                   # 测试输出（运行时生成）
├── build_tests.bat               # 完整测试编译脚本
├── build_tests_simple.bat        # 简化测试编译脚本
└── README_测试说明.md            # 本文件
```

## 测试类型

### 1. 单元测试 (Unit Tests)
- **目的**: 测试单个模块或类的功能
- **位置**: `tests/unit/`
- **示例**: B737数字孪生、控制优先级管理器等

### 2. 集成测试 (Integration Tests)
- **目的**: 测试多个模块之间的交互
- **位置**: `tests/integration/`
- **示例**: 完整仿真流程、数据流测试等

### 3. 性能测试 (Performance Tests)
- **目的**: 验证系统性能指标
- **位置**: `tests/performance/`
- **示例**: 仿真速度、内存使用等

### 4. 回归测试 (Regression Tests)
- **目的**: 确保新功能不破坏现有功能
- **位置**: `tests/regression/`
- **示例**: 已知场景的重复测试

## 使用方法

### 1. 安装Google Test

#### 方法一：手动安装（推荐）
```bash
# 1. 下载Google Test
git clone https://github.com/google/googletest.git

# 2. 编译
cd googletest
mkdir build
cd build
cmake ..
cmake --build . --config Release

# 3. 复制文件到测试目录
# 将编译后的库文件复制到 codetest/lib/
# 将头文件复制到 codetest/include/
```

#### 方法二：使用vcpkg
```bash
vcpkg install gtest
```

### 2. 编译测试

#### 使用简化脚本（推荐）
```bash
cd codetest
.\build_tests_simple.bat
```

#### 使用完整脚本
```bash
cd codetest
.\build_tests.bat
```

### 3. 运行测试

```bash
cd codetest/test_output
.\run_tests.exe
```

## 测试覆盖率目标

| 模块 | 目标覆盖率 | 当前覆盖率 | 状态 |
|------|------------|------------|------|
| 飞机模型 | 90% | 0% | ⏳ 待实现 |
| 飞行员模型 | 85% | 0% | ⏳ 待实现 |
| ATC模型 | 80% | 0% | ⏳ 待实现 |
| 环境模型 | 75% | 0% | ⏳ 待实现 |
| 仿真管理 | 95% | 0% | ⏳ 待实现 |
| 数据记录 | 90% | 0% | ⏳ 待实现 |

## 测试编写规范

### 1. 文件命名
- 测试文件以 `test_` 开头
- 与被测试文件对应：`test_模块名.cpp`

### 2. 测试类命名
- 测试类以 `Test` 结尾
- 例如：`B737DigitalTwinTest`

### 3. 测试用例命名
- 使用描述性的测试名称
- 格式：`功能_条件_期望结果`
- 例如：`SystemStateUpdate_ValidInput_StateUpdatedCorrectly`

### 4. 测试结构
```cpp
class ModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化代码
    }
    
    void TearDown() override {
        // 清理代码
    }
    
    // 测试数据成员
};

TEST_F(ModuleTest, TestCaseName) {
    // 测试代码
    EXPECT_EQ(actual, expected);
}
```

## 测试数据管理

### 1. 测试配置文件
- 位置：`test_data/`
- 格式：JSON
- 用途：提供测试用的仿真配置

### 2. 测试输出
- 位置：`test_output/`
- 包含：测试结果、覆盖率报告、性能数据

## 持续集成

### GitHub Actions配置
```yaml
name: Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v2
    - name: Install dependencies
      run: |
        # 安装Google Test
    - name: Build tests
      run: cd codetest && build_tests_simple.bat
    - name: Run tests
      run: cd codetest/test_output && run_tests.exe
```

## 故障排除

### 常见问题

1. **"找不到Google Test"**
   - 确保已正确安装Google Test
   - 检查库文件路径是否正确

2. **"编译失败"**
   - 检查源文件路径
   - 确认所有依赖文件存在

3. **"测试运行失败"**
   - 查看测试输出获取详细错误信息
   - 检查测试数据文件是否正确

### 调试建议

1. 先运行单个测试文件
2. 使用 `--gtest_filter` 过滤特定测试
3. 启用详细输出：`--gtest_verbose`

## 下一步计划

1. **第一阶段**：完善单元测试覆盖
2. **第二阶段**：添加集成测试
3. **第三阶段**：性能测试基准
4. **第四阶段**：自动化测试流程

## 贡献指南

1. 为新功能编写对应的测试
2. 确保测试覆盖率不低于目标
3. 遵循测试编写规范
4. 及时更新测试文档
