from PySide6.QtGui import QColor, QPalette
from PySide6.QtWidgets import QDialog, QDialogButtonBox, QMessageBox
from .ui.ui_new_input_dialog import Ui_NewInputDialog


def ask(title, question):
    msg_box = QMessageBox()
    msg_box.setWindowTitle(title)
    msg_box.setText(question)
    msg_box.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
    msg_box.setDefaultButton(QMessageBox.Yes)
    answer = msg_box.exec()
    return answer == QMessageBox.Yes


def warn(message):
    msg_box = QMessageBox()
    msg_box.setWindowTitle("Warning")
    msg_box.setIcon()
    msg_box.setText(message)
    return msg_box.exec()


class NewInputDialog(QDialog, Ui_NewInputDialog):
    def __init__(self, combox_box):
        super().__init__()
        self.setupUi(self)
        self.default_name_changed = False
        for index in range(combox_box.count()):
            self.inputs_combo_box.addItem(combox_box.itemText(index))
        self.inputs_combo_box.setEnabled(False)
        self.inputs_combo_box.currentIndexChanged.connect(self.on_duplicated_input_changed)
        self.duplicate_check_box.stateChanged.connect(self.on_check_box_state_changed)

        self.name_line_edit.setText(self.get_name())
        self.check_valid()

        self.name_line_edit.textChanged.connect(self.on_name_changed)

    def get_default_name(self):
        base_name = self.inputs_combo_box.currentText() if self.duplicate_check_box.isChecked() else "new input"
        if base_name.startswith("[AOC] "):
            base_name = base_name[6:]
        name = base_name
        if self.inputs_combo_box.findText(name) != -1:
            i = 1
            name = base_name + f" ({i})"
            while self.inputs_combo_box.findText(name) != -1:
                i += 1
                name = base_name + f" ({i})"
        return name

    def get_name(self):
        return self.name_line_edit.text() if self.default_name_changed else self.get_default_name()

    def on_check_box_state_changed(self):
        self.inputs_combo_box.setEnabled(self.duplicate_check_box.isChecked())
        self.name_line_edit.blockSignals(True)
        self.name_line_edit.setText(self.get_name())
        self.name_line_edit.blockSignals(False)
        self.check_valid()

    def on_duplicated_input_changed(self):
        self.name_line_edit.blockSignals(True)
        self.name_line_edit.setText(self.get_name())
        self.name_line_edit.blockSignals(False)
        self.check_valid()

    def on_name_changed(self):
        self.default_name_changed = True
        self.check_valid()

    def valid(self):
        name = self.get_name()
        return len(name) > 0 and not name.startswith("[") and self.inputs_combo_box.findText(name) == -1

    def check_valid(self):
        is_valid = self.valid()
        color = "black" if is_valid else "red"
        palette = self.name_line_edit.palette()
        palette.setColor(QPalette.ColorRole.Text, QColor(color))
        self.name_line_edit.setPalette(palette)
        self.button_box.button(QDialogButtonBox.StandardButton.Ok).setEnabled(is_valid)
