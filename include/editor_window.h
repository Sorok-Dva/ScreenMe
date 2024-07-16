#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>
#include <QPixmap>

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EditorWindow(const QPixmap& screenshot, QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void saveScreenshot();

private:
    QPixmap screenshot;
};

#endif // EDITOR_WINDOW_H
