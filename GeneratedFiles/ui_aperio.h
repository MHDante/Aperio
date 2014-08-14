/********************************************************************************
** Form generated from reading UI file 'aperio.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APERIO_H
#define UI_APERIO_H

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
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_aperioClass
{
public:
    QAction *actionAbout;
    QAction *actionToggle;
    QAction *actionOpen;
    QAction *actionFullScreen;
    QAction *actionExit;
    QWidget *centralWidget;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QPushButton *btnColor;
    QWidget *mainWidget;
    QSlider *verticalSlider;
    QPushButton *btnHello;
    QSlider *verticalSlider_2;
    QSlider *horizontalSlider;
    QTabWidget *tabWidget;
    QWidget *tab;
    QCheckBox *chkTriangulate;
    QCheckBox *chkDepthPeel;
    QCheckBox *chkWiggle;
    QWidget *tab_2;
    QCheckBox *chkToroid;
    QSlider *phiSlider;
    QSlider *thetaSlider;
    QLabel *label_7;
    QLabel *label_8;
    QWidget *tab_3;
    QListWidget *listWidget;
    QLabel *label;
    QSlider *hingeSlider;
    QLineEdit *hingeAmount;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QPushButton *pushButton_2;
    QSlider *hingeSlider_2;
    QLineEdit *explodeAmount;
    QSlider *hingeSlider_3;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *aperioClass)
    {
        if (aperioClass->objectName().isEmpty())
            aperioClass->setObjectName(QStringLiteral("aperioClass"));
        aperioClass->resize(827, 590);
        aperioClass->setStyleSheet(QLatin1String("QWidget{\n"
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
                        "(:/aperio/checked_list.png);\n"
"}\n"
"\n"
"QListView::indicator:unchecked\n"
"{\n"
"/*background: none;\n"
"border: 1px solid #222222;*/\n"
"	background-image: url(:/aperio/unchecked_list.png);\n"
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
"/*background: qradialgradient(cx: 0.3, cy: -"
                        "0.4,\n"
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
"	background-image: url(:/aperio/checked_list.png);\n"
"/*	 border-left: 2px solid blue;*/\n"
"	/*image: url(:/additive/checked.gif);*/\n"
"  }\n"
"  QCheckBox::indicator:unchecked\n"
"  {\n"
"	background-image: url(:/aperio/unchecked_list.png);\n"
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
" QScrollBar::handle:vertical  "
                        "{\n"
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
" 	background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 #FA7401, stop:1 #E39B51);\n"
"\n"
""
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
        actionAbout = new QAction(aperioClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionToggle = new QAction(aperioClass);
        actionToggle->setObjectName(QStringLiteral("actionToggle"));
        actionOpen = new QAction(aperioClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionFullScreen = new QAction(aperioClass);
        actionFullScreen->setObjectName(QStringLiteral("actionFullScreen"));
        actionExit = new QAction(aperioClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralWidget = new QWidget(aperioClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(150, 90, 81, 20));
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(20, 90, 121, 20));
        btnColor = new QPushButton(centralWidget);
        btnColor->setObjectName(QStringLiteral("btnColor"));
        btnColor->setGeometry(QRect(150, 120, 81, 20));
        mainWidget = new QWidget(centralWidget);
        mainWidget->setObjectName(QStringLiteral("mainWidget"));
        mainWidget->setGeometry(QRect(20, 160, 791, 361));
        mainWidget->setCursor(QCursor(Qt::ArrowCursor));
        mainWidget->setMouseTracking(true);
        verticalSlider = new QSlider(centralWidget);
        verticalSlider->setObjectName(QStringLiteral("verticalSlider"));
        verticalSlider->setGeometry(QRect(250, 50, 22, 91));
        verticalSlider->setMaximum(256);
        verticalSlider->setValue(128);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setInvertedAppearance(false);
        btnHello = new QPushButton(centralWidget);
        btnHello->setObjectName(QStringLiteral("btnHello"));
        btnHello->setGeometry(QRect(20, 30, 100, 28));
        verticalSlider_2 = new QSlider(centralWidget);
        verticalSlider_2->setObjectName(QStringLiteral("verticalSlider_2"));
        verticalSlider_2->setGeometry(QRect(280, 50, 22, 91));
        verticalSlider_2->setMaximum(96);
        verticalSlider_2->setValue(0);
        verticalSlider_2->setOrientation(Qt::Vertical);
        verticalSlider_2->setInvertedAppearance(false);
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(139, 30, 91, 22));
        horizontalSlider->setMaximum(100);
        horizontalSlider->setValue(50);
        horizontalSlider->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(310, 10, 291, 131));
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
        tabWidget->setElideMode(Qt::ElideNone);
        tabWidget->setDocumentMode(false);
        tabWidget->setMovable(true);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        chkTriangulate = new QCheckBox(tab);
        chkTriangulate->setObjectName(QStringLiteral("chkTriangulate"));
        chkTriangulate->setGeometry(QRect(10, 20, 261, 20));
        chkTriangulate->setChecked(true);
        chkDepthPeel = new QCheckBox(tab);
        chkDepthPeel->setObjectName(QStringLiteral("chkDepthPeel"));
        chkDepthPeel->setEnabled(true);
        chkDepthPeel->setGeometry(QRect(10, 50, 261, 20));
        chkDepthPeel->setChecked(false);
        chkWiggle = new QCheckBox(tab);
        chkWiggle->setObjectName(QStringLiteral("chkWiggle"));
        chkWiggle->setEnabled(true);
        chkWiggle->setGeometry(QRect(10, 80, 261, 20));
        chkWiggle->setChecked(true);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        chkToroid = new QCheckBox(tab_2);
        chkToroid->setObjectName(QStringLiteral("chkToroid"));
        chkToroid->setGeometry(QRect(100, 80, 81, 17));
        chkToroid->setAutoFillBackground(false);
        phiSlider = new QSlider(tab_2);
        phiSlider->setObjectName(QStringLiteral("phiSlider"));
        phiSlider->setGeometry(QRect(160, 20, 111, 22));
        phiSlider->setMaximum(100);
        phiSlider->setValue(0);
        phiSlider->setOrientation(Qt::Horizontal);
        phiSlider->setInvertedAppearance(false);
        phiSlider->setInvertedControls(false);
        phiSlider->setTickPosition(QSlider::NoTicks);
        thetaSlider = new QSlider(tab_2);
        thetaSlider->setObjectName(QStringLiteral("thetaSlider"));
        thetaSlider->setGeometry(QRect(160, 50, 111, 22));
        thetaSlider->setMaximum(100);
        thetaSlider->setValue(0);
        thetaSlider->setOrientation(Qt::Horizontal);
        thetaSlider->setTickPosition(QSlider::NoTicks);
        label_7 = new QLabel(tab_2);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 20, 101, 16));
        label_8 = new QLabel(tab_2);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 50, 121, 16));
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
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(140, 20, 46, 13));
        hingeSlider = new QSlider(centralWidget);
        hingeSlider->setObjectName(QStringLiteral("hingeSlider"));
        hingeSlider->setGeometry(QRect(660, 20, 101, 22));
        hingeSlider->setMaximum(100);
        hingeSlider->setValue(0);
        hingeSlider->setOrientation(Qt::Horizontal);
        hingeSlider->setTickPosition(QSlider::NoTicks);
        hingeAmount = new QLineEdit(centralWidget);
        hingeAmount->setObjectName(QStringLiteral("hingeAmount"));
        hingeAmount->setGeometry(QRect(770, 20, 31, 20));
        hingeAmount->setLayoutDirection(Qt::LeftToRight);
        hingeAmount->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(610, 10, 41, 41));
        label_2->setPixmap(QPixmap(QString::fromUtf8(":/aperio/hinge.png")));
        label_2->setScaledContents(true);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(250, 10, 21, 21));
        label_3->setPixmap(QPixmap(QString::fromUtf8(":/aperio/shiny.png")));
        label_3->setScaledContents(true);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(280, 10, 21, 21));
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/aperio/rough.png")));
        label_4->setScaledContents(true);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(615, 60, 34, 34));
        label_5->setPixmap(QPixmap(QString::fromUtf8(":/aperio/explode.png")));
        label_5->setScaledContents(true);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(615, 105, 35, 35));
        label_6->setPixmap(QPixmap(QString::fromUtf8(":/aperio/sliding.png")));
        label_6->setScaledContents(true);
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(770, 110, 41, 41));
        QIcon icon;
        icon.addFile(QStringLiteral(":/aperio/slice.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon);
        pushButton_2->setIconSize(QSize(50, 50));
        hingeSlider_2 = new QSlider(centralWidget);
        hingeSlider_2->setObjectName(QStringLiteral("hingeSlider_2"));
        hingeSlider_2->setGeometry(QRect(660, 70, 101, 22));
        hingeSlider_2->setMaximum(100);
        hingeSlider_2->setValue(0);
        hingeSlider_2->setOrientation(Qt::Horizontal);
        hingeSlider_2->setTickPosition(QSlider::NoTicks);
        explodeAmount = new QLineEdit(centralWidget);
        explodeAmount->setObjectName(QStringLiteral("explodeAmount"));
        explodeAmount->setGeometry(QRect(770, 70, 31, 20));
        explodeAmount->setLayoutDirection(Qt::LeftToRight);
        explodeAmount->setAlignment(Qt::AlignCenter);
        hingeSlider_3 = new QSlider(centralWidget);
        hingeSlider_3->setObjectName(QStringLiteral("hingeSlider_3"));
        hingeSlider_3->setGeometry(QRect(660, 110, 101, 22));
        hingeSlider_3->setMaximum(100);
        hingeSlider_3->setValue(0);
        hingeSlider_3->setOrientation(Qt::Horizontal);
        hingeSlider_3->setTickPosition(QSlider::NoTicks);
        aperioClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(aperioClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 827, 31));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        aperioClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(aperioClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        aperioClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionToggle);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionFullScreen);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(aperioClass);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(aperioClass);
    } // setupUi

    void retranslateUi(QMainWindow *aperioClass)
    {
        aperioClass->setWindowTitle(QApplication::translate("aperioClass", "Aperio", 0));
        actionAbout->setText(QApplication::translate("aperioClass", "About", 0));
        actionToggle->setText(QApplication::translate("aperioClass", "Toggle Preview", 0));
        actionToggle->setShortcut(QApplication::translate("aperioClass", "P", 0));
        actionOpen->setText(QApplication::translate("aperioClass", "Open", 0));
        actionOpen->setShortcut(QApplication::translate("aperioClass", "Ctrl+O", 0));
        actionFullScreen->setText(QApplication::translate("aperioClass", "FullScreen", 0));
        actionFullScreen->setShortcut(QApplication::translate("aperioClass", "Alt+Return", 0));
        actionExit->setText(QApplication::translate("aperioClass", "Exit", 0));
        pushButton->setText(QApplication::translate("aperioClass", "Search", 0));
        lineEdit->setPlaceholderText(QString());
        btnColor->setText(QApplication::translate("aperioClass", "Color", 0));
#ifndef QT_NO_TOOLTIP
        verticalSlider->setToolTip(QApplication::translate("aperioClass", "Shininess", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btnHello->setToolTip(QApplication::translate("aperioClass", "Forward-Scattered Translucency", 0));
#endif // QT_NO_TOOLTIP
        btnHello->setText(QApplication::translate("aperioClass", "Light: 1", 0));
#ifndef QT_NO_TOOLTIP
        verticalSlider_2->setToolTip(QApplication::translate("aperioClass", "Roughness", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        horizontalSlider->setToolTip(QApplication::translate("aperioClass", "Opacity", 0));
#endif // QT_NO_TOOLTIP
        chkTriangulate->setText(QApplication::translate("aperioClass", "Triangulate hollow surface after slice", 0));
        chkDepthPeel->setText(QApplication::translate("aperioClass", "Depth Peeling for Translucency", 0));
        chkWiggle->setText(QApplication::translate("aperioClass", "Wiggle Selected Mesh", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("aperioClass", "Options", 0));
        chkToroid->setText(QApplication::translate("aperioClass", "Toroidal", 0));
#ifndef QT_NO_TOOLTIP
        phiSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        thetaSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("aperioClass", "Phi Roundness", 0));
        label_8->setText(QApplication::translate("aperioClass", "Theta Roundness", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("aperioClass", "Superquad", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("aperioClass", "Groups", 0));
        label->setText(QApplication::translate("aperioClass", "Opacity:", 0));
#ifndef QT_NO_TOOLTIP
        hingeSlider->setToolTip(QApplication::translate("aperioClass", "Hinge", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        hingeAmount->setToolTip(QApplication::translate("aperioClass", "Hinge amount (degrees)", 0));
#endif // QT_NO_TOOLTIP
        hingeAmount->setText(QApplication::translate("aperioClass", "135", 0));
        hingeAmount->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        label_2->setToolTip(QApplication::translate("aperioClass", "Hinge", 0));
#endif // QT_NO_TOOLTIP
        label_2->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_3->setToolTip(QApplication::translate("aperioClass", "Shininess", 0));
#endif // QT_NO_TOOLTIP
        label_3->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_4->setToolTip(QApplication::translate("aperioClass", "Roughness", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_5->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        label_5->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_6->setToolTip(QApplication::translate("aperioClass", "Slide", 0));
#endif // QT_NO_TOOLTIP
        label_6->setText(QString());
#ifndef QT_NO_TOOLTIP
        pushButton_2->setToolTip(QApplication::translate("aperioClass", "Slice", 0));
#endif // QT_NO_TOOLTIP
        pushButton_2->setText(QString());
#ifndef QT_NO_TOOLTIP
        hingeSlider_2->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        explodeAmount->setToolTip(QApplication::translate("aperioClass", "Explosion Amount (units)", 0));
#endif // QT_NO_TOOLTIP
        explodeAmount->setText(QApplication::translate("aperioClass", "10", 0));
        explodeAmount->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        hingeSlider_3->setToolTip(QApplication::translate("aperioClass", "Slide", 0));
#endif // QT_NO_TOOLTIP
        menuFile->setTitle(QApplication::translate("aperioClass", "File", 0));
        menuHelp->setTitle(QApplication::translate("aperioClass", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class aperioClass: public Ui_aperioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APERIO_H
