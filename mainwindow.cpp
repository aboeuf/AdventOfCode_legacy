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
#include <QInputDialog>
#include <solvers.h>
#include <jsonhelper.h>
#include <iso646.h>
#include <set>

#include <iostream>


void Configuration::reset()
{
  m_year = 2017;
  m_day = 1;
  m_puzzle_1 = true;
  m_use_last_input = false;
  m_cookies = {{"_ga", ""}, {"_gid", ""}, {"session", ""}};
  m_src_directory.clear();
}

QString Configuration::load(const QString& filepath)
{
  JsonHelper helper;
  QJsonObject parsed;
  if (!helper.read(filepath, parsed))
    return helper.error();
  helper.read(parsed, "src_directory", m_src_directory);
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
  if (not m_src_directory.isEmpty())
    conf.insert("src_directory", QJsonValue{m_src_directory});
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

void MainWindow::onSolved(const QString& output)
{
  if (m_running_solver) {
    disconnect(m_running_solver, SIGNAL(output(QString)), this, SLOT(onOutputReceived(QString)));
    disconnect(m_running_solver, SIGNAL(finished(QString)), this, SLOT(onSolved(QString)));
    m_running_solver = nullptr;
  }
  ui->m_push_button_solve->setText("SOLVE");
  ui->m_push_button_solve->setEnabled(true);
  ui->m_plain_text_edit_solver_output->clear();
  ui->m_plain_text_edit_solver_output->appendPlainText(output);
  ui->m_plain_text_edit_solver_output->moveCursor(QTextCursor::Start);
  ui->m_plain_text_edit_solver_output->ensureCursorVisible();
  on_m_push_button_solver_output_clicked();
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
  if (m_running_solver)
    return;

  if (not m_solvers(ui->m_spin_box_year->value(),
                    ui->m_spin_box_day->value(),
                    ui->m_spin_box_puzzle->value())) {
    QMessageBox msgBox;
    msgBox.setText("Puzzle not implemented");
    msgBox.setInformativeText("Create default files?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if (msgBox.exec() == QMessageBox::Yes)
      onSolved(createDefault());
    else
      onSolved("Not Implemented");
    return;
  }

  ui->m_plain_text_edit_input->clear();
  ui->m_plain_text_edit_solver_output->clear();
  ui->m_plain_text_edit_program_output->clear();
  m_running_solver = m_solvers(ui->m_spin_box_year->value(),
                               ui->m_spin_box_day->value(),
                               ui->m_spin_box_puzzle->value());
  ui->m_push_button_solve->setText("RUNNING");
  ui->m_push_button_solve->setEnabled(false);
  if (!ui->m_check_box_use_last_input->isChecked())
    downloadPuzzleInput();
  else {
    QFile file(m_dir_path + "last_input.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      ui->m_plain_text_edit_input->clear();
      ui->m_plain_text_edit_input->appendPlainText(QString(file.readAll()));
      ui->m_plain_text_edit_input->moveCursor(QTextCursor::Start);
      ui->m_plain_text_edit_input->ensureCursorVisible();
      file.close();
      solve();
    } else
      downloadPuzzleInput();
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

void MainWindow::on_m_push_button_solver_output_clicked()
{
  QGuiApplication::clipboard()->setText(ui->m_plain_text_edit_solver_output->toPlainText());
}

void MainWindow::on_m_push_button_program_output_clicked()
{
  QGuiApplication::clipboard()->setText(ui->m_plain_text_edit_program_output->toPlainText());
}

void MainWindow::on_m_push_button_sources_clicked()
{
  setSources();
}

void MainWindow::onOutputReceived(const QString& output)
{
  ui->m_plain_text_edit_program_output->appendPlainText(output);
  ui->m_plain_text_edit_program_output->moveCursor(QTextCursor::Start);
  ui->m_plain_text_edit_program_output->ensureCursorVisible();
}

void MainWindow::solve()
{
  m_running_solver = m_solvers(ui->m_spin_box_year->value(),
                               ui->m_spin_box_day->value(),
                               ui->m_spin_box_puzzle->value());
  if (m_running_solver) {
    connect(m_running_solver, SIGNAL(output(QString)), this, SLOT(onOutputReceived(QString)));
    connect(m_running_solver, SIGNAL(finished(QString)), this, SLOT(onSolved(QString)));
    m_running_solver->solve(ui->m_plain_text_edit_input->toPlainText());
  }
  else
    onSolved("Not Implemented");
}

void MainWindow::downloadPuzzleInput()
{
  QNetworkRequest request;
  request.setUrl(QUrl(QString("https://adventofcode.com/%1/day/%2/input")
                      .arg(ui->m_spin_box_year->value())
                      .arg(ui->m_spin_box_day->value())));
  m_config.setCookies(request);
  m_manager->get(request);
}

QString MainWindow::createDefault()
{
  const auto year = ui->m_spin_box_year->value();
  const auto day = ui->m_spin_box_day->value();
  if (m_solvers(year, day, 0) or m_solvers(year, day, 1))
    return "Error: existing implementation has been found";
  while (m_config.m_src_directory.isEmpty() or
         not QDir(m_config.m_src_directory).exists()) {
    if (not setSources())
      return "Error: failed to set sources directory";
  }
  const auto rootpath =  QDir(m_config.m_src_directory).absolutePath();
  const auto dirpath = rootpath + QString("/%1").arg(year);
  if (not QDir().mkpath(dirpath))
    return "Error: cannot create directory " + dirpath;

  QDir dir(dirpath);
  QString header = QString("puzzle_%1_%2.h").arg(year).arg(day, 2, 10, QChar('0'));
  if (dir.exists(header))
    return "Error: " + header + " already exists";
  QString implem = QString("puzzle_%1_%2.cpp").arg(year).arg(day, 2, 10, QChar('0'));
  if (dir.exists(implem))
    return "Error: " + implem + " already exists";

  QFile header_file(dir.absolutePath() + "/" + header);
  if (header_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&header_file);
    out << QString("#pragma once\n"
                   "#include <solvers.h>\n"
                   "\n"
                   "class Solver_%1_%2_1 : public Solver\n"
                   "{\n"
                   "public:\n"
                   "  void solve(const QString& input) override;\n"
                   "};\n"
                   "\n"
                   "class Solver_%1_%2_2 : public Solver\n"
                   "{\n"
                   "public:\n"
                   "  void solve(const QString& input) override;\n"
                   "};\n").arg(year).arg(day, 2, 10, QChar('0'));
  } else
    return "Error: cannot create file " + header;

  QFile implem_file(dir.absolutePath() + "/" + implem);
  if (implem_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&implem_file);
    out << QString("#include <%1/puzzle_%1_%2.h>\n"
                   "#include <common.h>\n"
                   "\n"
                   "void Solver_%1_%2_1::solve(const QString& input)\n"
                   "{\n"
                   "  emit output(input);\n"
                   "  emit finished(\"Default\");\n"
                   "}\n"
                   "\n"
                   "void Solver_%1_%2_2::solve(const QString& input)\n"
                   "{\n"
                   "  emit output(input);\n"
                   "  emit finished(\"Default\");\n"
                   "}\n").arg(year).arg(day, 2, 10, QChar('0'));
  } else
    return "Error: cannot create file " + implem;

  QFile event_file(dir.absolutePath() + QString("/event_%1.h").arg(year));
  if (event_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&event_file);
    for (const auto& file : dir.entryList())
      if (file.startsWith("puzzle_") and file.endsWith(".h"))
        out << QString("#include <%1/%2>\n").arg(year).arg(file);
  } else
    return "Error: cannot create write in file " + QString("event_%1.h").arg(year);

  QFile pro_file_in(rootpath + "/AdventOfCode.pro");
  QStringList begin;
  std::set<std::string> sources, headers, forms;
  if (pro_file_in.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&pro_file_in);
    auto push_in_begin = true;
    while (!in.atEnd()) {
      QString line = in.readLine();
      if (line.contains("SOURCES") or
          line.contains("HEADERS") or
          line.contains("FORMS")) {
        push_in_begin = false;
        while (not begin.empty() and begin.back().isEmpty())
          begin.pop_back();
      } else {
        if (push_in_begin) {
          line.remove('\n');
          begin << line;
        } else {
          line.remove(' ');
          line.remove('\\');
          if (line.contains(".cpp"))
            sources.insert(line.toStdString());
          else if (line.contains(".h"))
            headers.insert(line.toStdString());
          else if (line.contains(".ui"))
            forms.insert(line.toStdString());
        }
      }
    }
  } else
    return "Error: cannot read file AdventOfCode.pro";

  sources.insert(QString("%1/puzzle_%1_%2.cpp").arg(year).arg(day, 2, 10, QChar('0')).toStdString());
  headers.insert(QString("%1/puzzle_%1_%2.h").arg(year).arg(day, 2, 10, QChar('0')).toStdString());

  QFile pro_file_out(rootpath + "/AdventOfCode.pro");
  if (pro_file_out.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&pro_file_out);
    for (const auto& line : begin)
      out << line << "\n";
    const auto write = [&out](const auto& prompt, const auto& data) {
      out << "\n" << prompt << " += \\\n";
      for (auto it = std::begin(data); it != std::end(data); ++it) {
        out << "    " << it->c_str();
        if (std::next(it) != std::end(data))
          out << " \\";
        out << "\n";
      }
    };
    write("SOURCES", sources);
    write("HEADERS", headers);
    write("FORMS", forms);
  } else
    return "Error: cannot write in file AdventOfCode.pro";

  std::set<std::string> includes;
  for (auto y = ui->m_spin_box_year->minimum(); y <= ui->m_spin_box_year->maximum(); ++y) {
    auto filename = QString("%1/event_%1.h").arg(y);
    if (QFile(rootpath + "/" + filename).exists())
      includes.insert(QString("#include <%1>").arg(filename).toStdString());
  }

  std::set<std::string> solvers;
  for (auto y : m_solvers.m_solvers.keys())
    for (auto d : m_solvers.m_solvers[y].keys())
      for (auto p : m_solvers.m_solvers[y][d].keys())
        solvers.insert(QString("  m_solvers[%1][%2][%3] = new Solver_%1_%4_%3();")
                       .arg(y).arg(d).arg(p).arg(d, 2, 10, QChar('0')).toStdString());
  for (auto p = 1; p < 3; ++p)
    solvers.insert(QString("  m_solvers[%1][%2][%3] = new Solver_%1_%4_%3();")
                   .arg(year).arg(day).arg(p).arg(day, 2, 10, QChar('0')).toStdString());

  QFile solvers_file(rootpath + "/solvers.cpp");
  if (solvers_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&solvers_file);
    out << "#include <solvers.h>\n"
           "#include <mainwindow.h>\n\n";
    for (const auto& include : includes)
      out << include.c_str() << "\n";
    out << "\nSolvers::Solvers()\n{\n";
    for (const auto& solver : solvers)
      out << solver.c_str() << "\n";
    out << "}\n"
           "\n"
           "Solvers::~Solvers()\n"
           "{\n"
           "  for (auto year : m_solvers.values())\n"
           "    for (auto day : year.values())\n"
           "      for (auto solver : day.values())\n"
           "        delete solver;\n"
           "}\n"
           "\n"
           "Solver* Solvers::operator()(int year, int day, int puzzle) const\n"
           "{\n"
           "  if (m_solvers.contains(year))\n"
           "    if (m_solvers[year].contains(day))\n"
           "      if (m_solvers[year][day].contains(puzzle))\n"
           "        return m_solvers[year][day][puzzle];\n"
           "  return nullptr;\n"
           "}\n";
  } else
    return "Error: cannot write in file solvers.cpp";

  return QString("Default file created for puzzle_%1_%2").arg(year).arg(day, 2, 10, QChar('0'));
}

bool MainWindow::setSources()
{
  QString current = QDir::homePath();
  if (not m_config.m_src_directory.isEmpty() and QDir(m_config.m_src_directory).exists())
    current = m_config.m_src_directory;
  QString dir = QFileDialog::getExistingDirectory(this, tr("Sources Directory"),
                                                  current,
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty())
    return false;
  m_config.m_src_directory = dir;
  return true;
}


