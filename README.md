<div align="center">
    <img src="https://raw.githubusercontent.com/Sorok-Dva/ScreenMe/main/resources/icon.png" alt="ScreenMe Logo">
  <h1>ScreenMe</h1>
  <blockquote>Your Ultimate Tool for Effortless Screenshots.</blockquote>
  <img src="https://hits.dwyl.com/Sorok-Dva/ScreenMe.svg?style=flat-square" alt="Views"><br />
  <a href="https://github.com/Sorok-Dva/ScreenMe/releases/latest">
    <img src="https://img.shields.io/github/license/Sorok-Dva/ScreenMe?style=for-the-badge" alt="Licence MIT">
  </a>
  <a href="https://github.com/Sorok-Dva/ScreenMe/releases">
    <img src="https://img.shields.io/github/downloads/Sorok-Dva/ScreenMe/total.svg?style=for-the-badge" alt="Total downloads">
  </a>
  <a href="https://github.com/Sorok-Dva/ScreenMe/pulse" alt="Activity">
    <img src="https://img.shields.io/github/commit-activity/m/Sorok-Dva/ScreenMe.svg?style=for-the-badge" />
  </a>
  <a href="https://github.com/Sorok-Dva/ScreenMe/releases/latest">
    <img src="https://img.shields.io/github/release/Sorok-Dva/ScreenMe.svg?style=for-the-badge" alt="Latest Release">
  </a>
  <br />
  <a href="https://github.com/sponsors/Sorok-Dva">
    <img src="https://img.shields.io/badge/sponsor-30363D?style=for-the-badge&logo=GitHub-Sponsors&logoColor=#EA4AAA" alt="Sponsor Me">
  </a>
  <a href="https://patreon.com/sorokdva">
    <img src="https://img.shields.io/badge/Patreon-F96854?style=for-the-badge&logo=patreon&logoColor=white" alt="Support Me on Patreon">
  </a>
</div>

# ScreenMe – Build & Deploy Guide

ScreenMe is a Qt-based screenshot tool with a floating annotation editor, multi-monitor selection, and optional cloud sync.

---

## 1. Develop & Build

### 1.1 Clone
```bash
git clone https://github.com/Sorok-Dva/ScreenMe.git
cd ScreenMe
```

### 1.2 Qt Requirements
- Qt 5.15+ or Qt 6.5+ (Widgets, Network, WebSockets)
- C++17-capable compiler
- Optional but recommended: Qt Creator

#### Windows
Install Qt (MinGW or MSVC) with the Qt Maintenance Tool. Add CMake/Ninja if you prefer a CMake workflow.

#### macOS
Install via Homebrew:
```bash
brew install qt
```
Set `QT_HOME=/usr/local/opt/qt` (or `/opt/homebrew/opt/qt` on Apple Silicon) and ensure `qmake`/`cmake` are on `PATH`.

### 1.3 Build with qmake (default project)
```bash
qmake ScreenMe.pro
make          # or `nmake` on MSVC, `jom` on Windows with Qt tools
```
Outputs land in `./release` (MSVC) or the local directory (Unix).

### 1.4 Build with CMake (optional)
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="${QT_HOME}"
cmake --build build --config Release
```
> If Qt was installed via the official installer, point `CMAKE_PREFIX_PATH` to `<Qt>/5.15.2/msvc2019_64` (or equivalent).

---

## 2. Platform Notes

### 2.1 macOS
- `ScreenMe.pro` now sets `CONFIG += app_bundle` and links Carbon/ApplicationServices. Running `make` produces `ScreenMe.app`.
- Screenshots use the macOS capture API via Qt’s `QScreen::grabWindow`. Hotkeys rely on Qt’s event filters (no Carbon shortcut capture yet).
- Auto-start is disabled by default. To enable login launch: copy `ScreenMe.app` to `/Applications`, run `osx/set-login-item.sh` (to be delivered) or use `launchctl` manually.

### 2.2 Windows
- System tray integration, global hotkeys, and auto-start run natively.
- `config.json` lives in `%APPDATA%/ScreenMe/` by default (Qt `AppDataLocation`).
- Build generates `ScreenMe.exe`; use `windeployqt ScreenMe.exe` (from the Qt bin folder) to gather dependent Qt DLLs.

---

## 3. Packaging & Deployment

### 3.1 Windows Installer (MSI/EXE)
1. Build Release (`nmake release` or Visual Studio).
2. Deploy Qt DLLs: `windeployqt --release ScreenMe.exe`.
3. Bundle resources: ensure `resources/` and `icons/` are copied next to the EXE. If using an installer (NSIS/InnoSetup/WiX), include:
   - `ScreenMe.exe`
   - `Qt5Core.dll`, `Qt5Gui.dll`, `Qt5Widgets.dll`, `Qt5Network.dll`, `Qt5WebSockets.dll`
   - `platforms/qwindows.dll`
   - `imageformats/` plug-ins if PNG/JPEG support is missing (usually automatic)

4. Optional: Sign the executable, build an MSI via WiX Toolset.

### 3.2 macOS Bundle (.app)
1. Build Release (`qmake`, `make`).
2. Run `macdeployqt ScreenMe.app` to embedded frameworks/plugins.
3. Sign and notarize (optional but recommended for Gatekeeper).
4. Zip the `.app` or create a `.dmg` (e.g., `hdiutil create ScreenMe.dmg -srcfolder ScreenMe.app`).

---

## 4. Configuration & Assets

- `resources/config.json`: default settings for save path, image quality, etc.
- `icons.qrc` bundl es toolbar icons and the app icon.
- Login info persists in `login_info.json` (Qt `AppDataLocation`).

---

## 5. Key Features Recap
- Multi-screen aware area selection & editor overlay
- Tailwind-inspired floating editor with tooltip-only action buttons
- Windows global hotkeys (macOS uses inline key capture)
- Uploads to `https://screen.sorokdva.eu`

---

## 6. Troubleshooting

| Issue | Fix |
|-------|-----|
| `Qt5*.dll` missing at runtime (Win) | Run `windeployqt ScreenMe.exe` |
| Bundle won’t launch on macOS | Ensure `macdeployqt ScreenMe.app` ran; check Gatekeeper logs |
| Hotkeys don’t fire on macOS | Hotkeys fall back to Qt key events. Ensure the options dialog has focus when recording |
| Multi-monitor capture off-by-one | Qt 5.9 or lower may lack detected scaling. Upgrade to Qt 5.12+ |

---

## 7. Contributing & Support

Contributions welcome! Fork, branch, PR as usual. Bug reports via [GitHub Issues](https://github.com/Sorok-Dva/ScreenMe/issues). For commercial support, open a ticket or reach out to the maintainer.

Happy capturing ✨
