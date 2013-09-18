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
#include <QtGui/QDial>
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
    QWidget *centralWidget;
    QDial *dial;
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
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *additiveClass)
    {
        if (additiveClass->objectName().isEmpty())
            additiveClass->setObjectName(QString::fromUtf8("additiveClass"));
        additiveClass->resize(637, 591);
        additiveClass->setMouseTracking(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/additive/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        additiveClass->setWindowIcon(icon);
        additiveClass->setStyleSheet(QString::fromUtf8("QWidget{\n"
"	color: #FFFFFF;\n"
"	font: 10pt \"Lucida Sans\";\n"
"	background-color: #505050;\n"
"	selection-background-color:#FEE728;\n"
"}\n"
"\n"
"QPushButton {\n"
"color: white;\n"
"border: 1px solid #cccccc;\n"
"border-radius: 12px;\n"
"padding: 5px;\n"
"background-color:qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #777777, stop: 1 #222222);\n"
"min-width: 80px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"border: 1px solid #ffffff;\n"
"color:white;\n"
"background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #222222, stop: 1 #777777);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"color: white;\n"
"background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #000, stop: 1 #000);\n"
"}\n"
"\n"
"QCheckBox, QRadioButton {	\n"
" \n"
"	border-color: rgb(255, 255, 255);\n"
"	alternate-background-color: rgb(120, 255, 96);\n"
"}\n"
"\n"
"QMenu  {\n"
"	selection-color: #000000;\n"
"	color: #000000;\n"
"	backgroun"
                        "d-color: #FFFFFF;\n"
"}\n"
"\n"
"QMenuBar {\n"
"	border-bottom: 1px solid #999955;\n"
"}\n"
"\n"
" QTabWidget::pane { \n"
"     border-left: 1px solid #eee;\n"
"     border-top: 1px solid #eee;\n"
"     border-right: 1px solid #aaa;\n"
"     border-bottom: 1px solid #aaa;\n"
" }\n"
"\n"
"QLineEdit {\n"
"color: white;\n"
"selection-color: blue;\n"
"                        \n"
"} \n"
"\n"
"QStatusBar {\n"
"	color: #FFFFFF;\n"
"	border-top: 2px solid #000;\n"
"	background-color: #252525;\n"
"\n"
"\n"
"}\n"
"\n"
" QCheckBox::indicator\n"
"  {\n"
"	width: 18px;\n"
"	height: 18px;\n"
"  }\n"
" QCheckBox::indicator:checked\n"
"  {\n"
"	image: url(:/additive/checked.gif);\n"
"  }\n"
"  QCheckBox::indicator:unchecked\n"
"  {\n"
"	image: url(:/additive/unchecked.gif);\n"
"  }\n"
"\n"
"\n"
"#centralWidget2 {\n"
"	border: 2px solid #FFC35B;\n"
"}\n"
"\n"
"#qv{\n"
"	border: 2px solid #FFC35B;\n"
"}\n"
"\n"
"QListWidget {\n"
"	\n"
"	selection-color: rgb(0, 0, 0);\n"
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
        centralWidget = new QWidget(additiveClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setMouseTracking(true);
        dial = new QDial(centralWidget);
        dial->setObjectName(QString::fromUtf8("dial"));
        dial->setGeometry(QRect(180, 80, 50, 51));
        btnHello = new QPushButton(centralWidget);
        btnHello->setObjectName(QString::fromUtf8("btnHello"));
        btnHello->setGeometry(QRect(20, 30, 92, 23));
        verticalSlider = new QSlider(centralWidget);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        verticalSlider->setGeometry(QRect(280, 20, 22, 121));
        verticalSlider->setValue(50);
        verticalSlider->setOrientation(Qt::Vertical);
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(129, 30, 141, 22));
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
        listWidget->setGeometry(QRect(0, 0, 301, 111));
        listWidget->setMouseTracking(true);
        tabWidget->addTab(tab_3, QString());
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(20, 90, 131, 20));
        centralWidget2 = new QWidget(centralWidget);
        centralWidget2->setObjectName(QString::fromUtf8("centralWidget2"));
        centralWidget2->setGeometry(QRect(20, 160, 601, 371));
        centralWidget2->setCursor(QCursor(Qt::CrossCursor));
        centralWidget2->setMouseTracking(true);
        gridLayoutWidget = new QWidget(centralWidget2);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 10, 581, 351));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(130, 10, 46, 13));
        additiveClass->setCentralWidget(centralWidget);
        dial->raise();
        btnHello->raise();
        verticalSlider->raise();
        horizontalSlider->raise();
        tabWidget->raise();
        lineEdit->raise();
        label->raise();
        centralWidget2->raise();
        menuBar = new QMenuBar(additiveClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 637, 21));
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
        menuFile->addAction(actionNew_Window);
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
        additiveClass->setWindowTitle(QApplication::translate("additiveClass", "Additive Widgets", 0, QApplication::UnicodeUTF8));
        actionNew_Window->setText(QApplication::translate("additiveClass", "New Window", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("additiveClass", "Open", 0, QApplication::UnicodeUTF8));
        actionOpen->setShortcut(QApplication::translate("additiveClass", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("additiveClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionFull_Screen->setText(QApplication::translate("additiveClass", "FullScreen", 0, QApplication::UnicodeUTF8));
        actionFull_Screen->setShortcut(QApplication::translate("additiveClass", "Alt+Return", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("additiveClass", "About", 0, QApplication::UnicodeUTF8));
        btnHello->setText(QApplication::translate("additiveClass", "Show Edge", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("additiveClass", "Skeletals", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("additiveClass", "Muscle", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("additiveClass", "Systems", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("additiveClass", "Solid", 0, QApplication::UnicodeUTF8));
        radioButton_2->setText(QApplication::translate("additiveClass", "Wireframe", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("additiveClass", "Rendering", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("additiveClass", "Groups", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("additiveClass", "Opacity:", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("additiveClass", "File", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("additiveClass", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class additiveClass: public Ui_additiveClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDITIVE_H
