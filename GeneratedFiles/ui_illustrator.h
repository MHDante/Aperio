/********************************************************************************
** Form generated from reading UI file 'illustrator.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ILLUSTRATOR_H
#define UI_ILLUSTRATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_illustratorClass
{
public:
    QAction *actionPreview;
    QAction *actionOpen;
    QAction *actionFull_Screen;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralWidget;
    QPushButton *btnHello;
    QLabel *label;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QWidget *mainWidget;
    QSlider *horizontalSlider;
    QSlider *verticalSlider_2;
    QTabWidget *tabWidget;
    QWidget *tab;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QWidget *tab_2;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QWidget *tab_3;
    QListWidget *listWidget;
    QSlider *verticalSlider;
    QPushButton *btnColor;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *illustratorClass)
    {
        if (illustratorClass->objectName().isEmpty())
            illustratorClass->setObjectName(QStringLiteral("illustratorClass"));
        illustratorClass->resize(635, 590);
        illustratorClass->setStyleSheet(QLatin1String("QWidget{\n"
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
"#mainWidget {\n"
"	border: 2px solid #D0592A;\n"
"}\n"
"\n"
"QPushButton {\n"
"\n"
"b"
                        "ackground: rgba(86,80,72,150);\n"
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
"QListView::indicator:checked\n"
"{\n"
"/*background-color: #988C84;\n"
"border: 1px solid #222222;*/\n"
"	background-image: url"
                        "(:/illustrator/checked_list.png);\n"
"}\n"
"\n"
"QListView::indicator:unchecked\n"
"{\n"
"/*background: none;\n"
"border: 1px solid #222222;*/\n"
"	background-image: url(:/illustrator/unchecked_list.png);\n"
"}\n"
"\n"
" QTabWidget::pane { /* The tab widget frame */\n"
"	border: none;\n"
"	background: rgba(57, 60, 46, 255);\n"
" }\n"
"\n"
"QTabBar::tab {\n"
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
"/*background: qradialgradient(cx: "
                        "0.3, cy: -0.4,\n"
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
"  {\n"
"	width: 13px;\n"
"	height: 13px;\n"
"  }\n"
" QCheckBox::indicator:checked\n"
"  {\n"
"	background-image: url(:/illustrator/checked_list.png);\n"
"/*	 border-left: 2px solid blue;*/\n"
"	/*image: url(:/additive/checked.gif);*/\n"
"  }\n"
"  QCheckBox::indicator:unchecked\n"
"  {\n"
"	background-image: url(:/illustrator/unchecked_list.png);\n"
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
" QScrollBa"
                        "r::handle:vertical  {\n"
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
" QScrollBar::sub-line:vertical  {\n"
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
" 	background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 #FA7401, sto"
                        "p:1 #E39B51);\n"
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
        actionPreview = new QAction(illustratorClass);
        actionPreview->setObjectName(QStringLiteral("actionPreview"));
        actionOpen = new QAction(illustratorClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionFull_Screen = new QAction(illustratorClass);
        actionFull_Screen->setObjectName(QStringLiteral("actionFull_Screen"));
        actionExit = new QAction(illustratorClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionAbout = new QAction(illustratorClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(illustratorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnHello = new QPushButton(centralWidget);
        btnHello->setObjectName(QStringLiteral("btnHello"));
        btnHello->setGeometry(QRect(20, 30, 100, 28));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(140, 10, 46, 13));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(180, 90, 81, 20));
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(20, 90, 151, 20));
        mainWidget = new QWidget(centralWidget);
        mainWidget->setObjectName(QStringLiteral("mainWidget"));
        mainWidget->setGeometry(QRect(20, 160, 601, 361));
        mainWidget->setCursor(QCursor(Qt::ArrowCursor));
        mainWidget->setMouseTracking(true);
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(139, 30, 131, 22));
        horizontalSlider->setMaximum(100);
        horizontalSlider->setValue(50);
        horizontalSlider->setOrientation(Qt::Horizontal);
        verticalSlider_2 = new QSlider(centralWidget);
        verticalSlider_2->setObjectName(QStringLiteral("verticalSlider_2"));
        verticalSlider_2->setGeometry(QRect(290, 20, 22, 121));
        verticalSlider_2->setMaximum(96);
        verticalSlider_2->setValue(26);
        verticalSlider_2->setOrientation(Qt::Vertical);
        verticalSlider_2->setInvertedAppearance(false);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(320, 10, 301, 131));
        QFont font;
        font.setFamily(QStringLiteral("Lucida Sans"));
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
        tab->setObjectName(QStringLiteral("tab"));
        checkBox = new QCheckBox(tab);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(30, 40, 101, 17));
        checkBox->setChecked(true);
        checkBox_2 = new QCheckBox(tab);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
        checkBox_2->setGeometry(QRect(180, 40, 101, 17));
        checkBox_2->setAutoFillBackground(false);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        radioButton = new QRadioButton(tab_2);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(30, 40, 111, 17));
        radioButton_2 = new QRadioButton(tab_2);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        radioButton_2->setGeometry(QRect(170, 40, 111, 17));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        listWidget = new QListWidget(tab_3);
        listWidget->setObjectName(QStringLiteral("listWidget"));
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
        verticalSlider = new QSlider(centralWidget);
        verticalSlider->setObjectName(QStringLiteral("verticalSlider"));
        verticalSlider->setGeometry(QRect(270, 20, 22, 121));
        verticalSlider->setMaximum(256);
        verticalSlider->setValue(128);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setInvertedAppearance(false);
        btnColor = new QPushButton(centralWidget);
        btnColor->setObjectName(QStringLiteral("btnColor"));
        btnColor->setGeometry(QRect(180, 120, 81, 20));
        illustratorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(illustratorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 635, 31));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        illustratorClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(illustratorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        illustratorClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionPreview);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionFull_Screen);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(illustratorClass);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(illustratorClass);
    } // setupUi

    void retranslateUi(QMainWindow *illustratorClass)
    {
        illustratorClass->setWindowTitle(QApplication::translate("illustratorClass", "Aperio", 0));
        actionPreview->setText(QApplication::translate("illustratorClass", "Toggle Preview", 0));
        actionPreview->setShortcut(QApplication::translate("illustratorClass", "P", 0));
        actionOpen->setText(QApplication::translate("illustratorClass", "Open", 0));
        actionOpen->setShortcut(QApplication::translate("illustratorClass", "Ctrl+O", 0));
        actionFull_Screen->setText(QApplication::translate("illustratorClass", "FullScreen", 0));
        actionFull_Screen->setShortcut(QApplication::translate("illustratorClass", "Alt+Return", 0));
        actionExit->setText(QApplication::translate("illustratorClass", "Exit", 0));
        actionAbout->setText(QApplication::translate("illustratorClass", "About", 0));
        btnHello->setText(QApplication::translate("illustratorClass", "Light: 1", 0));
        label->setText(QApplication::translate("illustratorClass", "Opacity:", 0));
        pushButton->setText(QApplication::translate("illustratorClass", "Search", 0));
        lineEdit->setPlaceholderText(QString());
        checkBox->setText(QApplication::translate("illustratorClass", "Skeletals", 0));
        checkBox_2->setText(QApplication::translate("illustratorClass", "Muscle", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("illustratorClass", "Systems", 0));
        radioButton->setText(QApplication::translate("illustratorClass", "Solid", 0));
        radioButton_2->setText(QApplication::translate("illustratorClass", "Wireframe", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("illustratorClass", "Rendering", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("illustratorClass", "Groups", 0));
        btnColor->setText(QApplication::translate("illustratorClass", "Color", 0));
        menuFile->setTitle(QApplication::translate("illustratorClass", "File", 0));
        menuHelp->setTitle(QApplication::translate("illustratorClass", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class illustratorClass: public Ui_illustratorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ILLUSTRATOR_H
