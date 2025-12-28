from bs4 import BeautifulSoup
from .paths import get_absolute_path
from .wget import DOMAIN_NAME, get_url

ANSWER_FIRST_TEXT = "Your puzzle answer was"
BOTH_PARTS_TEXT = "Both parts of this puzzle are complete"


def read_css():
    css_file = open(get_absolute_path("styles.css"), "r")
    css_data = css_file.read()
    css_file.close()
    return css_data


HTML_HEADER = f"<html><style>{read_css()}</style><body>"


class Problem:
    def __init__(self, year, day):
        self.year = year
        self.day = day
        self.html = None
        self.answers = []
        self.aoc_personal_input = None
        self.aoc_example_inputs = []
        self.custom_inputs = {}
        self.error = None

    def clear(self):
        self.html = None
        self.answers.clear()
        self.aoc_personal_input = None
        self.aoc_example_inputs.clear()
        self.error = None

    def refresh(self):
        self.clear()

        input_response = get_url(
            f"https://{DOMAIN_NAME}/{self.year}/day/{self.day}/input"
        )
        if input_response is None:
            self.aoc_personal_input = "Error: timeout"
        elif input_response.status_code == 200:
            self.aoc_personal_input = input_response.text
        else:
            self.aoc_personal_input = f"Error: {input_response.status_code}"

        problem_response = get_url(f"https://{DOMAIN_NAME}/{self.year}/day/{self.day}")
        if problem_response is None:
            self.error = "timeout"
        elif problem_response.status_code != 200:
            self.error = f"Error: {problem_response.status_code}"
        else:
            soup = BeautifulSoup(problem_response.text, "html.parser")
            main_content = soup.find("main")
            result = [HTML_HEADER, '<div class="status">']
            solved = False
            for child in main_content.children:
                keep_child = False
                if child.name == "p":
                    text = child.get_text(strip=True)
                    both_parts = text.startswith(BOTH_PARTS_TEXT)
                    if both_parts:
                        solved = True
                    is_answer = text.startswith(ANSWER_FIRST_TEXT)
                    if is_answer:
                        grandchildren = [
                            grandchild.get_text(strip=True)
                            for grandchild in child.children
                        ]
                        if (
                            len(grandchildren) != 3
                            or grandchildren[0] != ANSWER_FIRST_TEXT
                            or grandchildren[2] != "."
                        ):
                            raise Exception(
                                f'cannot extract answer from string "{child}"'
                            )
                        self.answers.append(grandchildren[1])
                    keep_child = both_parts or is_answer
                if keep_child or child.name == "article":
                    for a in child.find_all("a"):
                        a.unwrap()
                    result.append(str(child))
            result.append("</body><html>")
            if (
                (not solved and len(self.answers) == 2)
                or (solved and len(self.answers) != 2)
                or len(self.answers) > 2
            ):
                raise Exception("invalid parsing result")
            if len(self.answers) == 0:
                result[1] += "Unsolved"
            else:
                result[1] += f"Part one: {self.answers[0]}"
                if len(self.answers) > 1:
                    result[1] += f"<br>Part two: {self.answers[1]}"
            result[1] += "</div>"
            self.html = "".join(result)
            pre_content = soup.find("pre")
            for input_example in pre_content:
                self.aoc_example_inputs.append(input_example.get_text(strip=True))
