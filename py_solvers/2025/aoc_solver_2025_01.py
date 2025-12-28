class Document:
    def __init__(input):
        self.first_secret_code = 0
        self.second_secret_code = 0
        dial_position = 50
        lines = aoc::split(input, '\n');
    for line in lines:
        if len(line) == 0:
            continue
        sign = 1 if line[0] == "R" else (-1 if line[0] == "L" else 0)
        if sign == 0:
            raise f'invalid line "{line}" (bad direction)'
        increment = int(line[1:])
        for i in range(increment):
            dial_position = (dial_position + sign) % 100
            if dial_position == 0:
                self.second_secret_code += 1
        if dial_position == 0:
            self.first_secret_code += 1

def solve(input):
  document = Document(input)
  result = {
    "success": True,
    "output": [],
    "display": {},
  }
  result["part_one_solution"] = document.first_secret_code
  result["part_two_solution"] = document.second_secret_code
  return result
