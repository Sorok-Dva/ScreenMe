# ScreenMe - Screenshot Tool

ScreenMe is a simple screenshot tool built with C++ and Qt. It allows users to take screenshots and save them with auto-incremented filenames. The tool includes a system tray icon for easy access to the screenshot functionality and configurable hotkeys.

## Features

- Take a screenshot of the active screen.
- Save screenshots.
- Configurable hotkeys for taking screenshots.
- System tray icon for easy access.

## Installation

### Prerequisites

- Qt framework
- C++ compiler

### Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/Sorok-Dva/ScreenMe.git
    cd ScreenMe
    ```

2. Open the project in Qt Creator or your preferred IDE.

3. Build and run the project.

## Usage

1. **System Tray Icon**:
    - Right-click the tray icon to access the options.
    - Left-click the tray icon to take a screenshot.

2. **Hotkeys**:
    - Configure hotkeys in the options menu.
    - Use the configured hotkeys to take screenshots.

## Configuration

The configuration file `config.json` is located in the `resources` directory. It includes the following settings:

- `screenshot_hotkey`: Hotkey for taking a screenshot.
- `fullscreen_hotkey`: Hotkey for taking a fullscreen screenshot.
- `file_extension`: Default file extension for screenshots (e.g., png, jpg).
- `image_quality`: Image quality for screenshots.
- `default_save_folder`: Default folder to save screenshots.
- `start_with_system`: Option to start the application with the system.

## Contributing

Contributions are welcome ! If you would like to contribute to this project, please follow these steps:

1. Fork the repository.
2. Create a new branch with your feature or bug fix.
3. Commit your changes.
4. Push to the branch.
5. Create a pull request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
