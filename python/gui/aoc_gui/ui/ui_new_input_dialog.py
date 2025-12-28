# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'new_input_dialog.ui'
##
## Created by: Qt User Interface Compiler version 6.10.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QAbstractButton, QApplication, QCheckBox, QComboBox,
    QDialog, QDialogButtonBox, QHBoxLayout, QLabel,
    QLineEdit, QSizePolicy, QVBoxLayout, QWidget)

class Ui_NewInputDialog(object):
    def setupUi(self, NewInputDialog):
        if not NewInputDialog.objectName():
            NewInputDialog.setObjectName(u"NewInputDialog")
        NewInputDialog.resize(315, 95)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(NewInputDialog.sizePolicy().hasHeightForWidth())
        NewInputDialog.setSizePolicy(sizePolicy)
        NewInputDialog.setMinimumSize(QSize(315, 95))
        NewInputDialog.setMaximumSize(QSize(315, 95))
        NewInputDialog.setSizeGripEnabled(False)
        self.main_layout = QVBoxLayout(NewInputDialog)
        self.main_layout.setSpacing(5)
        self.main_layout.setObjectName(u"main_layout")
        self.main_layout.setContentsMargins(5, 5, 5, 5)
        self.name_widget = QWidget(NewInputDialog)
        self.name_widget.setObjectName(u"name_widget")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Maximum)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.name_widget.sizePolicy().hasHeightForWidth())
        self.name_widget.setSizePolicy(sizePolicy1)
        self.name_widget_layout = QHBoxLayout(self.name_widget)
        self.name_widget_layout.setSpacing(5)
        self.name_widget_layout.setObjectName(u"name_widget_layout")
        self.name_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.name_label = QLabel(self.name_widget)
        self.name_label.setObjectName(u"name_label")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Preferred)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.name_label.sizePolicy().hasHeightForWidth())
        self.name_label.setSizePolicy(sizePolicy2)

        self.name_widget_layout.addWidget(self.name_label)

        self.name_line_edit = QLineEdit(self.name_widget)
        self.name_line_edit.setObjectName(u"name_line_edit")

        self.name_widget_layout.addWidget(self.name_line_edit)


        self.main_layout.addWidget(self.name_widget)

        self.duplicate_widget = QWidget(NewInputDialog)
        self.duplicate_widget.setObjectName(u"duplicate_widget")
        sizePolicy1.setHeightForWidth(self.duplicate_widget.sizePolicy().hasHeightForWidth())
        self.duplicate_widget.setSizePolicy(sizePolicy1)
        self.duplicate_widget_layout = QHBoxLayout(self.duplicate_widget)
        self.duplicate_widget_layout.setSpacing(5)
        self.duplicate_widget_layout.setObjectName(u"duplicate_widget_layout")
        self.duplicate_widget_layout.setContentsMargins(0, 0, 0, 0)
        self.duplicate_check_box = QCheckBox(self.duplicate_widget)
        self.duplicate_check_box.setObjectName(u"duplicate_check_box")
        sizePolicy3 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.duplicate_check_box.sizePolicy().hasHeightForWidth())
        self.duplicate_check_box.setSizePolicy(sizePolicy3)

        self.duplicate_widget_layout.addWidget(self.duplicate_check_box)

        self.inputs_combo_box = QComboBox(self.duplicate_widget)
        self.inputs_combo_box.setObjectName(u"inputs_combo_box")

        self.duplicate_widget_layout.addWidget(self.inputs_combo_box)


        self.main_layout.addWidget(self.duplicate_widget)

        self.button_box = QDialogButtonBox(NewInputDialog)
        self.button_box.setObjectName(u"button_box")
        self.button_box.setOrientation(Qt.Horizontal)
        self.button_box.setStandardButtons(QDialogButtonBox.Cancel|QDialogButtonBox.Ok)

        self.main_layout.addWidget(self.button_box)


        self.retranslateUi(NewInputDialog)
        self.button_box.accepted.connect(NewInputDialog.accept)
        self.button_box.rejected.connect(NewInputDialog.reject)

        QMetaObject.connectSlotsByName(NewInputDialog)
    # setupUi

    def retranslateUi(self, NewInputDialog):
        NewInputDialog.setWindowTitle(QCoreApplication.translate("NewInputDialog", u"New Input", None))
        self.name_label.setText(QCoreApplication.translate("NewInputDialog", u"Name:", None))
        self.duplicate_check_box.setText(QCoreApplication.translate("NewInputDialog", u"Duplicate", None))
    # retranslateUi

