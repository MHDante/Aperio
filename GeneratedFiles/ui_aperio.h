/********************************************************************************
** Form generated from reading UI file 'aperio.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
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
    QAction *actionAppend;
    QAction *actionFullScreen;
    QAction *actionExit;
    QWidget *centralWidget;
    QPushButton *btnSearch;
    QLineEdit *txtSearchText;
    QPushButton *btnColor;
    QWidget *mainWidget;
    QSlider *shininessSlider;
    QPushButton *btnLight;
    QSlider *darknessSlider;
    QSlider *opacitySlider;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QListWidget *listWidget;
    QWidget *tab;
    QCheckBox *chkWiggle;
    QCheckBox *chkSnap;
    QCheckBox *chkSnapReal;
    QWidget *tab_2;
    QPushButton *btnHinge;
    QSlider *hingeSlider;
    QLineEdit *txtHingeAmount;
    QPushButton *btnGlass;
    QCheckBox *chkCap;
    QCheckBox *chkDepthPeel;
    QPushButton *btnBrush;
    QSlider *thicknessSlider;
    QLabel *lblThickness;
    QCheckBox *chkToroid;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QSlider *ringSlider;
    QSlider *rodSlider;
    QPushButton *btnRing;
    QPushButton *btnRod;
    QPushButton *btnHide;
    QPushButton *btnKnife;
    QPushButton *btnRestore;
    QPushButton *btnCookie;
    QTabWidget *tabWidget_2;
    QWidget *tab_4;
    QSlider *ribbonwidthSlider;
    QSlider *ribbonfreqSlider;
    QLabel *lblRibbonWidth;
    QLabel *lblRibbonFrequency;
    QCheckBox *chkFrontRibbons;
    QPushButton *btnRibbons;
    QLabel *lblRibbonTilt;
    QSlider *ribbontiltSlider;
    QTabWidget *tabWidget_3;
    QWidget *tab_6;
    QLabel *label_11;
    QSlider *phiSlider;
    QLabel *label_13;
    QLabel *label_14;
    QSlider *thetaSlider;
    QSlider *taperSlider;
    QLabel *lblTaper;
    QPushButton *btnPickUp;
    QPushButton *btnCut;
    QPushButton *btnBackground;
    QLabel *label_2;
    QLabel *label_5;
    QLabel *label_7;
    QCheckBox *chkSpread;
    QPushButton *btnPlant;
    QPushButton *btnInvert;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *aperioClass)
    {
        if (aperioClass->objectName().isEmpty())
            aperioClass->setObjectName(QStringLiteral("aperioClass"));
        aperioClass->resize(1318, 600);
        aperioClass->setMouseTracking(false);
        aperioClass->setFocusPolicy(Qt::NoFocus);
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
                        "ackground: rgba(86, 80, 72, 150);\n"
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
"#tab, #tab_2, #tab_3, QListView,\n"
"#tab_4, #tab_6 {\n"
"	background: rgba(0, 0, 0, 0);\n"
"	color: #dddddd;\n"
"}\n"
"\n"
"QListView::indicator:checked\n"
"{\n"
"/*background-color: #988C84;\n"
"border: 1px solid #222222;*/\n"
""
                        "	background-image: url(:/aperio/checked_list.png);\n"
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
"/*background: qradialg"
                        "radient(cx: 0.3, cy: -0.4,\n"
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
" QScroll"
                        "Bar::handle:vertical  {\n"
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
" 	background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 #FA7401, s"
                        "top:1 #E39B51);\n"
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
        actionAbout = new QAction(aperioClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionToggle = new QAction(aperioClass);
        actionToggle->setObjectName(QStringLiteral("actionToggle"));
        actionToggle->setEnabled(true);
        actionToggle->setVisible(true);
        actionOpen = new QAction(aperioClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionAppend = new QAction(aperioClass);
        actionAppend->setObjectName(QStringLiteral("actionAppend"));
        actionFullScreen = new QAction(aperioClass);
        actionFullScreen->setObjectName(QStringLiteral("actionFullScreen"));
        actionExit = new QAction(aperioClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralWidget = new QWidget(aperioClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnSearch = new QPushButton(centralWidget);
        btnSearch->setObjectName(QStringLiteral("btnSearch"));
        btnSearch->setEnabled(true);
        btnSearch->setGeometry(QRect(150, 100, 81, 20));
        btnSearch->setFocusPolicy(Qt::NoFocus);
        txtSearchText = new QLineEdit(centralWidget);
        txtSearchText->setObjectName(QStringLiteral("txtSearchText"));
        txtSearchText->setEnabled(true);
        txtSearchText->setGeometry(QRect(20, 100, 121, 20));
        txtSearchText->setFocusPolicy(Qt::StrongFocus);
        btnColor = new QPushButton(centralWidget);
        btnColor->setObjectName(QStringLiteral("btnColor"));
        btnColor->setEnabled(true);
        btnColor->setGeometry(QRect(150, 130, 81, 20));
        btnColor->setFocusPolicy(Qt::NoFocus);
        mainWidget = new QWidget(centralWidget);
        mainWidget->setObjectName(QStringLiteral("mainWidget"));
        mainWidget->setGeometry(QRect(20, 170, 1281, 361));
        mainWidget->setCursor(QCursor(Qt::ArrowCursor));
        mainWidget->setMouseTracking(true);
        shininessSlider = new QSlider(centralWidget);
        shininessSlider->setObjectName(QStringLiteral("shininessSlider"));
        shininessSlider->setEnabled(true);
        shininessSlider->setGeometry(QRect(250, 50, 22, 101));
        shininessSlider->setFocusPolicy(Qt::NoFocus);
        shininessSlider->setMaximum(256);
        shininessSlider->setValue(47);
        shininessSlider->setOrientation(Qt::Vertical);
        shininessSlider->setInvertedAppearance(false);
        btnLight = new QPushButton(centralWidget);
        btnLight->setObjectName(QStringLiteral("btnLight"));
        btnLight->setEnabled(true);
        btnLight->setGeometry(QRect(20, 20, 100, 28));
        btnLight->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QStringLiteral(":/aperio/flashlight2.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnLight->setIcon(icon);
        btnLight->setIconSize(QSize(22, 22));
        darknessSlider = new QSlider(centralWidget);
        darknessSlider->setObjectName(QStringLiteral("darknessSlider"));
        darknessSlider->setEnabled(true);
        darknessSlider->setGeometry(QRect(280, 50, 22, 101));
        darknessSlider->setFocusPolicy(Qt::NoFocus);
        darknessSlider->setMaximum(96);
        darknessSlider->setValue(0);
        darknessSlider->setOrientation(Qt::Vertical);
        darknessSlider->setInvertedAppearance(false);
        opacitySlider = new QSlider(centralWidget);
        opacitySlider->setObjectName(QStringLiteral("opacitySlider"));
        opacitySlider->setEnabled(true);
        opacitySlider->setGeometry(QRect(139, 40, 91, 22));
        opacitySlider->setFocusPolicy(Qt::NoFocus);
        opacitySlider->setMaximum(100);
        opacitySlider->setValue(50);
        opacitySlider->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        tabWidget->setGeometry(QRect(310, 10, 241, 151));
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
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        listWidget = new QListWidget(tab_3);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setEnabled(true);
        listWidget->setGeometry(QRect(0, 0, 241, 121));
        QPalette palette;
        QBrush brush(QColor(221, 221, 221, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(0, 0, 0, 0));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        QBrush brush2(QColor(0, 0, 0, 255));
        brush2.setStyle(Qt::NoBrush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        QBrush brush3(QColor(254, 231, 40, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Highlight, brush3);
        QBrush brush4(QColor(74, 77, 63, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        QBrush brush5(QColor(0, 0, 0, 255));
        brush5.setStyle(Qt::NoBrush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Highlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::NoBrush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Highlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        listWidget->setPalette(palette);
        listWidget->setFocusPolicy(Qt::NoFocus);
        listWidget->setFrameShape(QFrame::NoFrame);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setDragDropMode(QAbstractItemView::InternalMove);
        listWidget->setAlternatingRowColors(true);
        listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        listWidget->setSelectionRectVisible(false);
        tabWidget->addTab(tab_3, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        chkWiggle = new QCheckBox(tab);
        chkWiggle->setObjectName(QStringLiteral("chkWiggle"));
        chkWiggle->setEnabled(true);
        chkWiggle->setGeometry(QRect(10, 90, 261, 20));
        chkWiggle->setFocusPolicy(Qt::NoFocus);
        chkWiggle->setChecked(true);
        chkSnap = new QCheckBox(tab);
        chkSnap->setObjectName(QStringLiteral("chkSnap"));
        chkSnap->setEnabled(true);
        chkSnap->setGeometry(QRect(10, 60, 211, 20));
        chkSnap->setFocusPolicy(Qt::NoFocus);
        chkSnap->setChecked(false);
        chkSnapReal = new QCheckBox(tab);
        chkSnapReal->setObjectName(QStringLiteral("chkSnapReal"));
        chkSnapReal->setEnabled(true);
        chkSnapReal->setGeometry(QRect(10, 20, 261, 20));
        chkSnapReal->setFocusPolicy(Qt::NoFocus);
        chkSnapReal->setChecked(false);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        btnHinge = new QPushButton(tab_2);
        btnHinge->setObjectName(QStringLiteral("btnHinge"));
        btnHinge->setEnabled(true);
        btnHinge->setGeometry(QRect(10, 10, 41, 41));
        btnHinge->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/aperio/hinge.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnHinge->setIcon(icon1);
        btnHinge->setIconSize(QSize(37, 37));
        hingeSlider = new QSlider(tab_2);
        hingeSlider->setObjectName(QStringLiteral("hingeSlider"));
        hingeSlider->setEnabled(true);
        hingeSlider->setGeometry(QRect(70, 20, 101, 22));
        hingeSlider->setFocusPolicy(Qt::NoFocus);
        hingeSlider->setMaximum(100);
        hingeSlider->setValue(0);
        hingeSlider->setOrientation(Qt::Horizontal);
        hingeSlider->setTickPosition(QSlider::NoTicks);
        txtHingeAmount = new QLineEdit(tab_2);
        txtHingeAmount->setObjectName(QStringLiteral("txtHingeAmount"));
        txtHingeAmount->setGeometry(QRect(180, 20, 31, 20));
        btnGlass = new QPushButton(tab_2);
        btnGlass->setObjectName(QStringLiteral("btnGlass"));
        btnGlass->setEnabled(true);
        btnGlass->setGeometry(QRect(10, 60, 41, 41));
        btnGlass->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/aperio/glass2.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnGlass->setIcon(icon2);
        btnGlass->setIconSize(QSize(37, 37));
        chkCap = new QCheckBox(tab_2);
        chkCap->setObjectName(QStringLiteral("chkCap"));
        chkCap->setGeometry(QRect(70, 50, 171, 20));
        chkCap->setFocusPolicy(Qt::NoFocus);
        chkCap->setChecked(true);
        chkDepthPeel = new QCheckBox(tab_2);
        chkDepthPeel->setObjectName(QStringLiteral("chkDepthPeel"));
        chkDepthPeel->setEnabled(true);
        chkDepthPeel->setGeometry(QRect(70, 80, 161, 20));
        chkDepthPeel->setFocusPolicy(Qt::NoFocus);
        chkDepthPeel->setChecked(false);
        btnBrush = new QPushButton(tab_2);
        btnBrush->setObjectName(QStringLiteral("btnBrush"));
        btnBrush->setEnabled(true);
        btnBrush->setGeometry(QRect(190, 70, 41, 41));
        btnBrush->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/aperio/brush.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnBrush->setIcon(icon3);
        btnBrush->setIconSize(QSize(38, 38));
        thicknessSlider = new QSlider(tab_2);
        thicknessSlider->setObjectName(QStringLiteral("thicknessSlider"));
        thicknessSlider->setEnabled(true);
        thicknessSlider->setGeometry(QRect(180, 110, 41, 16));
        thicknessSlider->setFocusPolicy(Qt::NoFocus);
        thicknessSlider->setMinimum(1);
        thicknessSlider->setMaximum(100);
        thicknessSlider->setValue(10);
        thicknessSlider->setOrientation(Qt::Horizontal);
        thicknessSlider->setInvertedAppearance(false);
        thicknessSlider->setInvertedControls(false);
        thicknessSlider->setTickPosition(QSlider::NoTicks);
        lblThickness = new QLabel(tab_2);
        lblThickness->setObjectName(QStringLiteral("lblThickness"));
        lblThickness->setGeometry(QRect(110, 110, 71, 16));
        chkToroid = new QCheckBox(tab_2);
        chkToroid->setObjectName(QStringLiteral("chkToroid"));
        chkToroid->setEnabled(false);
        chkToroid->setGeometry(QRect(30, 110, 81, 17));
        chkToroid->setAutoFillBackground(false);
        tabWidget->addTab(tab_2, QString());
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(140, 30, 46, 13));
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
        ringSlider = new QSlider(centralWidget);
        ringSlider->setObjectName(QStringLiteral("ringSlider"));
        ringSlider->setEnabled(true);
        ringSlider->setGeometry(QRect(610, 90, 141, 22));
        ringSlider->setFocusPolicy(Qt::NoFocus);
        ringSlider->setMaximum(100);
        ringSlider->setValue(0);
        ringSlider->setOrientation(Qt::Horizontal);
        ringSlider->setTickPosition(QSlider::NoTicks);
        rodSlider = new QSlider(centralWidget);
        rodSlider->setObjectName(QStringLiteral("rodSlider"));
        rodSlider->setEnabled(true);
        rodSlider->setGeometry(QRect(760, 90, 91, 22));
        rodSlider->setFocusPolicy(Qt::NoFocus);
        rodSlider->setMaximum(100);
        rodSlider->setValue(0);
        rodSlider->setOrientation(Qt::Horizontal);
        rodSlider->setTickPosition(QSlider::NoTicks);
        btnRing = new QPushButton(centralWidget);
        btnRing->setObjectName(QStringLiteral("btnRing"));
        btnRing->setEnabled(true);
        btnRing->setGeometry(QRect(560, 70, 41, 41));
        btnRing->setFocusPolicy(Qt::NoFocus);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/aperio/sliding2.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRing->setIcon(icon4);
        btnRing->setIconSize(QSize(37, 37));
        btnRing->setCheckable(false);
        btnRod = new QPushButton(centralWidget);
        btnRod->setObjectName(QStringLiteral("btnRod"));
        btnRod->setEnabled(true);
        btnRod->setGeometry(QRect(560, 122, 41, 41));
        btnRod->setFocusPolicy(Qt::NoFocus);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/aperio/sliding3.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRod->setIcon(icon5);
        btnRod->setIconSize(QSize(34, 34));
        btnHide = new QPushButton(centralWidget);
        btnHide->setObjectName(QStringLiteral("btnHide"));
        btnHide->setEnabled(true);
        btnHide->setGeometry(QRect(20, 130, 121, 21));
        btnHide->setFocusPolicy(Qt::NoFocus);
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/aperio/hide.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnHide->setIcon(icon6);
        btnHide->setIconSize(QSize(25, 20));
        btnKnife = new QPushButton(centralWidget);
        btnKnife->setObjectName(QStringLiteral("btnKnife"));
        btnKnife->setEnabled(true);
        btnKnife->setGeometry(QRect(610, 20, 41, 41));
        btnKnife->setFocusPolicy(Qt::NoFocus);
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/aperio/slice.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnKnife->setIcon(icon7);
        btnKnife->setIconSize(QSize(42, 42));
        btnRestore = new QPushButton(centralWidget);
        btnRestore->setObjectName(QStringLiteral("btnRestore"));
        btnRestore->setEnabled(true);
        btnRestore->setGeometry(QRect(710, 120, 41, 41));
        btnRestore->setFocusPolicy(Qt::NoFocus);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/aperio/restore.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRestore->setIcon(icon8);
        btnRestore->setIconSize(QSize(38, 38));
        btnCookie = new QPushButton(centralWidget);
        btnCookie->setObjectName(QStringLiteral("btnCookie"));
        btnCookie->setEnabled(true);
        btnCookie->setGeometry(QRect(560, 20, 41, 41));
        btnCookie->setFocusPolicy(Qt::NoFocus);
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/aperio/cookie.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCookie->setIcon(icon9);
        btnCookie->setIconSize(QSize(30, 30));
        tabWidget_2 = new QTabWidget(centralWidget);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));
        tabWidget_2->setEnabled(true);
        tabWidget_2->setGeometry(QRect(1080, 10, 221, 151));
        tabWidget_2->setFont(font);
        tabWidget_2->setAutoFillBackground(false);
        tabWidget_2->setTabPosition(QTabWidget::North);
        tabWidget_2->setTabShape(QTabWidget::Rounded);
        tabWidget_2->setElideMode(Qt::ElideNone);
        tabWidget_2->setDocumentMode(false);
        tabWidget_2->setMovable(true);
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        ribbonwidthSlider = new QSlider(tab_4);
        ribbonwidthSlider->setObjectName(QStringLiteral("ribbonwidthSlider"));
        ribbonwidthSlider->setGeometry(QRect(140, 10, 71, 22));
        ribbonwidthSlider->setFocusPolicy(Qt::NoFocus);
        ribbonwidthSlider->setMinimum(1);
        ribbonwidthSlider->setMaximum(100);
        ribbonwidthSlider->setValue(20);
        ribbonwidthSlider->setOrientation(Qt::Horizontal);
        ribbonwidthSlider->setInvertedAppearance(false);
        ribbonwidthSlider->setInvertedControls(false);
        ribbonwidthSlider->setTickPosition(QSlider::NoTicks);
        ribbonfreqSlider = new QSlider(tab_4);
        ribbonfreqSlider->setObjectName(QStringLiteral("ribbonfreqSlider"));
        ribbonfreqSlider->setGeometry(QRect(140, 40, 71, 22));
        ribbonfreqSlider->setFocusPolicy(Qt::NoFocus);
        ribbonfreqSlider->setMinimum(1);
        ribbonfreqSlider->setMaximum(100);
        ribbonfreqSlider->setValue(20);
        ribbonfreqSlider->setOrientation(Qt::Horizontal);
        ribbonfreqSlider->setTickPosition(QSlider::NoTicks);
        lblRibbonWidth = new QLabel(tab_4);
        lblRibbonWidth->setObjectName(QStringLiteral("lblRibbonWidth"));
        lblRibbonWidth->setGeometry(QRect(60, 10, 71, 16));
        lblRibbonFrequency = new QLabel(tab_4);
        lblRibbonFrequency->setObjectName(QStringLiteral("lblRibbonFrequency"));
        lblRibbonFrequency->setGeometry(QRect(60, 40, 71, 16));
        chkFrontRibbons = new QCheckBox(tab_4);
        chkFrontRibbons->setObjectName(QStringLiteral("chkFrontRibbons"));
        chkFrontRibbons->setGeometry(QRect(10, 95, 121, 17));
        chkFrontRibbons->setFocusPolicy(Qt::NoFocus);
        chkFrontRibbons->setAutoFillBackground(false);
        chkFrontRibbons->setChecked(true);
        btnRibbons = new QPushButton(tab_4);
        btnRibbons->setObjectName(QStringLiteral("btnRibbons"));
        btnRibbons->setEnabled(true);
        btnRibbons->setGeometry(QRect(10, 10, 41, 41));
        btnRibbons->setFocusPolicy(Qt::NoFocus);
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/aperio/ribbons.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRibbons->setIcon(icon10);
        btnRibbons->setIconSize(QSize(38, 38));
        lblRibbonTilt = new QLabel(tab_4);
        lblRibbonTilt->setObjectName(QStringLiteral("lblRibbonTilt"));
        lblRibbonTilt->setGeometry(QRect(60, 70, 71, 16));
        ribbontiltSlider = new QSlider(tab_4);
        ribbontiltSlider->setObjectName(QStringLiteral("ribbontiltSlider"));
        ribbontiltSlider->setGeometry(QRect(140, 70, 71, 22));
        ribbontiltSlider->setFocusPolicy(Qt::NoFocus);
        ribbontiltSlider->setMinimum(1);
        ribbontiltSlider->setMaximum(100);
        ribbontiltSlider->setValue(50);
        ribbontiltSlider->setOrientation(Qt::Horizontal);
        ribbontiltSlider->setTickPosition(QSlider::NoTicks);
        tabWidget_2->addTab(tab_4, QString());
        tabWidget_3 = new QTabWidget(centralWidget);
        tabWidget_3->setObjectName(QStringLiteral("tabWidget_3"));
        tabWidget_3->setEnabled(true);
        tabWidget_3->setGeometry(QRect(860, 10, 211, 151));
        tabWidget_3->setFont(font);
        tabWidget_3->setAutoFillBackground(false);
        tabWidget_3->setTabPosition(QTabWidget::North);
        tabWidget_3->setTabShape(QTabWidget::Rounded);
        tabWidget_3->setElideMode(Qt::ElideNone);
        tabWidget_3->setDocumentMode(false);
        tabWidget_3->setMovable(true);
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        label_11 = new QLabel(tab_6);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 5, 81, 41));
        label_11->setAutoFillBackground(false);
        label_11->setStyleSheet(QStringLiteral(""));
        label_11->setFrameShape(QFrame::NoFrame);
        label_11->setFrameShadow(QFrame::Plain);
        label_11->setLineWidth(1);
        label_11->setMidLineWidth(0);
        label_11->setTextFormat(Qt::AutoText);
        label_11->setPixmap(QPixmap(QString::fromUtf8(":/aperio/phi.png")));
        label_11->setScaledContents(true);
        phiSlider = new QSlider(tab_6);
        phiSlider->setObjectName(QStringLiteral("phiSlider"));
        phiSlider->setGeometry(QRect(100, 20, 91, 22));
        phiSlider->setFocusPolicy(Qt::NoFocus);
        phiSlider->setMinimum(1);
        phiSlider->setMaximum(100);
        phiSlider->setValue(2);
        phiSlider->setOrientation(Qt::Horizontal);
        phiSlider->setInvertedAppearance(false);
        phiSlider->setInvertedControls(false);
        phiSlider->setTickPosition(QSlider::NoTicks);
        label_13 = new QLabel(tab_6);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 10, 191, 71));
        label_13->setStyleSheet(QLatin1String("border-radius: 6px;\n"
"\n"
"background: rgba(255,255,255, 0.15);"));
        label_13->setFrameShape(QFrame::HLine);
        label_13->setScaledContents(true);
        label_14 = new QLabel(tab_6);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(10, 40, 81, 41));
        label_14->setStyleSheet(QStringLiteral(""));
        label_14->setFrameShape(QFrame::NoFrame);
        label_14->setPixmap(QPixmap(QString::fromUtf8(":/aperio/theta.png")));
        label_14->setScaledContents(true);
        thetaSlider = new QSlider(tab_6);
        thetaSlider->setObjectName(QStringLiteral("thetaSlider"));
        thetaSlider->setGeometry(QRect(100, 50, 91, 22));
        thetaSlider->setFocusPolicy(Qt::NoFocus);
        thetaSlider->setMinimum(3);
        thetaSlider->setMaximum(100);
        thetaSlider->setValue(100);
        thetaSlider->setOrientation(Qt::Horizontal);
        thetaSlider->setTickPosition(QSlider::NoTicks);
        taperSlider = new QSlider(tab_6);
        taperSlider->setObjectName(QStringLiteral("taperSlider"));
        taperSlider->setGeometry(QRect(100, 90, 91, 22));
        taperSlider->setFocusPolicy(Qt::NoFocus);
        taperSlider->setMinimum(0);
        taperSlider->setMaximum(100);
        taperSlider->setValue(0);
        taperSlider->setOrientation(Qt::Horizontal);
        taperSlider->setTickPosition(QSlider::NoTicks);
        lblTaper = new QLabel(tab_6);
        lblTaper->setObjectName(QStringLiteral("lblTaper"));
        lblTaper->setGeometry(QRect(40, 93, 46, 13));
        tabWidget_3->addTab(tab_6, QString());
        label_13->raise();
        label_11->raise();
        phiSlider->raise();
        label_14->raise();
        thetaSlider->raise();
        taperSlider->raise();
        lblTaper->raise();
        btnPickUp = new QPushButton(centralWidget);
        btnPickUp->setObjectName(QStringLiteral("btnPickUp"));
        btnPickUp->setEnabled(true);
        btnPickUp->setGeometry(QRect(660, 20, 91, 41));
        QFont font1;
        font1.setFamily(QStringLiteral("Lucida Sans"));
        font1.setPointSize(10);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(50);
        btnPickUp->setFont(font1);
        btnPickUp->setFocusPolicy(Qt::NoFocus);
        btnPickUp->setAutoFillBackground(false);
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/aperio/tools.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPickUp->setIcon(icon11);
        btnPickUp->setIconSize(QSize(20, 20));
        btnPickUp->setCheckable(false);
        btnCut = new QPushButton(centralWidget);
        btnCut->setObjectName(QStringLiteral("btnCut"));
        btnCut->setEnabled(true);
        btnCut->setGeometry(QRect(760, 120, 91, 41));
        btnCut->setFocusPolicy(Qt::NoFocus);
        btnCut->setIconSize(QSize(38, 38));
        btnBackground = new QPushButton(centralWidget);
        btnBackground->setObjectName(QStringLiteral("btnBackground"));
        btnBackground->setEnabled(true);
        btnBackground->setGeometry(QRect(20, 60, 101, 31));
        btnBackground->setFont(font1);
        btnBackground->setFocusPolicy(Qt::NoFocus);
        btnBackground->setAutoFillBackground(false);
        btnBackground->setIconSize(QSize(20, 20));
        btnBackground->setCheckable(false);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(560, 3, 46, 13));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(610, 70, 46, 13));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(760, 70, 46, 13));
        chkSpread = new QCheckBox(centralWidget);
        chkSpread->setObjectName(QStringLiteral("chkSpread"));
        chkSpread->setGeometry(QRect(630, 110, 71, 17));
        btnPlant = new QPushButton(centralWidget);
        btnPlant->setObjectName(QStringLiteral("btnPlant"));
        btnPlant->setEnabled(true);
        btnPlant->setGeometry(QRect(760, 20, 91, 41));
        btnPlant->setFocusPolicy(Qt::NoFocus);
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/aperio/plant.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPlant->setIcon(icon12);
        btnPlant->setIconSize(QSize(24, 24));
        btnInvert = new QPushButton(centralWidget);
        btnInvert->setObjectName(QStringLiteral("btnInvert"));
        btnInvert->setGeometry(QRect(610, 140, 91, 23));
        aperioClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(aperioClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1318, 31));
        menuBar->setMouseTracking(true);
        menuBar->setFocusPolicy(Qt::NoFocus);
        menuBar->setContextMenuPolicy(Qt::DefaultContextMenu);
        menuBar->setDefaultUp(false);
        menuBar->setNativeMenuBar(false);
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
        menuFile->addAction(actionAppend);
        menuFile->addAction(actionFullScreen);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(aperioClass);

        tabWidget->setCurrentIndex(0);
        tabWidget_2->setCurrentIndex(0);
        tabWidget_3->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(aperioClass);
    } // setupUi

    void retranslateUi(QMainWindow *aperioClass)
    {
        aperioClass->setWindowTitle(QApplication::translate("aperioClass", "Aperio", 0));
#ifndef QT_NO_TOOLTIP
        aperioClass->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        actionAbout->setText(QApplication::translate("aperioClass", "About", 0));
        actionToggle->setText(QApplication::translate("aperioClass", "Toggle Preview", 0));
        actionToggle->setShortcut(QApplication::translate("aperioClass", "P", 0));
        actionOpen->setText(QApplication::translate("aperioClass", "Open", 0));
        actionOpen->setShortcut(QApplication::translate("aperioClass", "Ctrl+O", 0));
        actionAppend->setText(QApplication::translate("aperioClass", "Append", 0));
        actionAppend->setShortcut(QApplication::translate("aperioClass", "Ctrl+A", 0));
        actionFullScreen->setText(QApplication::translate("aperioClass", "FullScreen", 0));
        actionFullScreen->setShortcut(QApplication::translate("aperioClass", "Alt+Return", 0));
        actionExit->setText(QApplication::translate("aperioClass", "Exit", 0));
        btnSearch->setText(QApplication::translate("aperioClass", "Search", 0));
        txtSearchText->setPlaceholderText(QString());
        btnColor->setText(QApplication::translate("aperioClass", "Color", 0));
#ifndef QT_NO_TOOLTIP
        shininessSlider->setToolTip(QApplication::translate("aperioClass", "Shininess", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btnLight->setToolTip(QApplication::translate("aperioClass", "Light (Forward-Scattered Translucency)", 0));
#endif // QT_NO_TOOLTIP
        btnLight->setText(QApplication::translate("aperioClass", "On", 0));
#ifndef QT_NO_TOOLTIP
        darknessSlider->setToolTip(QApplication::translate("aperioClass", "Roughness", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        opacitySlider->setToolTip(QApplication::translate("aperioClass", "Opacity", 0));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("aperioClass", "Items", 0));
        chkWiggle->setText(QApplication::translate("aperioClass", "Wiggle Selected Mesh", 0));
        chkSnap->setText(QApplication::translate("aperioClass", "Snap Ring When Planting ", 0));
        chkSnapReal->setText(QApplication::translate("aperioClass", "Snap Ring to BBOX (Real-time)", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("aperioClass", "Options", 0));
#ifndef QT_NO_TOOLTIP
        btnHinge->setToolTip(QApplication::translate("aperioClass", "Hinge", 0));
#endif // QT_NO_TOOLTIP
        btnHinge->setText(QString());
#ifndef QT_NO_TOOLTIP
        hingeSlider->setToolTip(QApplication::translate("aperioClass", "Hinge", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        txtHingeAmount->setToolTip(QApplication::translate("aperioClass", "Hinge Amount", 0));
#endif // QT_NO_TOOLTIP
        txtHingeAmount->setText(QApplication::translate("aperioClass", "170", 0));
#ifndef QT_NO_TOOLTIP
        btnGlass->setToolTip(QApplication::translate("aperioClass", "Magnifying Glass ", 0));
#endif // QT_NO_TOOLTIP
        btnGlass->setText(QString());
        chkCap->setText(QApplication::translate("aperioClass", "Cap Hole in Previewer", 0));
        chkDepthPeel->setText(QApplication::translate("aperioClass", "Depth Peeling", 0));
#ifndef QT_NO_TOOLTIP
        btnBrush->setToolTip(QApplication::translate("aperioClass", "Brush", 0));
#endif // QT_NO_TOOLTIP
        btnBrush->setText(QString());
#ifndef QT_NO_TOOLTIP
        thicknessSlider->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        lblThickness->setText(QApplication::translate("aperioClass", "Thickness", 0));
        chkToroid->setText(QApplication::translate("aperioClass", "Toroidal", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("aperioClass", "Etc.", 0));
        label->setText(QApplication::translate("aperioClass", "Opacity:", 0));
#ifndef QT_NO_TOOLTIP
        label_3->setToolTip(QApplication::translate("aperioClass", "Shininess", 0));
#endif // QT_NO_TOOLTIP
        label_3->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_4->setToolTip(QApplication::translate("aperioClass", "Roughness", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QString());
#ifndef QT_NO_TOOLTIP
        ringSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rodSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btnRing->setToolTip(QApplication::translate("aperioClass", "Explode Ring", 0));
#endif // QT_NO_TOOLTIP
        btnRing->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnRod->setToolTip(QApplication::translate("aperioClass", "Explode Rod", 0));
#endif // QT_NO_TOOLTIP
        btnRod->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnHide->setToolTip(QApplication::translate("aperioClass", "Show/Hide (H)", 0));
#endif // QT_NO_TOOLTIP
        btnHide->setText(QApplication::translate("aperioClass", "Show/Hide", 0));
#ifndef QT_NO_TOOLTIP
        btnKnife->setToolTip(QApplication::translate("aperioClass", "Knife (K)", 0));
#endif // QT_NO_TOOLTIP
        btnKnife->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnRestore->setToolTip(QApplication::translate("aperioClass", "Restore Mesh", 0));
#endif // QT_NO_TOOLTIP
        btnRestore->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnCookie->setToolTip(QApplication::translate("aperioClass", "Cookie Cutter (C)", 0));
#endif // QT_NO_TOOLTIP
        btnCookie->setText(QString());
#ifndef QT_NO_TOOLTIP
        ribbonwidthSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        ribbonfreqSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        lblRibbonWidth->setText(QApplication::translate("aperioClass", "Width", 0));
        lblRibbonFrequency->setText(QApplication::translate("aperioClass", "Frequency", 0));
        chkFrontRibbons->setText(QApplication::translate("aperioClass", "Front Ribbons", 0));
#ifndef QT_NO_TOOLTIP
        btnRibbons->setToolTip(QApplication::translate("aperioClass", "Toggle Ribbons (R)", 0));
#endif // QT_NO_TOOLTIP
        btnRibbons->setText(QString());
        lblRibbonTilt->setText(QApplication::translate("aperioClass", "Tilt", 0));
#ifndef QT_NO_TOOLTIP
        ribbontiltSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("aperioClass", "Ribbons", 0));
#ifndef QT_NO_TOOLTIP
        label_11->setToolTip(QApplication::translate("aperioClass", "Phi Roundness", 0));
#endif // QT_NO_TOOLTIP
        label_11->setText(QString());
#ifndef QT_NO_TOOLTIP
        phiSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_13->setToolTip(QApplication::translate("aperioClass", "Theta Roundness", 0));
#endif // QT_NO_TOOLTIP
        label_13->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_14->setToolTip(QApplication::translate("aperioClass", "Theta Roundness", 0));
#endif // QT_NO_TOOLTIP
        label_14->setText(QString());
#ifndef QT_NO_TOOLTIP
        thetaSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        taperSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        lblTaper->setText(QApplication::translate("aperioClass", "Taper", 0));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_6), QApplication::translate("aperioClass", "Roundness", 0));
#ifndef QT_NO_TOOLTIP
        btnPickUp->setToolTip(QApplication::translate("aperioClass", "Pick Up Tool (G)", 0));
#endif // QT_NO_TOOLTIP
        btnPickUp->setText(QApplication::translate("aperioClass", "Pick Up", 0));
#ifndef QT_NO_TOOLTIP
        btnCut->setToolTip(QApplication::translate("aperioClass", "Cut Mesh", 0));
#endif // QT_NO_TOOLTIP
        btnCut->setText(QApplication::translate("aperioClass", "Slice", 0));
#ifndef QT_NO_TOOLTIP
        btnBackground->setToolTip(QApplication::translate("aperioClass", "Pick Up Tool (G)", 0));
#endif // QT_NO_TOOLTIP
        btnBackground->setText(QApplication::translate("aperioClass", "Background", 0));
        label_2->setText(QApplication::translate("aperioClass", "Tools", 0));
        label_5->setText(QApplication::translate("aperioClass", "Slide", 0));
        label_7->setText(QApplication::translate("aperioClass", "Spin", 0));
        chkSpread->setText(QApplication::translate("aperioClass", "Spread", 0));
#ifndef QT_NO_TOOLTIP
        btnPlant->setToolTip(QApplication::translate("aperioClass", "Replant Last Tool (Enter)", 0));
#endif // QT_NO_TOOLTIP
        btnPlant->setText(QApplication::translate("aperioClass", "Replant", 0));
        btnInvert->setText(QApplication::translate("aperioClass", "Invert Path", 0));
        menuFile->setTitle(QApplication::translate("aperioClass", "File", 0));
        menuHelp->setTitle(QApplication::translate("aperioClass", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class aperioClass: public Ui_aperioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APERIO_H
