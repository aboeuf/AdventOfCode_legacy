#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <display/display.h>
#include <jsonhelper.h>
#include <leaderboard.h>
#include <solvers.h>
#include <unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct BoardConf {
  BoardConf() = default;
  QString name{"default_name"};
  QMap<unsigned int, qint64> last_updated{};
};

struct Configuration {
  Configuration() = default;
  void reset();
  QString load(const QString &filepath);
  bool save(const QString &filepath) const;
  void updateCookies(QWidget *parent = nullptr);
  void setCookies(QNetworkRequest &request) const;
  int m_year{};
  int m_day{};
  bool m_puzzle_1{};
  bool m_use_last_input{};
  QMap<QString, QString> m_cookies{};
  QMap<QString, BoardConf> m_leaderboards{};
  QString m_src_directory{};
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void onSolved(const QString &output);

private slots:
  void replyFinished(QNetworkReply *reply);
  void on_m_push_button_solve_clicked();
  void on_m_push_button_update_cookies_clicked();
  void on_m_spin_box_year_valueChanged(int);
  void on_m_spin_box_day_valueChanged(int);
  void on_m_push_button_input_clicked();
  void on_m_push_button_solver_output_clicked();
  void on_m_push_button_program_output_clicked();
  void on_m_push_button_sources_clicked();
  void on_m_add_leaderboard_push_button_clicked();
  void on_m_delete_leaderboard_push_button_clicked();
  void on_m_update_leaderboard_push_button_clicked();
  void on_m_leaderboard_combo_box_currentIndexChanged(int);
  void on_m_leaderboard_name_le_editingFinished();
  void on_m_leaderboard_table_widget_currentCellChanged(int row, int column,
                                                        int, int);
  void onOutputReceived(const QString &output);
  void closeEvent(QCloseEvent *event);

private:
  void updateLeaderboard(bool force = false);
  void updateLeaderboardDisplay();
  void fillComboBox();
  void saveConfig();
  void solve();
  void downloadPuzzleInput();
  QString createDefault();
  bool setSources();
  BoardConf &getCurrentBoardConf(QString *ret_id = nullptr);
  QString getCurrentBoardFilepath() const;

  Ui::MainWindow *ui;
  Display m_display;
  QNetworkAccessManager *m_manager;
  Solvers m_solvers;
  Solver *m_running_solver{nullptr};
  Configuration m_config;
  QString m_dir_path;
  QString m_last_selected{""};
  bool m_puzzle_requested = false;
  bool m_leaderboard_requested = false;
  qint64 m_requested_event = 0;
  Leaderboard m_current_board{};
  BoardConf m_error_access;
};
#endif // MAINWINDOW_H
