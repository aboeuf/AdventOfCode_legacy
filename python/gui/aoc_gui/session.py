import os
import json
from .events import Events


class Session:
    def __init__(self, config):
        self.config = config
        self.events = Events()
        self.modified = False

    def __str__(self):
        filename = "unsaved"
        return "Advent of Code ({}{})".format(
            "* " if self.modified else "",
            (
                ""
                if self.config["session_path"] is None
                else self.config["session_path"].split("/")[-1].split(".")[0]
            ),
        )

    def load(self, filepath):
        file = open(filepath, "r")
        parsed_data = json.loads(file.read())
        file.close()
        self.config["session_path"] = filepath
        for parsed_key, parsed_value in parsed_data.items():
            if parsed_key == "events":
                for year, parsed_event in parsed_value.items():
                    year_int = int(year)
                    if year_int not in self.events:
                        self.events[year_int] = Event(year)
                    event = self.events[year_int]
                    for day, parsed_problem in parsed_event.items():
                        day_int = int(day)
                        if day_int not in event:
                            event[day_int] = Problem(year, day)
                        problem = event[day_int]
                        problem.html = parsed_problem["html"]
                        problem.answers = parsed_problem["answers"]
                        problem.aoc_personal_input = parsed_problem[
                            "aoc_personal_input"
                        ]
                        if "aoc_example_inputs" in parsed_problem:
                            problem.aoc_example_inputs = parsed_problem[
                                "aoc_example_inputs"
                            ]
                        if "custom_inputs" in parsed_problem:
                            problem.custom_inputs = parsed_problem[
                                "custom_inputs"
                            ]
        self.modified = False

    def export(self, filepath):
        file = open(filepath, "w")
        data = {
            "events": {},
        }
        for year, event in self.events.items():
            if len(event) == 0:
                continue
            if year not in data:
                data["events"][year] = {}
            for day, problem in event.items():
                if problem.html is not None:
                    data["events"][year][day] = {
                        "html": problem.html,
                        "answers": problem.answers,
                        "aoc_personal_input": problem.aoc_personal_input,
                    }
                    if len(problem.aoc_example_inputs):
                        data["events"][year][day]["aoc_example_inputs"] = problem.aoc_example_inputs
                    if len(problem.custom_inputs):
                        data["events"][year][day]["custom_inputs"] = problem.custom_inputs
            if len(data["events"][year]) == 0:
                del data["events"][year]
        if len(data["events"]) == 0:
            del data["events"]
        file = open(filepath, "w")
        file.write(json.dumps(data, indent=4, sort_keys=True))
        file.close()

    def save(self):
        self.export(self.config["session_path"])
        self.modified = False

    def save_as(self, filepath):
        self.config["session_path"] = filepath
        self.save()
