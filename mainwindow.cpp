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

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_cookies_names{{"_ga", "_gid", "session"}}
{
  m_cookies_values.resize(m_cookies_names.size());
  ui->setupUi(this);
  QFile file("config");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text) && !file.atEnd()) {
    QByteArray line = file.readLine();
    QStringList data = QString(line).split(',');
    if (data.size() > 0)
      ui->m_spin_box_year->setValue(data[0].toInt());
    if (data.size() > 1)
      ui->m_spin_box_day->setValue(data[1].toInt());
    if (data.size() > 2)
      ui->m_spin_box_puzzle->setValue(data[2].toInt());
    if (data.size() > 3)
      ui->m_check_box_use_last_input->setChecked(static_cast<bool>(data[3].toInt()));
    for (int i = 0; i < static_cast<int>(m_cookies_names.size()); ++i) {
      if (data.size() > i + 4)
        m_cookies_values[i] = data[i + 4];
    }
    file.close();
  }
  for (const auto& cookie : m_cookies_values)
    if (cookie.isEmpty()) {
      on_m_push_button_update_cookies_clicked();
      break;
    }
  m_manager = new QNetworkAccessManager(this);
  connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
  QFile file("config");
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << ui->m_spin_box_year->value() << ","
        << ui->m_spin_box_day->value() << ","
        << ui->m_spin_box_puzzle->value() << ","
        << (ui->m_check_box_use_last_input->isChecked() ? "1" : "0");
    for (const auto& value : m_cookies_values)
      out << "," << value;
    file.close();
  }
  delete m_manager;
  delete ui;
}

void MainWindow::replyFinished(QNetworkReply* reply)
{
  ui->m_plain_text_edit_input->clear();
  QString input(reply->readAll());
  while (!input.isEmpty() && input.back() == '\n')
    input.chop(1);
  ui->m_plain_text_edit_input->appendPlainText(input);
  ui->m_plain_text_edit_input->moveCursor(QTextCursor::Start);
  ui->m_plain_text_edit_input->ensureCursorVisible();
  ui->m_check_box_use_last_input->setChecked(true);
  QFile file("input");
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
    QString cookies;
    for (auto i = 0u; i < m_cookies_names.size(); ++i) {
      cookies += m_cookies_names[i] + "=" + m_cookies_values[i];
      if (i + 1 < m_cookies_names.size())
        cookies += "; ";
    }
    request.setRawHeader("Cookie", cookies.toUtf8());
    m_manager->get(request);
  } else {
    QFile file("input");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      ui->m_plain_text_edit_input->clear();
      ui->m_plain_text_edit_input->appendPlainText(QString(file.readAll()));
      ui->m_plain_text_edit_output->moveCursor(QTextCursor::Start);
      ui->m_plain_text_edit_output->ensureCursorVisible();
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
    db_path = QFileDialog::getOpenFileName(this,
                                           tr("Open Cookies Sqlite Database"),
                                           QDir::homePath(),
                                           tr("Sqlite file (*.sqlite)"));
  if (db_path.isEmpty())
    return;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(db_path);
  if (db.open()) {
    QSqlQuery query(db);
    for (auto i = 0u; i < m_cookies_names.size(); ++i) {
      const QString query_str = QString("SELECT value FROM moz_cookies "
                                        "WHERE host=\".adventofcode.com\" "
                                        "AND name=\"%1\"").arg(m_cookies_names[i]);
      if(query.exec(query_str)) {
        if (query.next())
          m_cookies_values[i] = query.value(0).toString();
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
  const QString command = "subl " + QFileInfo(QFile("input")).absoluteFilePath();
  if (std::system(command.toStdString().c_str()) != 0)
    QMessageBox(QMessageBox::Warning,
                "Advent Of Code",
                "Can!edit input file with Sublime Text\nCommand: " + command).exec();
}

void MainWindow::on_m_push_button_output_clicked()
{
  QGuiApplication::clipboard()->setText(ui->m_plain_text_edit_output->toPlainText());
}

void MainWindow::solve()
{
  ui->m_plain_text_edit_output->clear();
  ui->m_plain_text_edit_output->appendPlainText(m_solvers(ui->m_spin_box_year->value(),
                                                          ui->m_spin_box_day->value(),
                                                          ui->m_spin_box_puzzle->value() == 1,
                                                          ui->m_plain_text_edit_input->toPlainText()));
  ui->m_plain_text_edit_output->moveCursor(QTextCursor::Start);
  ui->m_plain_text_edit_output->ensureCursorVisible();
  on_m_push_button_output_clicked();
}
