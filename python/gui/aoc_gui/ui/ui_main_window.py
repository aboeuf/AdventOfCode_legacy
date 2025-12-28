# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'main_window.ui'
##
## Created by: Qt User Interface Compiler version 6.10.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWebEngineWidgets import QWebEngineView
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QLabel,
    QLineEdit, QMainWindow, QMenu, QMenuBar,
    QPlainTextEdit, QPushButton, QSizePolicy, QSpacerItem,
    QSplitter, QTabWidget, QVBoxLayout, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(847, 559)
        self.actionUpdate = QAction(MainWindow)
        self.actionUpdate.setObjectName(u"actionUpdate")
        self.actionLoad = QAction(MainWindow)
        self.actionLoad.setObjectName(u"actionLoad")
        self.actionSave = QAction(MainWindow)
        self.actionSave.setObjectName(u"actionSave")
        self.actionSave_As = QAction(MainWindow)
        self.actionSave_As.setObjectName(u"actionSave_As")
        self.action_open = QAction(MainWindow)
        self.action_open.setObjectName(u"action_open")
        self.action_save = QAction(MainWindow)
        self.action_save.setObjectName(u"action_save")
        self.action_save_as = QAction(MainWindow)
        self.action_save_as.setObjectName(u"action_save_as")
        self.central_widget = QWidget(MainWindow)
        self.central_widget.setObjectName(u"central_widget")
        self.verticalLayout = QVBoxLayout(self.central_widget)
        self.verticalLayout.setSpacing(5)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(5, 5, 5, 5)
        self.tab_widget = QTabWidget(self.central_widget)
        self.tab_widget.setObjectName(u"tab_widget")
        self.problem_tab = QWidget()
        self.problem_tab.setObjectName(u"problem_tab")
        self.problem_tab_layout = QVBoxLayout(self.problem_tab)
        self.problem_tab_layout.setSpacing(5)
        self.problem_tab_layout.setObjectName(u"problem_tab_layout")
        self.problem_tab_layout.setContentsMargins(5, 5, 5, 5)
        self.year_day_selection_widget = QWidget(self.problem_tab)
        self.year_day_selection_widget.setObjectName(u"year_day_selection_widget")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.year_day_selection_widget.sizePolicy().hasHeightForWidth())
        self.year_day_selection_widget.setSizePolicy(sizePolicy)
        self.year_day_selection_widget_layout = QHBoxLayout(self.year_day_selection_widget)
        self.year_day_selection_widget_layout.setSpacing(5)
        self.year_day_selection_widget_layout.setObjectName(u"year_day_selection_widget_layout")
        self.year_day_selection_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.refresh_push_button = QPushButton(self.year_day_selection_widget)
        self.refresh_push_button.setObjectName(u"refresh_push_button")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.refresh_push_button.sizePolicy().hasHeightForWidth())
        self.refresh_push_button.setSizePolicy(sizePolicy1)
        self.refresh_push_button.setMinimumSize(QSize(25, 25))
        self.refresh_push_button.setMaximumSize(QSize(25, 25))

        self.year_day_selection_widget_layout.addWidget(self.refresh_push_button)

        self.new_problem_push_button = QPushButton(self.year_day_selection_widget)
        self.new_problem_push_button.setObjectName(u"new_problem_push_button")
        sizePolicy1.setHeightForWidth(self.new_problem_push_button.sizePolicy().hasHeightForWidth())
        self.new_problem_push_button.setSizePolicy(sizePolicy1)
        self.new_problem_push_button.setMinimumSize(QSize(25, 25))
        self.new_problem_push_button.setMaximumSize(QSize(25, 25))

        self.year_day_selection_widget_layout.addWidget(self.new_problem_push_button)

        self.year_label = QLabel(self.year_day_selection_widget)
        self.year_label.setObjectName(u"year_label")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Maximum)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.year_label.sizePolicy().hasHeightForWidth())
        self.year_label.setSizePolicy(sizePolicy2)

        self.year_day_selection_widget_layout.addWidget(self.year_label)

        self.year_combo_box = QComboBox(self.year_day_selection_widget)
        self.year_combo_box.setObjectName(u"year_combo_box")
        sizePolicy3 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.year_combo_box.sizePolicy().hasHeightForWidth())
        self.year_combo_box.setSizePolicy(sizePolicy3)

        self.year_day_selection_widget_layout.addWidget(self.year_combo_box)

        self.day_label = QLabel(self.year_day_selection_widget)
        self.day_label.setObjectName(u"day_label")
        sizePolicy2.setHeightForWidth(self.day_label.sizePolicy().hasHeightForWidth())
        self.day_label.setSizePolicy(sizePolicy2)

        self.year_day_selection_widget_layout.addWidget(self.day_label)

        self.day_combo_box = QComboBox(self.year_day_selection_widget)
        self.day_combo_box.setObjectName(u"day_combo_box")
        sizePolicy3.setHeightForWidth(self.day_combo_box.sizePolicy().hasHeightForWidth())
        self.day_combo_box.setSizePolicy(sizePolicy3)

        self.year_day_selection_widget_layout.addWidget(self.day_combo_box)

        self.year_day_selection_horizontal_spacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.year_day_selection_widget_layout.addItem(self.year_day_selection_horizontal_spacer)


        self.problem_tab_layout.addWidget(self.year_day_selection_widget)

        self.problem_bottom_widget = QWidget(self.problem_tab)
        self.problem_bottom_widget.setObjectName(u"problem_bottom_widget")
        self.horizontalLayout = QHBoxLayout(self.problem_bottom_widget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.splitter = QSplitter(self.problem_bottom_widget)
        self.splitter.setObjectName(u"splitter")
        self.splitter.setOrientation(Qt.Horizontal)
        self.web_engine_view = QWebEngineView(self.splitter)
        self.web_engine_view.setObjectName(u"web_engine_view")
        sizePolicy4 = QSizePolicy(QSizePolicy.Policy.MinimumExpanding, QSizePolicy.Policy.Preferred)
        sizePolicy4.setHorizontalStretch(0)
        sizePolicy4.setVerticalStretch(0)
        sizePolicy4.setHeightForWidth(self.web_engine_view.sizePolicy().hasHeightForWidth())
        self.web_engine_view.setSizePolicy(sizePolicy4)
        self.web_engine_view.setProperty(u"url", QUrl(u"about:blank"))
        self.splitter.addWidget(self.web_engine_view)
        self.solver_widget = QWidget(self.splitter)
        self.solver_widget.setObjectName(u"solver_widget")
        sizePolicy5 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy5.setHorizontalStretch(0)
        sizePolicy5.setVerticalStretch(0)
        sizePolicy5.setHeightForWidth(self.solver_widget.sizePolicy().hasHeightForWidth())
        self.solver_widget.setSizePolicy(sizePolicy5)
        self.solver_widget_layout = QVBoxLayout(self.solver_widget)
        self.solver_widget_layout.setSpacing(5)
        self.solver_widget_layout.setObjectName(u"solver_widget_layout")
        self.solver_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.solve_widget = QWidget(self.solver_widget)
        self.solve_widget.setObjectName(u"solve_widget")
        self.solve_widget_layout = QHBoxLayout(self.solve_widget)
        self.solve_widget_layout.setSpacing(5)
        self.solve_widget_layout.setObjectName(u"solve_widget_layout")
        self.solve_widget_layout.setContentsMargins(0, 0, 0, 0)

        self.solver_widget_layout.addWidget(self.solve_widget)

        self.solver_selection_widget = QWidget(self.solver_widget)
        self.solver_selection_widget.setObjectName(u"solver_selection_widget")
        self.solver_selection_widget_layout = QHBoxLayout(self.solver_selection_widget)
        self.solver_selection_widget_layout.setSpacing(5)
        self.solver_selection_widget_layout.setObjectName(u"solver_selection_widget_layout")
        self.solver_selection_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.solver_label = QLabel(self.solver_selection_widget)
        self.solver_label.setObjectName(u"solver_label")
        sizePolicy6 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Preferred)
        sizePolicy6.setHorizontalStretch(0)
        sizePolicy6.setVerticalStretch(0)
        sizePolicy6.setHeightForWidth(self.solver_label.sizePolicy().hasHeightForWidth())
        self.solver_label.setSizePolicy(sizePolicy6)

        self.solver_selection_widget_layout.addWidget(self.solver_label)

        self.solvers_combo_box = QComboBox(self.solver_selection_widget)
        self.solvers_combo_box.setObjectName(u"solvers_combo_box")

        self.solver_selection_widget_layout.addWidget(self.solvers_combo_box)

        self.new_solver_push_button = QPushButton(self.solver_selection_widget)
        self.new_solver_push_button.setObjectName(u"new_solver_push_button")
        sizePolicy1.setHeightForWidth(self.new_solver_push_button.sizePolicy().hasHeightForWidth())
        self.new_solver_push_button.setSizePolicy(sizePolicy1)
        self.new_solver_push_button.setMinimumSize(QSize(25, 25))
        self.new_solver_push_button.setMaximumSize(QSize(25, 25))

        self.solver_selection_widget_layout.addWidget(self.new_solver_push_button)

        self.delete_solver_push_button = QPushButton(self.solver_selection_widget)
        self.delete_solver_push_button.setObjectName(u"delete_solver_push_button")
        sizePolicy1.setHeightForWidth(self.delete_solver_push_button.sizePolicy().hasHeightForWidth())
        self.delete_solver_push_button.setSizePolicy(sizePolicy1)
        self.delete_solver_push_button.setMinimumSize(QSize(25, 25))
        self.delete_solver_push_button.setMaximumSize(QSize(25, 25))

        self.solver_selection_widget_layout.addWidget(self.delete_solver_push_button)


        self.solver_widget_layout.addWidget(self.solver_selection_widget)

        self.input_widget = QWidget(self.solver_widget)
        self.input_widget.setObjectName(u"input_widget")
        sizePolicy.setHeightForWidth(self.input_widget.sizePolicy().hasHeightForWidth())
        self.input_widget.setSizePolicy(sizePolicy)
        self.input_widget_layout = QHBoxLayout(self.input_widget)
        self.input_widget_layout.setSpacing(5)
        self.input_widget_layout.setObjectName(u"input_widget_layout")
        self.input_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.io_combo_box = QComboBox(self.input_widget)
        self.io_combo_box.setObjectName(u"io_combo_box")

        self.input_widget_layout.addWidget(self.io_combo_box)

        self.inputs_combo_box = QComboBox(self.input_widget)
        self.inputs_combo_box.setObjectName(u"inputs_combo_box")
        sizePolicy7 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy7.setHorizontalStretch(0)
        sizePolicy7.setVerticalStretch(0)
        sizePolicy7.setHeightForWidth(self.inputs_combo_box.sizePolicy().hasHeightForWidth())
        self.inputs_combo_box.setSizePolicy(sizePolicy7)

        self.input_widget_layout.addWidget(self.inputs_combo_box)

        self.inputs_horizontal_spacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.input_widget_layout.addItem(self.inputs_horizontal_spacer)

        self.new_input_push_button = QPushButton(self.input_widget)
        self.new_input_push_button.setObjectName(u"new_input_push_button")
        sizePolicy1.setHeightForWidth(self.new_input_push_button.sizePolicy().hasHeightForWidth())
        self.new_input_push_button.setSizePolicy(sizePolicy1)
        self.new_input_push_button.setMinimumSize(QSize(25, 25))
        self.new_input_push_button.setMaximumSize(QSize(25, 25))

        self.input_widget_layout.addWidget(self.new_input_push_button)

        self.delete_input_push_button = QPushButton(self.input_widget)
        self.delete_input_push_button.setObjectName(u"delete_input_push_button")
        sizePolicy1.setHeightForWidth(self.delete_input_push_button.sizePolicy().hasHeightForWidth())
        self.delete_input_push_button.setSizePolicy(sizePolicy1)
        self.delete_input_push_button.setMinimumSize(QSize(25, 25))
        self.delete_input_push_button.setMaximumSize(QSize(25, 25))

        self.input_widget_layout.addWidget(self.delete_input_push_button)


        self.solver_widget_layout.addWidget(self.input_widget)

        self.io_plain_text_edit = QPlainTextEdit(self.solver_widget)
        self.io_plain_text_edit.setObjectName(u"io_plain_text_edit")
        sizePolicy8 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Expanding)
        sizePolicy8.setHorizontalStretch(0)
        sizePolicy8.setVerticalStretch(0)
        sizePolicy8.setHeightForWidth(self.io_plain_text_edit.sizePolicy().hasHeightForWidth())
        self.io_plain_text_edit.setSizePolicy(sizePolicy8)
        self.io_plain_text_edit.setReadOnly(True)

        self.solver_widget_layout.addWidget(self.io_plain_text_edit)

        self.solve_push_button = QPushButton(self.solver_widget)
        self.solve_push_button.setObjectName(u"solve_push_button")
        sizePolicy5.setHeightForWidth(self.solve_push_button.sizePolicy().hasHeightForWidth())
        self.solve_push_button.setSizePolicy(sizePolicy5)

        self.solver_widget_layout.addWidget(self.solve_push_button)

        self.part_one_widget = QWidget(self.solver_widget)
        self.part_one_widget.setObjectName(u"part_one_widget")
        sizePolicy.setHeightForWidth(self.part_one_widget.sizePolicy().hasHeightForWidth())
        self.part_one_widget.setSizePolicy(sizePolicy)
        self.part_one_widget_layout = QHBoxLayout(self.part_one_widget)
        self.part_one_widget_layout.setSpacing(5)
        self.part_one_widget_layout.setObjectName(u"part_one_widget_layout")
        self.part_one_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.part_one_label = QLabel(self.part_one_widget)
        self.part_one_label.setObjectName(u"part_one_label")
        sizePolicy6.setHeightForWidth(self.part_one_label.sizePolicy().hasHeightForWidth())
        self.part_one_label.setSizePolicy(sizePolicy6)

        self.part_one_widget_layout.addWidget(self.part_one_label)

        self.part_one_line_edit = QLineEdit(self.part_one_widget)
        self.part_one_line_edit.setObjectName(u"part_one_line_edit")
        sizePolicy7.setHeightForWidth(self.part_one_line_edit.sizePolicy().hasHeightForWidth())
        self.part_one_line_edit.setSizePolicy(sizePolicy7)
        self.part_one_line_edit.setReadOnly(True)

        self.part_one_widget_layout.addWidget(self.part_one_line_edit)

        self.copy_part_one_push_button = QPushButton(self.part_one_widget)
        self.copy_part_one_push_button.setObjectName(u"copy_part_one_push_button")
        sizePolicy1.setHeightForWidth(self.copy_part_one_push_button.sizePolicy().hasHeightForWidth())
        self.copy_part_one_push_button.setSizePolicy(sizePolicy1)
        self.copy_part_one_push_button.setMinimumSize(QSize(25, 25))
        self.copy_part_one_push_button.setMaximumSize(QSize(25, 25))

        self.part_one_widget_layout.addWidget(self.copy_part_one_push_button)


        self.solver_widget_layout.addWidget(self.part_one_widget)

        self.part_two_widget = QWidget(self.solver_widget)
        self.part_two_widget.setObjectName(u"part_two_widget")
        sizePolicy.setHeightForWidth(self.part_two_widget.sizePolicy().hasHeightForWidth())
        self.part_two_widget.setSizePolicy(sizePolicy)
        self.part_two_widget_layout = QHBoxLayout(self.part_two_widget)
        self.part_two_widget_layout.setSpacing(5)
        self.part_two_widget_layout.setObjectName(u"part_two_widget_layout")
        self.part_two_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.part_two_label = QLabel(self.part_two_widget)
        self.part_two_label.setObjectName(u"part_two_label")
        sizePolicy6.setHeightForWidth(self.part_two_label.sizePolicy().hasHeightForWidth())
        self.part_two_label.setSizePolicy(sizePolicy6)

        self.part_two_widget_layout.addWidget(self.part_two_label)

        self.part_two_line_edit = QLineEdit(self.part_two_widget)
        self.part_two_line_edit.setObjectName(u"part_two_line_edit")
        sizePolicy7.setHeightForWidth(self.part_two_line_edit.sizePolicy().hasHeightForWidth())
        self.part_two_line_edit.setSizePolicy(sizePolicy7)
        self.part_two_line_edit.setReadOnly(True)

        self.part_two_widget_layout.addWidget(self.part_two_line_edit)

        self.copy_part_two_push_button = QPushButton(self.part_two_widget)
        self.copy_part_two_push_button.setObjectName(u"copy_part_two_push_button")
        sizePolicy1.setHeightForWidth(self.copy_part_two_push_button.sizePolicy().hasHeightForWidth())
        self.copy_part_two_push_button.setSizePolicy(sizePolicy1)
        self.copy_part_two_push_button.setMinimumSize(QSize(25, 25))
        self.copy_part_two_push_button.setMaximumSize(QSize(25, 25))

        self.part_two_widget_layout.addWidget(self.copy_part_two_push_button)


        self.solver_widget_layout.addWidget(self.part_two_widget)

        self.splitter.addWidget(self.solver_widget)

        self.horizontalLayout.addWidget(self.splitter)


        self.problem_tab_layout.addWidget(self.problem_bottom_widget)

        self.tab_widget.addTab(self.problem_tab, "")
        self.leaderboards_tab = QWidget()
        self.leaderboards_tab.setObjectName(u"leaderboards_tab")
        self.tab_widget.addTab(self.leaderboards_tab, "")

        self.verticalLayout.addWidget(self.tab_widget)

        MainWindow.setCentralWidget(self.central_widget)
        self.menu_bar = QMenuBar(MainWindow)
        self.menu_bar.setObjectName(u"menu_bar")
        self.menu_bar.setGeometry(QRect(0, 0, 847, 22))
        self.menu_file = QMenu(self.menu_bar)
        self.menu_file.setObjectName(u"menu_file")
        MainWindow.setMenuBar(self.menu_bar)

        self.menu_bar.addAction(self.menu_file.menuAction())
        self.menu_file.addAction(self.action_open)
        self.menu_file.addAction(self.action_save)
        self.menu_file.addAction(self.action_save_as)

        self.retranslateUi(MainWindow)

        self.tab_widget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"Advent of Code", None))
        self.actionUpdate.setText(QCoreApplication.translate("MainWindow", u"Update", None))
        self.actionLoad.setText(QCoreApplication.translate("MainWindow", u"Load", None))
        self.actionSave.setText(QCoreApplication.translate("MainWindow", u"Save", None))
        self.actionSave_As.setText(QCoreApplication.translate("MainWindow", u"Save As...", None))
        self.action_open.setText(QCoreApplication.translate("MainWindow", u"Open", None))
        self.action_save.setText(QCoreApplication.translate("MainWindow", u"Save", None))
        self.action_save_as.setText(QCoreApplication.translate("MainWindow", u"Save As...", None))
#if QT_CONFIG(tooltip)
        self.refresh_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Refresh (F5)</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.refresh_push_button.setText("")
#if QT_CONFIG(tooltip)
        self.new_problem_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>New</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.new_problem_push_button.setText("")
        self.year_label.setText(QCoreApplication.translate("MainWindow", u"Year", None))
        self.day_label.setText(QCoreApplication.translate("MainWindow", u"Day", None))
        self.solver_label.setText(QCoreApplication.translate("MainWindow", u"Solver:", None))
#if QT_CONFIG(tooltip)
        self.new_solver_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>New</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.new_solver_push_button.setText("")
#if QT_CONFIG(tooltip)
        self.delete_solver_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Delete</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.delete_solver_push_button.setText("")
#if QT_CONFIG(tooltip)
        self.new_input_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>New</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.new_input_push_button.setText("")
#if QT_CONFIG(tooltip)
        self.delete_input_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Delete</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.delete_input_push_button.setText("")
        self.solve_push_button.setText(QCoreApplication.translate("MainWindow", u"SOLVE", None))
        self.part_one_label.setText(QCoreApplication.translate("MainWindow", u"Part one:", None))
#if QT_CONFIG(tooltip)
        self.copy_part_one_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Copy</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.copy_part_one_push_button.setText("")
        self.part_two_label.setText(QCoreApplication.translate("MainWindow", u"Part two:", None))
#if QT_CONFIG(tooltip)
        self.copy_part_two_push_button.setToolTip(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Copy</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.copy_part_two_push_button.setText("")
        self.tab_widget.setTabText(self.tab_widget.indexOf(self.problem_tab), QCoreApplication.translate("MainWindow", u"Problem", None))
        self.tab_widget.setTabText(self.tab_widget.indexOf(self.leaderboards_tab), QCoreApplication.translate("MainWindow", u"Leaderboards", None))
        self.menu_file.setTitle(QCoreApplication.translate("MainWindow", u"File", None))
    # retranslateUi

