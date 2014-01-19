/********************************************************************************
** Form generated from reading UI file 'additive.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDITIVE_H
#define UI_ADDITIVE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_additiveClass
{
public:
    QAction *actionNew_Window;
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionFull_Screen;
    QAction *actionAbout;
    QAction *actionPreview;
    QWidget *centralWidget;
    QPushButton *btnHello;
    QSlider *verticalSlider;
    QSlider *horizontalSlider;
    QTabWidget *tabWidget;
    QWidget *tab;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QWidget *tab_2;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QWidget *tab_3;
    QListWidget *listWidget;
    QLineEdit *lineEdit;
    QWidget *centralWidget2;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *additiveClass)
    {
        if (additiveClass->objectName().isEmpty())
            additiveClass->setObjectName(QString::fromUtf8("additiveClass"));
        additiveClass->resize(635, 590);
        additiveClass->setMouseTracking(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/additive/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        additiveClass->setWindowIcon(icon);
        additiveClass->setWindowOpacity(1);
        additiveClass->setStyleSheet(QString::fromUtf8("QWidget{\n"
"	color: #dddddd;\n"
"	font: 10pt \"Lucida Sans\";\n"
"	selection-background-color: #FEE728;\n"
"}\n"
"\n"
"#centralWidget, QLineEdit, QSlider, QDial {\n"
"background: #111111;\n"
"}\n"
"QMenuBar {\n"
"	background-color:#3C3C3C;\n"
"	color: #ffffff;\n"
"	selection-color: #000000;\n"
"\n"
"	font: 10pt \"Lucida Sans\";\n"
"	padding: 5px;\n"
"}\n"
"QMenuBar::item:selected { /* when selected using mouse or keyboard */\n"
"	background: #FEE728;\n"
"	color: #444444;\n"
" }\n"
"\n"
"QStatusBar {\n"
"	background-color: #111111;\n"
"	color: #000;\n"
"	padding: 5px;\n"
"}\n"
"\n"
"QMenu  {\n"
"\n"
"	font: 10pt \"Lucida Sans\";\n"
"	/*	gridline-color: rgb(167, 207, 255);*/\n"
"	selection-color: #444444;\n"
"	color: #ffffff;\n"
"	background-color: #222222;\n"
"\n"
"/*	border: ;/*#565048;*/\n"
"\n"
"	border-left: 1px solid #aaaaaa;\n"
"	border-right: 1px solid #aaaaaa;\n"
"	border-bottom: 1px solid #aaaaaa;\n"
"}\n"
"\n"
"\n"
"#centralWidget2 {\n"
"	border: 2px solid #D0592A;\n"
"}\n"
"\n"
"QPushButton {\n"
"\n"
""
                        "background: rgba(86,80,72,150);\n"
"\n"
"border-radius: 4px;\n"
"color: #ffffff;\n"
"\n"
"padding: 5px;\n"
"\n"
"/*background: rgba(85, 120, 10, 150);*/\n"
"border: none;/*1px solid #808080;*/\n"
"/*border: 1px solid black;*/\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"/*background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #bbb);*/\n"
"background: #82766A;/* #6A8B0F;*/\n"
"\n"
"\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"/*background: qradialgradient(cx: 0.4, cy: -0.1,\n"
"fx: 0.4, fy: -0.1,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #ddd);*/\n"
"background: #474037;\n"
"}\n"
"\n"
"QCheckBox, QRadioButton {	\n"
" \n"
"	border-color: rgb(255, 255, 255);\n"
"	alternate-background-color: rgb(120, 255, 96);\n"
"}\n"
"\n"
"/** tabs ***/\n"
"#tab, #tab_2, #tab_3, QListView  {\n"
"	background: rgba(0, 0, 0, 0);\n"
"	color: #dddddd;\n"
"}\n"
"\n"
" QTabWidget::pane { /* The tab widget frame */\n"
"	border: none;\n"
"	background: rgba(57, 60, 46, 255);\n"
" }\n"
"\n"
"QTab"
                        "Bar::tab {\n"
"\n"
"color: #dddddd;\n"
"border-top-left-radius: 10px;\n"
"border-bottom-right-radius: 10px;\n"
"border-top-right-radius: 10px;\n"
"\n"
"margin-right: 2px;\n"
"\n"
"/*background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #888);*/\n"
"\n"
"background: #524A44;\n"
"\n"
"     min-width: 8ex;\n"
"     /*padding: 5px;*/\n"
"	/*margin-right: 5px;*/\n"
"\n"
"	padding-right: 20px;\n"
"	padding-left: 20px;\n"
"	padding-top: 5px;\n"
"	padding-bottom: 5px;\n"
"}\n"
"\n"
"QTabBar::tab:hover\n"
"{\n"
"background: #CF7C2B;\n"
"color: #ffffff;\n"
"}\n"
"QTabBar::tab:selected  {\n"
"/*background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #ddd);*/\n"
"\n"
"background: rgba(208, 89, 42, 200);\n"
"\n"
"color: #ffffff;\n"
" }\n"
"\n"
"\n"
"QLineEdit {\n"
"color: black;\n"
"selection-color: black;\n"
"background: white;   \n"
"border: none;                     \n"
"} \n"
"\n"
" QCheckBox::indicator\n"
"  {"
                        "\n"
"	width: 18px;\n"
"	height: 18px;\n"
"  }\n"
" QCheckBox::indicator:checked\n"
"  {\n"
"	background: black;\n"
"/*	 border-left: 2px solid blue;*/\n"
"	/*image: url(:/additive/checked.gif);*/\n"
"  }\n"
"  QCheckBox::indicator:unchecked\n"
"  {\n"
"	background: white;\n"
"/*	 border: 2px solid white;*/\n"
"	/*image: url(:/additive/unchecked.gif);*/\n"
"  }\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"QScrollBar:vertical  {\n"
"\n"
"     background: #988C84;\n"
"     width: 15px;\n"
"     margin: 20px 0 20px 0;\n"
"/*	border-top: 1px solid white;\n"
"	border-bottom: 1px solid white;*/\n"
"\n"
" }\n"
" QScrollBar::handle:vertical  {\n"
"     background: #392C23;\n"
"	min-height: 10px;\n"
"\n"
" }\n"
" QScrollBar::add-line:vertical  {\n"
"\n"
"     background: #7A6354;\n"
"     height: 20px;\n"
"     subcontrol-position: bottom;\n"
"     subcontrol-origin: margin;\n"
" }\n"
" \n"
" QScrollBar::add-line:vertical:hover, QScrollBar::sub-line:vertical:hover   {\n"
"	background: #B09888;\n"
"\n"
"}\n"
"\n"
" QScrollBar::sub-line:"
                        "vertical  {\n"
"/*	border: 2px solid #ffffff;*/\n"
"\n"
"     background: #7A6354;\n"
"     height: 20px;\n"
"     subcontrol-position: top;\n"
"     subcontrol-origin: margin;\n"
" }\n"
" QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical  {\n"
"     width: 3px;\n"
"     height: 4px;\n"
"     background: #392C23;\n"
" }\n"
"\n"
" QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical  {\n"
"     background: none;\n"
"}\n"
"\n"
"QSlider::handle:horizontal, QSlider::handle:vertical {\n"
" 	background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 #FA7401, stop:1 #E39B51);\n"
"\n"
"background-color: rgba(86,80,72,200); /*#565048;*/\n"
"\n"
"	height: 1px;\n"
"	max-height: 1px;\n"
"/*	border-radius: 2px;*/\n"
"\n"
"/*	border: 1px solid #777777;*/\n"
"	border: 1px solid #524A44;\n"
"\n"
"\n"
"}\n"
"\n"
"QSlider::handle:horizontal:hover, QSlider::handle:vertical:hover {\n"
"background: #82766A;\n"
"\n"
"}"));
        actionNew_Window = new QAction(additiveClass);
        actionNew_Window->setObjectName(QString::fromUtf8("actionNew_Window"));
        actionOpen = new QAction(additiveClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(additiveClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionFull_Screen = new QAction(additiveClass);
        actionFull_Screen->setObjectName(QString::fromUtf8("actionFull_Screen"));
        actionAbout = new QAction(additiveClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionPreview = new QAction(additiveClass);
        actionPreview->setObjectName(QString::fromUtf8("actionPreview"));
        centralWidget = new QWidget(additiveClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setMouseTracking(true);
        btnHello = new QPushButton(centralWidget);
        btnHello->setObjectName(QString::fromUtf8("btnHello"));
        btnHello->setGeometry(QRect(20, 30, 100, 28));
        verticalSlider = new QSlider(centralWidget);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        verticalSlider->setGeometry(QRect(280, 20, 22, 121));
        verticalSlider->setValue(50);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setInvertedAppearance(false);
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(139, 30, 131, 22));
        horizontalSlider->setMaximum(100);
        horizontalSlider->setValue(50);
        horizontalSlider->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(320, 10, 301, 131));
        QFont font;
        font.setFamily(QString::fromUtf8("Lucida Sans"));
        font.setPointSize(10);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        font.setStrikeOut(false);
        font.setKerning(true);
        tabWidget->setFont(font);
        tabWidget->setAutoFillBackground(false);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        checkBox = new QCheckBox(tab);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(30, 40, 101, 17));
        checkBox->setChecked(true);
        checkBox_2 = new QCheckBox(tab);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setGeometry(QRect(180, 40, 101, 17));
        checkBox_2->setAutoFillBackground(false);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        radioButton = new QRadioButton(tab_2);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(30, 40, 111, 17));
        radioButton_2 = new QRadioButton(tab_2);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setGeometry(QRect(170, 40, 111, 17));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        listWidget = new QListWidget(tab_3);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(0, 0, 299, 100));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);
        listWidget->setMouseTracking(true);
        listWidget->setFocusPolicy(Qt::NoFocus);
        listWidget->setFrameShape(QFrame::NoFrame);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tabWidget->addTab(tab_3, QString());
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(20, 90, 151, 20));
        centralWidget2 = new QWidget(centralWidget);
        centralWidget2->setObjectName(QString::fromUtf8("centralWidget2"));
        centralWidget2->setGeometry(QRect(20, 160, 601, 361));
        centralWidget2->setCursor(QCursor(Qt::ArrowCursor));
        centralWidget2->setMouseTracking(true);
        layoutWidget = new QWidget(centralWidget2);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(9, 9, 581, 341));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(140, 10, 46, 13));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(180, 90, 81, 20));
        additiveClass->setCentralWidget(centralWidget);
        btnHello->raise();
        verticalSlider->raise();
        horizontalSlider->raise();
        tabWidget->raise();
        lineEdit->raise();
        label->raise();
        centralWidget2->raise();
        pushButton->raise();
        menuBar = new QMenuBar(additiveClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 635, 31));
        menuBar->setFocusPolicy(Qt::StrongFocus);
        menuBar->setDefaultUp(false);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        additiveClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(additiveClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        additiveClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionPreview);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionFull_Screen);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(additiveClass);
        QObject::connect(actionFull_Screen, SIGNAL(triggered()), additiveClass, SLOT(slot_fullScreen()));

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(additiveClass);
    } // setupUi

    void retranslateUi(QMainWindow *additiveClass)
    {
        additiveClass->setWindowTitle(QApplication::translate("additiveClass", "Mesh Illustrator", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        additiveClass->setWhatsThis(QApplication::translate("additiveClass", "<html><head/><body><p><br/></p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionNew_Window->setText(QApplication::translate("additiveClass", "New Window", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("additiveClass", "Open", 0, QApplication::UnicodeUTF8));
        actionOpen->setShortcut(QApplication::translate("additiveClass", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("additiveClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionFull_Screen->setText(QApplication::translate("additiveClass", "FullScreen", 0, QApplication::UnicodeUTF8));
        actionFull_Screen->setShortcut(QApplication::translate("additiveClass", "Alt+Return", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("additiveClass", "About", 0, QApplication::UnicodeUTF8));
        actionPreview->setText(QApplication::translate("additiveClass", "Toggle Preview", 0, QApplication::UnicodeUTF8));
        actionPreview->setShortcut(QApplication::translate("additiveClass", "P", 0, QApplication::UnicodeUTF8));
        btnHello->setText(QApplication::translate("additiveClass", "Translucency", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("additiveClass", "Skeletals", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("additiveClass", "Muscle", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("additiveClass", "Systems", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("additiveClass", "Solid", 0, QApplication::UnicodeUTF8));
        radioButton_2->setText(QApplication::translate("additiveClass", "Wireframe", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("additiveClass", "Rendering", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("additiveClass", "Groups", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("additiveClass", "Opacity:", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("additiveClass", "Search", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("additiveClass", "File", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("additiveClass", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class additiveClass: public Ui_additiveClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDITIVE_H
