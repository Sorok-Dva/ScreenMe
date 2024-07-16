#include "include/editor_window.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QKeyEvent>

EditorWindow::EditorWindow(const QPixmap& screenshot, QWidget* parent) : QMainWindow(parent), screenshot(screenshot) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowOpacity(1.0);
    setAttribute(Qt::WA_QuitOnClose, false);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QLabel* label = new QLabel(centralWidget);
    label->setPixmap(screenshot);
    layout->addWidget(label);

    setCentralWidget(centralWidget);
}

void EditorWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    else if (event->key() == Qt::Key_S) {
        saveScreenshot();
    }
}

void EditorWindow::saveScreenshot() {
    QString savePath = QFileDialog::getSaveFileName(this, "Save Screenshot", "", "Images (*.png *.jpg)");
    if (!savePath.isEmpty()) {
        screenshot.save(savePath);
    }
}
