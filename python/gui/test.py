from bs4 import BeautifulSoup


def get_html():
    with open("/home/aboeuf/Documents/example_2015_01.txt", "r") as file:
        html_content = file.read()
        file.close()
        return html_content


soup = BeautifulSoup(get_html(), "html.parser")

main = soup.find_all("main")[0]
