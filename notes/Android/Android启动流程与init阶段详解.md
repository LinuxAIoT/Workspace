# Android 启动流程与 init 阶段详解（Android 11 源码级分析）

## 0. init 的整体架构（基于 Android 11 源码）

init 分为两个主要阶段：
- **First Stage Init**（`first_stage_main.cpp`）：在 ramdisk 中执行，负责最早期的挂载和环境初始化。
- **Second Stage Init**（`init.cpp` 的 `SecondStageMain`）：从 ramdisk 切换到系统分区后执行，负责解析 rc 文件和启动系统服务。

---

## 1. 启动总体流程概览

1. **内核启动**：设备加电后，Bootloader 加载 Linux 内核，内核完成硬件初始化。
2. **First Stage Init（ramdisk 中）**：
   - 内核启动后，执行 ramdisk 中的 `/init`（PID 1）。
   - 挂载基础伪文件系统（`/proc`、`/sys`、`/dev`）。
   - 初始化 SELinux 策略。
   - 执行 `first_stage_mount` 挂载系统分区（基于 fstab.${ro.hardware}）。
3. **Second Stage Init（系统分区）**：
   - 从 ramdisk 中的 init 切换到系统分区的 init。
   - 初始化属性系统（Property Service）。
   - 解析 `.rc` 文件，构建 action/service/trigger 列表。
   - 按顺序执行各阶段的 action（early-init、init、on fs、post-fs、post-fs-data、late-init 等）。
4. **Framework 启动**：
   - init 启动 `zygote` 进程，zygote fork 出 `system_server`。
   - `system_server` 启动 Android Framework 的各项系统服务。
   - 最后设置 `sys.boot_completed=1`，触发最终初始化动作。

---

## 2. init 的分阶段与执行流程

### 2.1 First Stage Init 阶段（`FirstStageMain` 函数）

**源码位置**：`first_stage_init.cpp:174` 的 `FirstStageMain()`

**主要工作**：
1. **清理环境**：清空环境变量，设置 PATH。
2. **挂载基础伪文件系统**：
   ```cpp
   mount("tmpfs", "/dev", "tmpfs", MS_NOSUID, "mode=0755");
   mount("devpts", "/dev/pts", "devpts", 0, NULL);
   mount("proc", "/proc", "proc", 0, "hidepid=2,gid=AID_READPROC");
   mount("sysfs", "/sys", "sysfs", 0, NULL);
   mount("selinuxfs", "/sys/fs/selinux", "selinuxfs", 0, NULL);
   ```
3. **创建设备节点**：`/dev/kmsg`、`/dev/random`、`/dev/urandom`、`/dev/null`、`/dev/ptmx` 等。
4. **初始化 SELinux**：加载 SELinux 策略文件。
5. **加载内核模块**（如需要）：从 `/lib/modules` 加载。
6. **挂载 tmpfs**：为 `/mnt`、`/debug_ramdisk` 挂载临时文件系统。
7. **执行 first_stage_mount**：调用 `first_stage_mount.cpp` 中的挂载逻辑，根据 `fstab.${ro.hardware}` 挂载系统分区。
8. **切换根目录**（if needed）：完成 ramdisk 到系统的切换。
9. **执行 second stage init**：使用 `execv` 执行系统分区中的 init，进入 SecondStageMain。

---

### 2.2 early-init 阶段

**触发时机**：Second Stage Init 开始后，最早执行的 action。

**主要工作**：
- 再次确认基础文件系统挂载。
- 初始化 cgroup（进程分组与资源控制）。
- 创建关键系统目录。
- 设置系统级别的属性。

**典型 rc 文件中的操作**：
```
on early-init
    mkdir /dev/pts
    mkdir /dev/socket
    mount devpts devpts /dev/pts
    chmod 0666 /dev/ptmx
```

---

### 2.3 init 阶段

**触发时机**：early-init 完成后立即执行。

**主要工作**：
1. **启动早期守护进程**：
   - `ueventd`：设备节点管理，监听内核 uevent 事件，创建和权限设置 `/dev` 节点。
   - `logd`：日志守护程序。
   - `healthd`：电池和系统健康监控。
2. **解析所有 `.rc` 文件**（在 `LoadBootScripts` 中）：
   ```cpp
   // 源码位置：init.cpp:272
   parser.ParseConfig("/system/etc/init/hw/init.rc");
   parser.ParseConfig("/system/etc/init");
   parser.ParseConfig("/system_ext/etc/init");
   parser.ParseConfig("/product/etc/init");
   parser.ParseConfig("/odm/etc/init");
   parser.ParseConfig("/vendor/etc/init");
   ```
3. **启动 servicemanager 和 hwservicemanager**：这两个是 Binder 通信的核心基础设施。

**典型服务**：
- `ueventd`
- `logd`
- `healthd`
- `servicemanager`
- `hwservicemanager`

---

### 2.4 on fs 阶段

**触发时机**：init 阶段完成后。

**主要工作**：
- 首次挂载系统关键分区（通常是 `/system`）。
- 触发文件系统检查（fsck）。
- 为后续 post-fs 做准备。

**典型 rc 文件中的操作**：
```
on fs
    mount_all /vendor/etc/fstab.${ro.hardware}
    swapon_all /vendor/etc/fstab.${ro.hardware}
```

---

### 2.5 post-fs 阶段

**触发时机**：on fs 完成后。

**主要工作**：
1. **继续挂载系统分区**：
   - `/vendor`、`/odm`、`/product` 等分区。
2. **执行权限恢复**：
   - 恢复挂载分区的 SELinux 上下文（`restorecon`）。
3. **启动依赖这些分区的服务**。

**典型 rc 文件中的操作**：
```
on post-fs
    mount_all --late
    restorecon_recursive /system
    restorecon_recursive /vendor
```

---

### 2.6 post-fs-data 阶段

**触发时机**：post-fs 完成后。

**主要工作**：
1. **挂载 `/data` 分区**（用户数据分区）。
2. **启动依赖 `/data` 的服务**：
   - `vold`：存储管理守护程序，处理存储设备挂载。
3. **恢复 `/data` 的 SELinux 上下文**。
4. **执行数据相关任务**：
   - 恢复用户数据。
   - dex 优化（如需要）。
   - 应用数据迁移。

**典型 rc 文件中的操作**：
```
on post-fs-data
    mkdir /data/misc_de
    restorecon_recursive /data
    start vold
    start adbd
```

**关键服务**：
- `vold`（存储管理）
- `adbd`（ADB 守护，如启用）

---

### 2.7 late-init 阶段

**触发时机**：post-fs-data 完成，系统已基本就绪。

**主要工作**：
1. **启动 Android 运行时**：
   - `zygote` / `zygote64`：Java 虚拟机启动点。
2. **启动系统图形服务**：
   - `surfaceflinger`：图形合成服务（通常由 `system_server` 启动）。
3. **启动网络相关服务**：
   - `netd`：网络守护程序。
4. **启动厂商/硬件相关服务**：
   - 蓝牙、GPS、相机 HAL 等。
5. **启动开机动画**（`bootanim`）。

**典型 rc 文件中的操作**：
```
on late-init
    start zygote
    start zygote64
    start netd
    class_start main
```

**关键服务**：
- `zygote` / `zygote64`
- `netd`
- `bootanim`
- 各种 HAL 服务

---

### 2.8 property 驱动的最终化阶段

**触发时机**：Framework 初始化完成，`sys.boot_completed=1` 被设置。

**主要工作**：
1. **停止开机动画**（`bootanim`）。
2. **启动依赖引导完成的服务**：
   - 桌面 launcher 相关服务。
   - 定时任务。
3. **运行启动完成的脚本**。

**典型 rc 文件中的操作**：
```
on property:sys.boot_completed=1
    stop bootanim
    start other_services
```

---

### 2.9 持续监控与管理阶段

**触发时机**：整个系统运行期间。

**主要工作**：
1. **服务监控**（源码位置：`service.cpp`）：
   - init 监控所有子进程状态。
   - 服务异常退出时，根据 `restart` 属性自动重启。
2. **属性变化监听**（源码位置：`property_service.cpp`）：
   - 监听 `setprop` 命令。
   - 根据属性变化触发对应的 action。
3. **信号处理**：
   - 响应 `SIGTERM`、`SIGKILL` 等信号。
4. **关机/重启管理**：
   - 响应 `on shutdown`、`on reboot` 等触发器。

---

## 3. 分区挂载详细流程

### 3.1 挂载时序

| 阶段           | 挂载的分区        | 完成时机     |
|----------------|------------------|------------|
| First Stage    | ramdisk 切换     | first_stage_mount |
| on fs          | `/system`        | on fs 完成 |
| post-fs        | `/vendor`、`/odm`、`/product` | post-fs 完成 |
| post-fs-data   | `/data`          | post-fs-data 完成 |

### 3.2 挂载机制

- **首次挂载** (`on fs`)：基于 `fstab` 文件挂载系统分区，通常在这一阶段完成文件系统检查。
- **延后挂载** (`post-fs`)：在 init 阶段启动 `ueventd` 和其他依赖项后，挂载额外分区。
- **数据分区挂载** (`post-fs-data`)：由 `vold` 管理，支持加密和其他高级特性。

---

## 4. init 的触发器与监控行为

### 4.1 触发器机制（源码位置：`action.cpp`、`action_manager.cpp`）

**触发器类型**：
- `on <event>`：由内核事件、文件系统挂载、属性变化等驱动。
- 常见触发器：`on early-init`、`on init`、`on fs`、`on post-fs`、`on post-fs-data`、`on late-init`。
- 属性触发：`on property:<name>=<value>`（如 `on property:sys.boot_completed=1`）。

**触发器执行流程**（源码位置：`action_manager.cpp` 的 `ExecuteOneCommand`）：
```cpp
// 伪代码，实际流程
for (const auto& action : am.actions_) {
    if (action.trigger_matches(current_trigger)) {
        for (const auto& command : action.commands_) {
            command.Execute();
        }
    }
}
```

### 4.2 服务监控与重启策略（源码位置：`service.cpp`）

**服务定义格式**：
```
service <name> <path> [<args>]
    class <classname>
    restart <policy>
    oneshot
    disabled
```

**重启策略**：
- `restart`：默认，服务崩溃时自动重启。
- `oneshot`：仅启动一次，不自动重启。
- `disabled`：手动启动或通过属性控制启动。

**监控实现**：
- init 使用 `sigchld` 信号处理程序监控子进程。
- 进程退出时，根据策略决定是否重启。

### 4.3 属性驱动的动态管理（源码位置：`property_service.cpp`）

**属性系统初始化**（源码位置：`SecondStageMain` 中的 `PropertyInit`）：
- 从 `/system/build.prop`、`/vendor/build.prop` 等文件加载属性。
- 启动 property service 监听 Unix domain socket。

**属性变化驱动的动作**：
- `setprop sys.boot_completed 1` 触发 `on property:sys.boot_completed=1` 动作。
- `setprop persist.service.adb.enable 1` 可触发 ADB 启动。

**关键属性**：
- `ro.boot.serialno`：序列号。
- `ro.hardware`：硬件标识，用于加载设备特定的 rc 文件。
- `sys.boot_completed`：标记启动完成。
- `persist.service.adb.enable`：ADB 启用状态。

### 4.4 文件系统与事件驱动

**ueventd 的作用**（源码位置：`ueventd.cpp`）：
- 监听内核 uevent 事件（通过 netlink socket）。
- 根据 `ueventd.rc` 规则动态创建设备节点。
- 设置设备节点权限和 SELinux 上下文。

**vold 的作用**（存储管理）：
- 监听 uevent 事件，检测存储设备热插拔。
- 负责存储设备挂载、卸载、加密等。

### 4.5 关机与重启管理

**关机流程**：
```
on shutdown
    class_stop shutdown
    exec /system/bin/sync
    reboot
```

**源码位置**：`init.cpp` 中的 `shutdown_state` 和 `TriggerShutdown` 函数。

---

## 5. 版本与厂商差异

### 5.1 rc 文件位置（Android 11）

按加载顺序：
1. `/system/etc/init/hw/init.rc`（系统基础）
2. `/system/etc/init/*.rc`（系统扩展）
3. `/system_ext/etc/init/*.rc`（系统扩展分区）
4. `/product/etc/init/*.rc`（产品分区）
5. `/odm/etc/init/*.rc`（ODM 分区）
6. `/vendor/etc/init/*.rc`（厂商分区）

### 5.2 Service Class 的引入

从 Android 8 开始，引入 `class` 概念：
- `class core`：关键系统服务，优先启动。
- `class main`：普通系统服务。
- `class late_start`：延后启动的服务。

通过 `class_start main` 可批量启动某一类服务。

### 5.3 首次启动 vs 升级启动

- **首次启动**（`FirstBoot`）：需要 dex 优化、应用数据初始化等。
- **升级启动**（`FactoryReset`）：恢复出厂设置或升级后的启动。

---

## 6. 关键源文件说明

| 文件 | 功能 |
|------|------|
| `first_stage_main.cpp` | First Stage Init 入口 |
| `first_stage_init.cpp` | FirstStageMain 函数实现 |
| `first_stage_mount.cpp` | First Stage 挂载逻辑 |
| `init.cpp` | Second Stage Init，SecondStageMain 函数 |
| `action.cpp`/`action.h` | Action 类定义和执行 |
| `action_manager.cpp` | Action 管理和触发 |
| `service.cpp`/`service.h` | Service 类定义和监控 |
| `property_service.cpp` | 属性系统实现 |
| `ueventd.cpp` | 设备节点管理 |
| `builtins.cpp` | 内建命令实现（mount、start、setprop 等） |

---

## 7. 典型启动时序（从源码视角）

```
[内核] →
  ↓
[First Stage Init: first_stage_main()]
  ├─ 挂载 /proc, /sys, /dev 等伪文件系统
  ├─ 初始化 SELinux
  ├─ 加载内核模块
  ├─ 执行 first_stage_mount
  │   └─ 根据 fstab 挂载系统分区
  └─ execv() → Second Stage Init
  ↓
[Second Stage Init: SecondStageMain()]
  ├─ PropertyInit() 初始化属性系统
  ├─ LoadBootScripts() 解析所有 .rc 文件
  ├─ ExecuteOneCommand() 执行命令
  │   ├─ on early-init
  │   ├─ on init
  │   │   └─ start ueventd, logd, healthd
  │   ├─ on fs
  │   │   └─ mount_all (挂载 /system)
  │   ├─ on post-fs
  │   │   └─ mount_all --late (挂载 /vendor, /odm)
  │   ├─ on post-fs-data
  │   │   └─ start vold, adbd
  │   ├─ on late-init
  │   │   └─ start zygote, zygote64, netd, bootanim
  │   └─ on property:sys.boot_completed=1
  │       └─ stop bootanim
  └─ 持续监控服务状态，响应属性变化
      ↓
[Zygote & Framework]
  ├─ Fork system_server
  ├─ system_server 启动各系统服务
  └─ setprop sys.boot_completed 1
```

---

## 8. 常见调试方法（基于源码）

1. **查看 init 启动日志**：
   ```bash
   adb shell dmesg | grep init
   adb shell cat /dev/kmsg | grep init
   ```

2. **查看 rc 文件解析结果**：
   ```bash
   adb shell getprop | grep -E "init|boot"
   ```

3. **监控服务状态**：
   ```bash
   adb shell ps -A | grep -E "ueventd|logd|zygote|system_server"
   ```

4. **手动触发 action**：
   ```bash
   adb shell setprop sys.usb.config adb
   adb shell setprop sys.boot_completed 1
   ```

5. **分析启动性能**：
   ```bash
   adb shell cat /proc/bootprof  # 启动耗时统计
   ```

---

## 9. 实用建议

- 理解 First Stage 和 Second Stage 的区别对排查启动问题至关重要。
- init 的核心就是 action、service 和 trigger 的管理，理解这三个核心概念有助于快速定位问题。
- 属性系统是整个启动流程的"神经系统"，很多服务的启动都由属性变化驱动。
- 分区挂载涉及 fstab、SELinux、加密等多个环节，任何一个环节出错都会阻止启动。
