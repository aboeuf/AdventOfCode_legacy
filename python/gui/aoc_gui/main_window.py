from .config import Configuration
from .dialogs import NewInputDialog, ask, warn
from .events import Events
from .icons import Icons
import os
from .paths import get_parent_dirpath
from PySide6.QtCore import QTimer
from PySide6.QtGui import QColor, QKeySequence, QPalette
from PySide6.QtWidgets import QFileDialog, QMainWindow
from .session import Session
from .solver import solve
from .ui.ui_main_window import Ui_MainWindow


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setupUi(self)

        self.nb_examples = 0
        self.output = ""
        self.input = ""
        self.custom_input = False

        self.config = Configuration()
        self.session = Session(self.config)

        if self.config["session_path"] is not None:
            self.session.load(self.config["session_path"])

        if self.config["year"] is None:
            self.config["year"] = min(self.session.events.keys())

        if self.config["day"] is None:
            self.config["day"] = min(self.session.events[self.config["year"]].keys())

        self.setWindowTitle(str(self.session))

        self.icons = Icons()
        self.setWindowIcon(self.icons["main"])
        self.refresh_push_button.setIcon(self.icons["refresh"])
        self.new_problem_push_button.setIcon(self.icons["new"])
        self.copy_part_one_push_button.setIcon(self.icons["copy"])
        self.copy_part_two_push_button.setIcon(self.icons["copy"])
        self.new_input_push_button.setIcon(self.icons["new"])
        self.delete_input_push_button.setIcon(self.icons["delete"])

        for year in sorted(self.session.events.keys()):
            self.year_combo_box.addItem(str(year))
        year_index = self.year_combo_box.findText(str(self.config["year"]))
        if year_index < 0:
            raise Exception(
                f'cannot find text "{self.config["year"]}" in year_combo_box'
            )
        self.year_combo_box.setCurrentIndex(year_index)
        self.inputs_combo_box.addItem("[AOC] perso")
        self.io_combo_box.addItem("Input")
        self.io_combo_box.addItem("Output")

        self.year_combo_box.currentIndexChanged.connect(
            lambda: self.on_year_changed(None)
        )
        self.day_combo_box.currentIndexChanged.connect(
            lambda: self.on_day_changed(False)
        )
        self.io_combo_box.currentIndexChanged.connect(self.on_io_changed)
        self.inputs_combo_box.currentIndexChanged.connect(self.on_input_changed)
        self.refresh_push_button.clicked.connect(lambda: self.on_day_changed(True))
        self.solve_push_button.clicked.connect(self.solve)
        self.action_open.triggered.connect(self.load_session)
        self.action_save.triggered.connect(self.save_session)
        self.action_save_as.triggered.connect(self.save_session_as)
        self.new_input_push_button.clicked.connect(self.new_input)
        self.delete_input_push_button.clicked.connect(self.delete_input)
        self.io_plain_text_edit.textChanged.connect(self.on_custom_input_text_changed)

        self.action_open.setShortcut(QKeySequence("Ctrl+O"))
        self.action_save.setShortcut(QKeySequence("Ctrl+S"))

        self.on_year_changed(self.config["day"])

    def closeEvent(self, event):
        self.config.save()
        if self.session.modified:
            if ask(
                "The session has been modified.", "Do you want to save your changes?"
            ):
                if self.config["session_path"] is None:
                    self.save_session_as()
                else:
                    self.save_session()
        super().closeEvent(event)

    def current_problem(self):
        return self.session.events[self.config["year"]][self.config["day"]]

    def on_year_changed(self, day):
        self.config["year"] = int(self.year_combo_box.currentText())
        self.day_combo_box.blockSignals(True)
        while self.day_combo_box.count():
            self.day_combo_box.removeItem(0)
        for day_key in sorted(self.session.events[self.config["year"]].keys()):
            self.day_combo_box.addItem(str(day_key))
        if day is None:
            day = min(self.session.events[self.config["year"]].keys())
        day_index = self.day_combo_box.findText(str(day))
        if day_index < 0:
            raise Exception(f'cannot find text "{day}" in day_combo_box')
        self.day_combo_box.setCurrentIndex(day_index)
        self.day_combo_box.blockSignals(False)
        self.on_day_changed(False)

    def show_input(self):
        self.io_combo_box.blockSignals(True)
        self.io_combo_box.setCurrentIndex(0)
        self.io_combo_box.blockSignals(False)
        self.on_io_changed()

    def show_output(self):
        self.io_combo_box.blockSignals(True)
        self.io_combo_box.setCurrentIndex(1)
        self.io_combo_box.blockSignals(False)
        self.on_io_changed()

    def on_io_changed(self):
        is_input = self.io_combo_box.currentIndex() == 0
        self.inputs_combo_box.setEnabled(is_input)
        self.new_input_push_button.setEnabled(is_input)
        self.delete_input_push_button.setEnabled(is_input)
        self.io_plain_text_edit.blockSignals(True)
        self.io_plain_text_edit.clear()
        if is_input:
            self.io_plain_text_edit.appendPlainText(self.input)
            self.io_plain_text_edit.setReadOnly(not self.custom_input)
            self.delete_input_push_button.setEnabled(self.custom_input)
        else:
            self.io_plain_text_edit.appendPlainText(self.output)
            self.io_plain_text_edit.setReadOnly(True)
        self.io_plain_text_edit.blockSignals(False)

    def on_day_changed(self, force_refresh):
        self.config["day"] = int(self.day_combo_box.currentText())
        problem = self.current_problem()
        if force_refresh or problem.html is None:
            self.refresh_problem()
        else:
            self.web_engine_view.setHtml(problem.html)
        self.part_one_line_edit.clear()
        self.part_two_line_edit.clear()
        self.io_plain_text_edit.blockSignals(True)
        self.io_plain_text_edit.clear()
        self.io_plain_text_edit.blockSignals(False)
        self.input = ""
        self.output = ""
        self.show_input()
        self.update_inputs()

    def update_inputs(self, set_to_custom=None):
        problem = self.current_problem()
        self.inputs_combo_box.blockSignals(True)
        while self.inputs_combo_box.count() > 1:
            self.inputs_combo_box.removeItem(self.inputs_combo_box.count() - 1)
        self.nb_examples = len(problem.aoc_example_inputs)
        if self.nb_examples > 1:
            for i in range(self.nb_examples):
                self.inputs_combo_box.addItem(f"[AOC] example #{i + 1}")
        elif self.nb_examples > 0:
            self.inputs_combo_box.addItem(f"[AOC] example")
        for name in sorted(problem.custom_inputs.keys()):
            self.inputs_combo_box.addItem(name)
        if set_to_custom is None:
            self.inputs_combo_box.setCurrentIndex(0)
        else:
            index = self.inputs_combo_box.findText(set_to_custom)
            if index < 0:
                self.inputs_combo_box.setCurrentIndex(0)
            else:
                self.inputs_combo_box.setCurrentIndex(index)
        self.inputs_combo_box.blockSignals(False)
        self.on_input_changed()

    def on_input_changed(self):
        self.part_one_line_edit.clear()
        self.part_two_line_edit.clear()
        self.io_plain_text_edit.blockSignals(True)
        self.io_plain_text_edit.clear()
        self.io_plain_text_edit.blockSignals(False)
        self.input = ""
        self.output = ""
        self.custom_input = self.inputs_combo_box.currentIndex() > self.nb_examples
        self.delete_input_push_button.setEnabled(self.custom_input)
        self.read_only = not self.custom_input
        problem = self.current_problem()
        if self.inputs_combo_box.currentIndex() == 0:
            self.input = problem.aoc_personal_input
        elif self.custom_input:
            self.input = problem.custom_inputs[self.inputs_combo_box.currentText()]
        else:
            self.input = problem.aoc_example_inputs[
                self.inputs_combo_box.currentIndex() - 1
            ]
        self.show_input()

    def touch_session(self):
        self.session.modified = True
        self.setWindowTitle(str(self.session))

    def refresh_problem(self):
        problem = self.current_problem()
        problem.refresh()
        self.touch_session()
        if problem.error is None:
            self.web_engine_view.setHtml(problem.html)
        else:
            self.web_engine_view.setHtml(f"<html>{problem.error}<html>")

    def solve(self):
        self.part_one_line_edit.clear()
        self.part_two_line_edit.clear()
        self.io_plain_text_edit.blockSignals(True)
        self.io_plain_text_edit.clear()
        self.io_plain_text_edit.blockSignals(False)
        self.output = ""

        try:
            result = solve(self.config["year"], self.config["day"], self.input)
        except Exception as exc:
            self.output = str(exc)
            self.show_output()
            return

        self.part_one_line_edit.setText(result.part_one_solution)
        self.part_two_line_edit.setText(result.part_two_solution)
        self.output = "\n".join(result.output())
        self.show_output()

        problem = self.current_problem()
        widgets = [self.part_one_line_edit, self.part_two_line_edit]
        for i in range(2):
            if self.inputs_combo_box.currentIndex() != 0 or i >= len(problem.answers):
                color = "black"
            elif widgets[i].text() == problem.answers[i]:
                color = "green"
            else:
                color = "red"
            palette = widgets[i].palette()
            palette.setColor(QPalette.ColorRole.Text, QColor(color))
            widgets[i].setPalette(palette)

    def save_session_as(self):
        dirpath = (
            os.getenv("HOME")
            if self.config["session_path"] is None
            else get_parent_dirpath(self.config["session_path"])
        )
        filepath = QFileDialog.getSaveFileName(
            parent=self, caption="Save AOC Session", dir=dirpath, filter="JSON (*.json)"
        )[0]
        if len(filepath) == 0:
            return
        self.session.save_as(filepath)
        self.setWindowTitle(str(self.session))

    def save_session(self):
        if self.config["session_path"] is None:
            self.save_session_as()
        else:
            self.session.save()
            self.setWindowTitle(str(self.session))

    def load_session(self):
        dirpath = (
            os.getenv("HOME")
            if self.config["session_path"] is None
            else get_parent_dirpath(self.config["session_path"])
        )
        filepath = QFileDialog.getOpenFileName(
            parent=self, caption="Open AOC Session", dir=dirpath, filter="JSON (*.json)"
        )[0]
        if len(filepath) == 0:
            return
        self.session.load(filepath)
        self.setWindowTitle(str(self.session))

    def new_input(self):
        dialog = NewInputDialog(self.inputs_combo_box)
        if not dialog.exec():
            return
        name = dialog.get_name()
        if not dialog.valid():
            return warn(f'Invalid input name "{name}".')
        problem = self.current_problem()
        if name in problem.custom_inputs:
            return warn(f'Invalid with name "{name}" exists already.')
        input = ""
        if dialog.duplicate_check_box.isChecked():
            if dialog.inputs_combo_box.currentIndex() == 0:
                input = problem.aoc_personal_input
            elif dialog.inputs_combo_box.currentIndex() - 1 < len(problem.aoc_example_inputs):
                input = problem.aoc_example_inputs[dialog.inputs_combo_box.currentIndex() - 1]
            else:
                input = problem.custom_inputs[dialog.inputs_combo_box.currentText()]
        problem.custom_inputs[name] = input
        self.update_inputs(set_to_custom=name)
        self.touch_session()

    def delete_input(self):
        problem = self.current_problem()
        index = self.inputs_combo_box.currentIndex()
        if not self.custom_input or index <= len(problem.aoc_example_inputs):
            return warn("Can only delete a custom input.")
        if not ask("Delete current input?", "Are you sure?"):
            return
        del problem.custom_inputs[self.inputs_combo_box.currentText()]
        self.inputs_combo_box.blockSignals(True)
        self.inputs_combo_box.removeItem(index)
        self.inputs_combo_box.blockSignals(False)
        self.update_inputs()
        self.touch_session()

    def on_custom_input_text_changed(self):
        if self.custom_input:
            problem = self.current_problem()
            self.input = self.io_plain_text_edit.toPlainText()
            problem.custom_inputs[self.inputs_combo_box.currentText()] = self.input
            self.touch_session()
