import browser_cookie3
import requests

DOMAIN_NAME = "adventofcode.com"
TIMEOUT = 1.0


def get_cookies_from_chrome():
    cookies = dict()
    cookie_jar = browser_cookie3.chrome(domain_name=DOMAIN_NAME)
    for cookie in cookie_jar:
        cookies[cookie.name] = cookie.value
    return cookies


def get_url(url):
    try:
        return requests.get(
            url,
            cookies=get_cookies_from_chrome(),
            timeout=TIMEOUT,
        )
    except ReadTimeout:
        pass
