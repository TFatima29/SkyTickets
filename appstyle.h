#ifndef APPSTYLE_H
#define APPSTYLE_H
#include <QString>
// Централизованные QSS-стили — Aviasales-inspired
namespace AppStyle {
inline QString mainStyle(){
    return R"(
QWidget { font-family: 'Segoe UI', Arial, sans-serif; font-size: 13px; background:#F0F4F8; }
QMainWindow { background:#F0F4F8; }
/* === КНОПКИ === */
QPushButton {
    background:#FF6D00; color:white; border:none;
    border-radius:6px; padding:8px 18px; font-weight:bold; font-size:13px;
}
QPushButton:hover  { background:#E65100; }
QPushButton:pressed{ background:#BF360C; }
QPushButton#btnSecondary {
    background:#1565C0; color:white;
}
QPushButton#btnSecondary:hover { background:#0D47A1; }
QPushButton#btnDanger {
    background:#C62828; color:white;
}
QPushButton#btnDanger:hover { background:#B71C1C; }
QPushButton#btnGreen {
    background:#2E7D32; color:white;
}
QPushButton#btnGreen:hover { background:#1B5E20; }
QPushButton#btnGhost {
    background:transparent; color:#1565C0;
    border:2px solid #1565C0; border-radius:6px;
}
QPushButton#btnGhost:hover { background:#E3F2FD; }
/* === ПОЛЯ ВВОДА === */
QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QDateEdit, QDateTimeEdit {
    background:white; border:1.5px solid #BDBDBD;
    border-radius:6px; padding:6px 10px; font-size:13px; color:#212121;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus,
QComboBox:focus, QDateEdit:focus, QDateTimeEdit:focus {
    border-color:#1565C0;
}
QTextEdit { background:white; border:1.5px solid #BDBDBD; border-radius:6px; padding:6px; }
/* === ТАБЛИЦЫ === */
QTableWidget {
    background:white; border:none; gridline-color:#E0E0E0;
    border-radius:8px; selection-background-color:#E3F2FD;
}
QTableWidget::item { padding:8px; border-bottom:1px solid #F5F5F5; }
QTableWidget::item:selected { background:#BBDEFB; color:#0D47A1; }
QHeaderView::section {
    background:#1565C0; color:white; font-weight:bold;
    padding:9px; border:none; font-size:12px;
}
/* === ВКЛАДКИ === */
QTabWidget::pane { border:none; background:#F0F4F8; }
QTabBar::tab {
    background:#E3F2FD; color:#1565C0; padding:10px 20px;
    border-radius:6px 6px 0 0; margin-right:2px; font-weight:bold;
}
QTabBar::tab:selected { background:#1565C0; color:white; }
QTabBar::tab:hover:!selected { background:#BBDEFB; }
/* === СКРОЛЛБАР === */
QScrollBar:vertical { background:#F0F4F8; width:8px; border-radius:4px; }
QScrollBar::handle:vertical { background:#BDBDBD; border-radius:4px; }
QScrollBar::handle:vertical:hover { background:#9E9E9E; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
/* === КАРТОЧКИ === */
QFrame#card {
    background:white; border-radius:12px;
    border:1px solid #E0E0E0;
}
QFrame#cardHighlight {
    background:white; border-radius:12px;
    border:2px solid #1565C0;
}
/* === МЕТКИ === */
QLabel#lblTitle    { color:#212121; font-size:18px; font-weight:bold; }
QLabel#lblSubtitle { color:#757575; font-size:13px; }
QLabel#lblPrice    { color:#FF6D00; font-size:20px; font-weight:bold; }
QLabel#lblBadge    { background:#E3F2FD; color:#1565C0; border-radius:10px; padding:3px 10px; font-size:11px; }
QLabel#lblBadgeGreen { background:#E8F5E9; color:#2E7D32; border-radius:10px; padding:3px 10px; font-size:11px; }
QLabel#lblBadgeOrange{ background:#FFF3E0; color:#E65100; border-radius:10px; padding:3px 10px; font-size:11px; }
/* === ГРУППЫ === */
QGroupBox {
    background:white; border:1px solid #E0E0E0; border-radius:10px;
    margin-top:14px; font-weight:bold; color:#1565C0; padding:10px;
}
QGroupBox::title { subcontrol-origin:margin; left:12px; padding:0 5px; background:white; }
/* === ДИАЛОГИ === */
QDialog { background:#F0F4F8; }
/* === СТАТУСБАР === */
QStatusBar { background:#1565C0; color:white; font-size:12px; }
)";
}
inline QString loginStyle(){
    return R"(
QWidget#loginBg {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
        stop:0 #0D47A1, stop:0.5 #1565C0, stop:1 #1976D2);
}
QFrame#loginCard {
    background:white; border-radius:16px;
}
QLabel#loginTitle { color:#0D47A1; font-size:24px; font-weight:bold; }
QLabel#loginSub   { color:#757575; font-size:13px; }
QLineEdit {
    background:#F5F5F5; border:1.5px solid #E0E0E0;
    border-radius:8px; padding:10px 14px; font-size:14px;
}
QLineEdit:focus { border-color:#1565C0; background:white; }
QPushButton#btnLogin {
    background:#FF6D00; color:white; font-size:15px; font-weight:bold;
    border:none; border-radius:8px; padding:12px;
}
QPushButton#btnLogin:hover { background:#E65100; }
QPushButton#btnRegLink {
    background:transparent; color:#1565C0; border:none;
    font-size:13px; text-decoration:underline;
}
QPushButton#btnRegLink:hover { color:#0D47A1; }
)";
}
}
#endif
