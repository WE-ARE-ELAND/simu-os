# SimuOS 毛玻璃半透明终端配置

## 原理

毛玻璃（Acrylic / Blur）和透明度是**终端模拟器**的功能，不是 simu-os 程序本身的能力。
Windows Terminal 内置了对这两者的原生支持。配置完成后，simu-os **零改动**即可运行在毛玻璃窗口中。

## 一、安装 Windows Terminal

微软商店搜索 "Windows Terminal" 或访问 https://aka.ms/terminal

## 二、一键应用配置

双击运行项目根目录下的 `install-glass.ps1` 脚本，即可自动：
1. 将 SimuOS 注册为 Windows Terminal 的一个 profile
2. 开启 Acrylic（毛玻璃）+ 60% 透明度
3. 设置深色主题 + 等宽字体

```powershell
cd E:\WorkSpace\Programmer\simu-os
powershell -ExecutionPolicy Bypass -File install-glass.ps1
```

## 三、手动配置（不想跑脚本的话）

### 1. 打开 Windows Terminal 设置

- 按 `Ctrl + ,` 打开 JSON 设置文件
- 或者菜单 → 设置 → 打开 JSON 文件

### 2. 在 `profiles.list` 数组里添加 SimuOS profile

```json
{
    "guid": "{simu-os-glass}",
    "name": "SimuOS",
    "commandline": "cmd /c \"E:\\WorkSpace\\Programmer\\simu-os\\build\\launch\"",
    "hidden": false,
    "acrylicOpacity": 0.6,
    "useAcrylic": true,
    "colorScheme": "One Half Dark",
    "fontFace": "Cascadia Code",
    "fontSize": 12
}
```

### 3. 在 `themes` 里添加 SimuOS 主题（可选，控制窗口的圆角/阴影）

```json
"themes": {
    "SimuOS": {
        "windowRadius": 8,
        "backgroundColor": "#0f1117"
    }
}
```

## 四、配置参数速查

| 参数 | 值 | 效果 |
|------|----|------|
| `useAcrylic` | `true` | 开启毛玻璃模糊 |
| `acrylicOpacity` | `0.0 ~ 1.0` | 透明度（0=完全透明，1=不透明） |
| `colorScheme` | 方案名 | 配色方案 |
| `fontFace` | 字体名 | 终端字体 |
| `fontSize` | 数字 | 字号 |
| `suppressApplicationTitle` | `true` | 隐藏窗口标题栏上的应用名 |

## 五、效果预览

```
┌─────────────────────────────────────────────────┐
│  (半透明 + 模糊背景)                            │
│  [admin@simuos root]$ pwd                       │
│  root                                           │
│  [admin@simuos root]$ ls                        │
│  admin        testdir                           │
│  [admin@simuos root]$ df                        │
│  Filesystem     Size   Used   Avail  Use% Mount │
│  simu-disk       35K     0B     35K    0%   /  │
│  [admin@simuos root]$ ___                        │
└─────────────────────────────────────────────────┘
```

**实际效果**：窗口背景会模糊显示你桌面/其他窗口的内容，simu-os 的彩色提示符浮在上面，
类似 macOS Terminal 的透明效果。

## 六、推荐配色方案

| 方案 | 特点 |
|------|------|
| `One Half Dark` | 深色 + 高对比度，推荐搭配毛玻璃 |
| `Nord` | 冷色调蓝灰，配合模糊效果很干净 |
| `Dracula` | 紫红主题，视觉冲击力强 |
| `Solarized Dark` | 经典低饱和度，长时间使用不累 |

配色方案在 Windows Terminal 官方 GitHub 可下载：
https://github.com/microsoft/terminal/tree/main/src/color-schemes

## 七、常见问题

**Q: 我的 Windows Terminal 没有毛玻璃选项？**
A: Windows Terminal 1.8+ 才支持 Acrylic。去微软商店更新到最新版。

**Q: 在普通 cmd.exe / PowerShell 窗口里能实现吗？**
A: 不能。这两个内置终端不支持毛玻璃。必须用 Windows Terminal、ConEmu、WezTerm 等现代终端。

**Q: 想调透明度但不想动脚本？**
A: Windows Terminal 打开 SimuOS profile → 右上角下拉菜单 → 选择你的 profile → 设置 → 调整 "透明度" 滑块即可实时预览。