#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QProcessEnvironment>
#include <QDir>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QClipboard>
#include <solvers.h>
#include <jsonhelper.h>

void Configuration::reset()
{
  m_year = 2017;
  m_day = 1;
  m_puzzle_1 = true;
  m_use_last_input = false;
  m_cookies = {{"_ga", ""}, {"_gid", ""}, {"session", ""}};
}

QString Configuration::load(const QString& filepath)
{
  JsonHelper helper;
  QJsonObject parsed;
  if (!helper.read(filepath, parsed))
    return helper.error();
  if (!helper.read(parsed, "year", m_year))
    return "Cannot parse field \"year\"\n" + helper.error();
  if (!helper.read(parsed, "day", m_day))
    return "Cannot parse field \"day\"\n" + helper.error();
  if (!helper.read(parsed, "puzzle_1", m_puzzle_1))
    return "Cannot parse field \"puzzle_1\"\n" + helper.error();
  if (!helper.read(parsed, "use_last_input", m_use_last_input))
    return "Cannot parse field \"use_last_input\"\n" + helper.error();
  QJsonObject cookies;
  if (!helper.read(parsed, "cookies", cookies))
    return "Cannot parse field \"cookies\"\n" + helper.error();
  QString str;
  if (!helper.read(cookies, "_ga", str))
    return "Cannot parse cookie \"_ga\"\n" + helper.error();
  m_cookies["_ga"] = str;
  if (!helper.read(cookies, "_gid", str))
    return "Cannot parse cookie \"_gid\"\n" + helper.error();
  m_cookies["_gid"] = str;
  if (!helper.read(cookies, "session", str))
    return "Cannot parse cookie \"session\"\n" + helper.error();
  m_cookies["session"] = str;
  return QString{};
}

bool Configuration::save(const QString& filepath) const
{
  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;
  QJsonObject conf;
  conf.insert("year", QJsonValue{m_year});
  conf.insert("day", QJsonValue{m_day});
  conf.insert("puzzle_1", QJsonValue{m_puzzle_1});
  conf.insert("use_last_input", QJsonValue{m_use_last_input});
  QJsonObject cookies;
  for (auto it = m_cookies.begin(); it != m_cookies.end(); ++it)
    cookies.insert(QString{it->first.c_str()}, QJsonValue{it->second});
  conf.insert("cookies", cookies);
  QTextStream out(&file);
  out << QJsonDocument(conf).toJson();
  file.close();
  return true;
}

void Configuration::updateCookies(QWidget* parent)
{
#ifdef WIN32
  QString appdata = QProcessEnvironment::systemEnvironment().value("APPDATA");
  appdata.replace("\\", "/");
  if (!appdata.endsWith("/"))
    appdata += "/";
  QDir firefox(appdata + "Mozilla/Firefox/Profiles");
#else
  QDir firefox(QDir::homePath() + "/.mozilla/firefox");
#endif
  QString db_path;
  for (QFileInfo& di : firefox.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
    for (QFileInfo& fi : QDir(di.absoluteFilePath()).entryInfoList(QDir::AllEntries)) {
      if (fi.fileName() == "cookies.sqlite") {
        db_path = fi.absoluteFilePath();
      }
    }
  }
  if (db_path.isEmpty())
    db_path = QFileDialog::getOpenFileName(parent,
                                           "Open Cookies Sqlite Database",
                                           QDir::homePath(),
                                           "Sqlite file (*.sqlite)");
  if (db_path.isEmpty())
    return;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(db_path);
  if (db.open()) {
    QSqlQuery query(db);
    for (auto it = m_cookies.begin(); it != m_cookies.end(); ++it) {
      const QString query_str = QString("SELECT value FROM moz_cookies "
                                        "WHERE host=\".adventofcode.com\" "
                                        "AND name=\"%1\"").arg(it->first.c_str());
      if(query.exec(query_str)) {
        if (query.next())
          m_cookies[it->first] = query.value(0).toString();
        else
          QMessageBox(QMessageBox::Warning,
                      "Advent Of Code",
                      "Query \"" + query_str + "\"\n"
                                               "failed to provide a value.").exec();
      } else
        QMessageBox(QMessageBox::Warning,
                    "Advent Of Code",
                    "Query \"" + query_str + "\"\n" +
                    "failed with error\n" + query.lastError().text()).exec();
    }
    db.close();
  } else
    QMessageBox(QMessageBox::Warning,
                "Advent Of Code",
                "Can!open sqlite database \"" + db_path + "\".\n" +
                "Close Firefox  and retry.").exec();
}

void Configuration::setCookies(QNetworkRequest& request) const
{
  QString cookies;
  for (auto it = m_cookies.begin(); it != m_cookies.end(); ++it) {
    cookies += QString{it->first.c_str()} + "=" + it->second;
    if (std::next(it) != m_cookies.end())
      cookies += "; ";
  }
  request.setRawHeader("Cookie", cookies.toUtf8());
}

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

#ifdef WIN32
  m_dir_path = QProcessEnvironment::systemEnvironment().value("APPDATA");
  m_dir_path.replace("\\", "/");
  if (!m_dir_path.endsWith("/"))
    m_dir_path += "/";
#else
  m_dir_path = QProcessEnvironment::systemEnvironment().value("HOME");
  if (!m_dir_path.endsWith("/"))
    m_dir_path += "/";
  m_dir_path += ".config/";
#endif
  m_dir_path += "AdventOfCode/";
  if (!QDir(m_dir_path).exists())
    QDir().mkdir(m_dir_path);

  if (QFile(m_dir_path + "config.json").exists()) {
    const QString error = m_config.load(m_dir_path + "config.json");
    if (!error.isEmpty()) {
      QMessageBox(QMessageBox::Warning,
                  "Advent Of Code",
                  QString("Failed to load configuration from file \"%1\"\n").arg(m_dir_path + "config.json") + error).exec();
      m_config.reset();
      m_config.updateCookies(this);
    }
  } else {
    m_config.reset();
    m_config.updateCookies(this);
  }

  ui->m_spin_box_year->setValue(m_config.m_year);
  ui->m_spin_box_day->setValue(m_config.m_day);
  ui->m_spin_box_puzzle->setValue(m_config.m_puzzle_1 ? 1 : 2);
  ui->m_check_box_use_last_input->setChecked(m_config.m_use_last_input);

  m_manager = new QNetworkAccessManager(this);
  connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
  m_config.m_year = ui->m_spin_box_year->value();
  m_config.m_day = ui->m_spin_box_day->value();
  m_config.m_puzzle_1 = ui->m_spin_box_puzzle->value() == 1;
  m_config.m_use_last_input = ui->m_check_box_use_last_input->isChecked();
  if (!m_config.save(m_dir_path + "config.json"))
    QMessageBox(QMessageBox::Warning,
                "Advent Of Code",
                QString("Failed to save configuration to file \"%1\"").arg(m_dir_path + "config.json")).exec();
  delete m_manager;
  delete ui;
}

void MainWindow::replyFinished(QNetworkReply* reply)
{
  QString received(reply->readAll());


  ui->m_plain_text_edit_input->clear();
  while (!received.isEmpty() && received.back() == '\n')
    received.chop(1);
  ui->m_plain_text_edit_input->appendPlainText(received);
  ui->m_plain_text_edit_input->moveCursor(QTextCursor::Start);
  ui->m_plain_text_edit_input->ensureCursorVisible();
  ui->m_check_box_use_last_input->setChecked(true);
  QFile file(m_dir_path + "last_input.txt");
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << ui->m_plain_text_edit_input->toPlainText();
    file.close();
  }
  reply->deleteLater();
  solve();

}

void MainWindow::on_m_push_button_solve_clicked()
{
  if (!ui->m_check_box_use_last_input->isChecked()) {
    QNetworkRequest request;
    request.setUrl(QUrl(QString("https://adventofcode.com/%1/day/%2/input")
                        .arg(ui->m_spin_box_year->value())
                        .arg(ui->m_spin_box_day->value())));
    m_config.setCookies(request);
    m_manager->get(request);
  } else {
    QFile file(m_dir_path + "last_input.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      ui->m_plain_text_edit_input->clear();
      ui->m_plain_text_edit_input->appendPlainText(QString(file.readAll()));
      ui->m_plain_text_edit_input->moveCursor(QTextCursor::Start);
      ui->m_plain_text_edit_input->ensureCursorVisible();
      file.close();
      solve();
    } else {
      ui->m_check_box_use_last_input->setChecked(false);
      on_m_push_button_solve_clicked();
    }
  }
}

void MainWindow::on_m_push_button_update_cookies_clicked()
{
  m_config.updateCookies(this);
}

void MainWindow::on_m_spin_box_year_valueChanged(int)
{
  ui->m_check_box_use_last_input->setChecked(false);
}

void MainWindow::on_m_spin_box_day_valueChanged(int)
{
  ui->m_check_box_use_last_input->setChecked(false);
}

void MainWindow::on_m_push_button_input_clicked()
{
  const QString command = "subl " + QFileInfo(QFile(m_dir_path + "last_input.txt")).absoluteFilePath();
  if (std::system(command.toStdString().c_str()) != 0)
    QMessageBox(QMessageBox::Warning,
                "Advent Of Code",
                "Cannot edit input file with Sublime Text\nCommand: " + command).exec();
}

void MainWindow::on_m_push_button_output_clicked()
{
  QGuiApplication::clipboard()->setText(ui->m_plain_text_edit_output->toPlainText());
}

void MainWindow::solve()
{
  ui->m_plain_text_edit_output->clear();
  ui->m_plain_text_edit_output->appendPlainText(m_solvers(ui->m_plain_text_edit_input->toPlainText(),
                                                          ui->m_spin_box_year->value(),
                                                          ui->m_spin_box_day->value(),
                                                          ui->m_spin_box_puzzle->value() == 1));
  ui->m_plain_text_edit_output->moveCursor(QTextCursor::Start);
  ui->m_plain_text_edit_output->ensureCursorVisible();
  on_m_push_button_output_clicked();
}
