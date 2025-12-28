import os
from .paths import SOLVERS_DIR
from .problem import Problem


class Event(dict):
    def __init__(self, year):
        super().__init__()
        self.year = year

    def empty(self):
        return len(self) == 0


class Events(dict):
    def __init__(self):
        super().__init__()
        for event_dir in os.listdir(SOLVERS_DIR):
            abs_path_event_dir = os.path.join(SOLVERS_DIR, event_dir)
            if not os.path.isdir(abs_path_event_dir):
                continue
            try:
                year = int(event_dir)
            except ValueError:
                continue
            self[year] = Event(year)
            for solver_file_name in os.listdir(abs_path_event_dir):
                header = f"solve_{year}_"
                if not solver_file_name.startswith(
                    header
                ) or not solver_file_name.endswith(".cpp"):
                    raise Exception(f"bad solver file name {solver_file_name}")
                day_str = solver_file_name[len(header) : -4]
                try:
                    day = int(day_str)
                except ValueError:
                    raise Exception(
                        f'bad solver file name {solver_file_name} (bad day string "{day_str}")'
                    )
                self[year][day] = Problem(year, day)
            if self[year].empty():
                del self[year]
