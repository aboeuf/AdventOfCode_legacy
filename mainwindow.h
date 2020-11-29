#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <solvers.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void replyFinished(QNetworkReply* reply);
  void on_m_push_button_solve_clicked();
  void on_m_push_button_update_cookies_clicked();
  void on_m_spin_box_year_valueChanged(int);
  void on_m_spin_box_day_valueChanged(int);
  void on_m_push_button_input_clicked();
  void on_m_push_button_output_clicked();

private:
  void solve();
  Ui::MainWindow *ui;
  QNetworkAccessManager *m_manager;
  std::vector<QString> m_cookies_names;
  std::vector<QString> m_cookies_values;
  Solvers m_solvers;
};
#endif // MAINWINDOW_H
