<div align="center">
    <img src="https://raw.githubusercontent.com/Sorok-Dva/ScreenMe/main/resources/icon.png" alt="ScreenMe Logo">
  <h1>ScreenMe</h1>
  <blockquote>Your Ultimate Tool for Effortless Screenshots.</blockquote>
  <a href="https://github.com/Sorok-Dva/ScreenMe/releases/latest">
    <img src="https://img.shields.io/github/license/Sorok-Dva/ScreenMe?style=for-the-badge" alt="Licence MIT">
  </a>
  <a href="https://github.com/Sorok-Dva/ScreenMe/releases">
    <img src="https://img.shields.io/github/downloads/Sorok-Dva/ScreenMe/total.svg?style=for-the-badge" alt="Total downloads">
  </a>
  <!--<a href="https://shields.io/community#sponsors" alt="Sponsors">
    <img src="https://img.shields.io/opencollective/sponsors/Sorok-Dva.svg?style=for-the-badge" />
  </a>-->
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
  <a href="https://github.com/sponsors/Sorok-Dva">
    <img src="https://img.shields.io/badge/Patreon-F96854?style=for-the-badge&logo=patreon&logoColor=white" alt="Support Me on Patreon">
  </a>


</div>

# ScreenMe - Screenshot Tool

ScreenMe is a simple screenshot tool built with C++ and Qt. It allows users to take screenshots and save them with auto-incremented filenames. The tool includes a system tray icon for easy access to the screenshot functionality and configurable hotkeys.
ScreenMe is designed for capturing, managing, and sharing screenshots efficiently. The application provides a range of functionalities to enhance the screenshot experience, from hotkey configurations to a built-in editor for annotating screenshots.

A website is available to register user captures online: [ScreenMe Website](https://screen-me.cloud).

### Why ScreenMe?
ScreenMe was created because of security concerns with Lightshot. There have been security vulnerabilities in Lightshot that allow anyone to view screenshots from other users. ScreenMe addresses this issue with robust privacy settings that ensure only you control who sees your screenshots.

Additionally, Lightshot has issues with Facebook login, resulting in users losing access to their galleries. ScreenMe offers reliable tools to migrate your screenshots from Lightshot to ScreenMe.

## Features

- Capture full screen or selected area screenshots
- Annotate screenshots with text, shapes, and drawing tools
- Save screenshots in multiple formats (PNG, JPEG)
- Hotkey support for quick access to screenshot functionalities
- System tray integration for easy access
- Online synchronization of screenshots (requires login)
- Configurable options for image quality and file saving

## Installation

### Prerequisites

- Qt 5.12 or later
- CMake 3.10 or later

### Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/Sorok-Dva/ScreenMe.git
    cd ScreenMe
    ```

2. Open the project in Qt Creator or your preferred IDE.

3. Build and run the project.

## Usage

### Starting the Application

Launch the application by running the `ScreenMe` executable. The application will minimize to the system tray, where you can access its functionalities by right-clicking the tray icon.

### Taking Screenshots

- **Take Screenshot**: Use the configured hotkey to capture a screenshot of the selected area.
- **Take Fullscreen Screenshot**: Use the configured hotkey to capture a fullscreen screenshot.

### Editing Screenshots

After capturing a screenshot, the built-in editor allows you to:
- Draw shapes (rectangle, ellipse, line, arrow)
- Add text
- Undo and redo actions
- Save or copy the edited screenshot

### Options

Access the options window through the system tray menu to configure:
- Hotkeys for taking screenshots
- Default save folder
- Image quality and file format

### Online Synchronization

To upload and synchronize your screenshots online:
1. Log in through the system tray menu.
2. After logging in, your screenshots will be uploaded automatically.
3. Visit [ScreenMe Website](https://screen-me.cloud) to view and manage your screenshots.

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

## Acknowledgments

- Developed by [Сорок два](https://github.com/Sorok-Dva). All rights reserved.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributors

<a href="https://github.com/sorok-dva/screenMe/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=sorok-dva/screenMe" />
</a>

## Contact

For any inquiries or feedback, please visit our [GitHub Repository](https://github.com/Sorok-Dva/ScreenMe) or contact the developers directly.

