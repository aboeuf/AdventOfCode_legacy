#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <solvers.h>
#include <jsonhelper.h>
#include <unordered_map>
#include <display/display.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Configuration
{
  Configuration() = default;
  void reset();
  QString load(const QString& filepath);
  bool save(const QString& filepath) const;
  void updateCookies(QWidget* parent = nullptr);
  void setCookies(QNetworkRequest& request) const;
  int m_year{};
  int m_day{};
  bool m_puzzle_1{};
  bool m_use_last_input{};
  std::unordered_map<std::string, QString> m_cookies{};
  QString m_src_directory{};
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void onSolved(const QString& output);

private slots:
  void replyFinished(QNetworkReply* reply);
  void on_m_push_button_solve_clicked();
  void on_m_push_button_update_cookies_clicked();
  void on_m_spin_box_year_valueChanged(int);
  void on_m_spin_box_day_valueChanged(int);
  void on_m_push_button_input_clicked();
  void on_m_push_button_solver_output_clicked();
  void on_m_push_button_program_output_clicked();
  void on_m_push_button_sources_clicked();
  void onOutputReceived(const QString& output);
  void closeEvent(QCloseEvent *event);

private:
  void solve();
  void downloadPuzzleInput();
  QString createDefault();
  bool setSources();

  Ui::MainWindow *ui;
  Display m_display;
  QNetworkAccessManager *m_manager;
  Solvers m_solvers;
  Solver* m_running_solver{nullptr};
  Configuration m_config;
  QString m_dir_path;
};
#endif // MAINWINDOW_H
