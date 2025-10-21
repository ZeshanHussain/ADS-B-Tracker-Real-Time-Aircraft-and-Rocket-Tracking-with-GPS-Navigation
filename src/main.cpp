#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    // Set application info BEFORE creating QApplication
    // This is important for GeoClue2 to recognize the app
    QCoreApplication::setApplicationName("AircraftRocketTracker");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName("AeroTracker");
    QCoreApplication::setOrganizationDomain("aerotracker.local");
    
    QApplication app(argc, argv);
    
    // Set Palantir-style dark tactical theme
    app.setStyle("Fusion");
    QPalette palantirPalette;
    
    // Deep blacks and grays - tactical interface
    palantirPalette.setColor(QPalette::Window, QColor(15, 18, 22));           // Very dark blue-black
    palantirPalette.setColor(QPalette::WindowText, QColor(220, 225, 230));    // Light gray text
    palantirPalette.setColor(QPalette::Base, QColor(10, 13, 17));             // Even darker base
    palantirPalette.setColor(QPalette::AlternateBase, QColor(20, 23, 27));    // Subtle contrast
    palantirPalette.setColor(QPalette::ToolTipBase, QColor(30, 35, 40));
    palantirPalette.setColor(QPalette::ToolTipText, QColor(220, 225, 230));
    palantirPalette.setColor(QPalette::Text, QColor(220, 225, 230));          // Readable light gray
    palantirPalette.setColor(QPalette::Button, QColor(25, 30, 35));           // Dark buttons
    palantirPalette.setColor(QPalette::ButtonText, QColor(220, 225, 230));
    palantirPalette.setColor(QPalette::BrightText, QColor(255, 82, 82));      // Alert red
    palantirPalette.setColor(QPalette::Link, QColor(0, 168, 255));            // Bright cyan
    palantirPalette.setColor(QPalette::Highlight, QColor(0, 168, 255));       // Cyan highlight
    palantirPalette.setColor(QPalette::HighlightedText, QColor(10, 13, 17));
    palantirPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(100, 105, 110));
    
    app.setPalette(palantirPalette);
    
    // Set custom stylesheet for enhanced Palantir look
    app.setStyleSheet(
        "QWidget { "
        "   font-family: 'Consolas', 'Monaco', 'Courier New', monospace; "
        "}"
        "QGroupBox { "
        "   border: 1px solid #2A3F5F; "
        "   border-radius: 4px; "
        "   margin-top: 8px; "
        "   padding-top: 8px; "
        "   font-weight: bold; "
        "   color: #00A8FF; "
        "}"
        "QGroupBox::title { "
        "   subcontrol-origin: margin; "
        "   left: 10px; "
        "   padding: 0 5px 0 5px; "
        "}"
        "QPushButton { "
        "   background-color: #1A2332; "
        "   border: 1px solid #2A3F5F; "
        "   border-radius: 3px; "
        "   padding: 6px 12px; "
        "   color: #DCE1E6; "
        "   min-height: 24px; "
        "}"
        "QPushButton:hover { "
        "   background-color: #243447; "
        "   border: 1px solid #00A8FF; "
        "}"
        "QPushButton:pressed { "
        "   background-color: #0A0D11; "
        "}"
        "QLineEdit, QSpinBox { "
        "   background-color: #0A0D11; "
        "   border: 1px solid #2A3F5F; "
        "   border-radius: 3px; "
        "   padding: 4px; "
        "   color: #DCE1E6; "
        "   selection-background-color: #00A8FF; "
        "}"
        "QLineEdit:focus, QSpinBox:focus { "
        "   border: 1px solid #00A8FF; "
        "}"
        "QComboBox { "
        "   background-color: #1A2332; "
        "   border: 1px solid #2A3F5F; "
        "   border-radius: 3px; "
        "   padding: 4px; "
        "   color: #DCE1E6; "
        "}"
        "QComboBox:hover { "
        "   border: 1px solid #00A8FF; "
        "}"
        "QComboBox::drop-down { "
        "   border: none; "
        "   width: 20px; "
        "}"
        "QComboBox QAbstractItemView { "
        "   background-color: #1A2332; "
        "   border: 1px solid #2A3F5F; "
        "   selection-background-color: #00A8FF; "
        "   color: #DCE1E6; "
        "}"
        "QListWidget { "
        "   background-color: #0A0D11; "
        "   border: 1px solid #2A3F5F; "
        "   border-radius: 3px; "
        "   color: #DCE1E6; "
        "   font-family: 'Consolas', 'Monaco', monospace; "
        "   font-size: 11px; "
        "}"
        "QListWidget::item { "
        "   padding: 4px; "
        "   border-bottom: 1px solid #1A2332; "
        "}"
        "QListWidget::item:selected { "
        "   background-color: #2A3F5F; "
        "   color: #00A8FF; "
        "}"
        "QListWidget::item:hover { "
        "   background-color: #1A2332; "
        "}"
        "QStatusBar { "
        "   background-color: #0F1216; "
        "   border-top: 1px solid #2A3F5F; "
        "   color: #00A8FF; "
        "}"
        "QLabel { "
        "   color: #DCE1E6; "
        "}"
    );
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

