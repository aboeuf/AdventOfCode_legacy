#include <QDebug>
#include <QMap>
#include <solvers/common.h>
#include <solvers/2020/puzzle_2020_04.h>

namespace puzzle_2020_04 {

using Passport = QMap<QString, QString>;

const QStringList FIELDS = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};

QList<Passport> readPassports(const QString &input) {
  QStringList lines = common::splitLines(input);
  QList<Passport> passwords;
  Passport password;
  for (const QString &line : lines) {
    if (line.isEmpty()) {
      if (!password.isEmpty())
        passwords << password;
      password.clear();
    } else {
      QStringList values = common::splitValues(line, ' ');
      for (const QString &value : values) {
        const QStringList key_and_val = common::splitValues(value, ':');
        ;
        if (key_and_val.size() == 2) {
          password.insert(key_and_val[0], key_and_val[1]);
        }
      }
    }
  }
  if (!lines.back().isEmpty() && !password.isEmpty())
    passwords << password;
  return passwords;
}

bool hasAllFields(const Passport &password) {
  for (const QString &field : FIELDS) {
    if (!password.contains(field))
      return false;
  }
  return true;
}

bool checkYear(const Passport &password, const QString &field, int min,
               int max) {
  if (!password.contains(field))
    return false;
  if (password[field].size() != 4)
    return false;
  bool ok;
  int year = password[field].toInt(&ok);
  return ok && min <= year && year <= max;
}

bool checkHeight(const Passport &password) {
  if (!password.contains("hgt"))
    return false;
  QString height_str = password.value("hgt");
  int min, max;
  if (height_str.endsWith("cm")) {
    min = 150;
    max = 193;
  } else if (height_str.endsWith("in")) {
    min = 59;
    max = 76;
  } else
    return false;
  height_str.chop(2);
  bool ok;
  int height = height_str.toInt(&ok);
  return ok && min <= height && height <= max;
}

bool checkHairColor(const Passport &password) {
  if (!password.contains("hcl"))
    return false;
  if (password["hcl"].size() != 7)
    return false;
  if (password["hcl"].front() != '#')
    return false;
  for (int i = 1; i < 7; ++i) {
    if (password["hcl"][i].isDigit())
      continue;
    if (!password["hcl"][i].isLetter())
      return false;
    if (password["hcl"][i].isUpper())
      return false;
    if (password["hcl"][i] < 'a' || 'f' < password["hcl"][i])
      return false;
  }
  return true;
}

bool checkEyeColor(const Passport &password) {
  if (!password.contains("ecl"))
    return false;
  return password["ecl"] == "amb" || password["ecl"] == "blu" ||
         password["ecl"] == "brn" || password["ecl"] == "gry" ||
         password["ecl"] == "grn" || password["ecl"] == "hzl" ||
         password["ecl"] == "oth";
}

bool checkPassportID(const Passport &password) {
  if (!password.contains("pid"))
    return false;
  if (password["pid"].size() != 9)
    return false;
  for (const QChar &c : password["pid"])
    if (!c.isDigit())
      return false;
  return true;
}

bool isValid(const Passport &password) {
  if (!checkYear(password, "byr", 1920, 2002))
    return false;
  if (!checkYear(password, "iyr", 2010, 2020))
    return false;
  if (!checkYear(password, "eyr", 2020, 2030))
    return false;
  if (!checkHeight(password))
    return false;
  if (!checkHairColor(password))
    return false;
  if (!checkEyeColor(password))
    return false;
  if (!checkPassportID(password))
    return false;
  return true;
}

} // namespace puzzle_2020_04

void Solver_2020_04_1::solve(const QString &input) {
  using namespace puzzle_2020_04;
  QList<Passport> passwords = puzzle_2020_04::readPassports(input);
  uint nb_valid = 0;
  for (const Passport &password : passwords)
    if (hasAllFields(password))
      ++nb_valid;
  emit finished(QString::number(nb_valid));
}

void Solver_2020_04_2::solve(const QString &input) {
  using namespace puzzle_2020_04;
  QList<Passport> passwords = puzzle_2020_04::readPassports(input);
  uint nb_valid = 0;
  for (const Passport &password : passwords)
    if (isValid(password))
      ++nb_valid;
  emit finished(QString::number(nb_valid));
}
