#ifndef PUZZLE_2020_24_DISPLAY_H
#define PUZZLE_2020_24_DISPLAY_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class Puzzle_2020_24_Display;
}

namespace puzzle_2020_24
{
class Lobby;
}

class Puzzle_2020_24_Display : public QWidget
{
  Q_OBJECT

public:
  explicit Puzzle_2020_24_Display(puzzle_2020_24::Lobby* lobby,
                                  QWidget *parent = nullptr);
  ~Puzzle_2020_24_Display();

signals:
  void advanceDays(uint nb_days);

public slots:
  void updateDispay();

private slots:
  void on_m_spin_box_nb_days_valueChanged(int arg1);
  void on_m_push_button_advance_clicked();

private:
  Ui::Puzzle_2020_24_Display *ui;
  puzzle_2020_24::Lobby *m_lobby;
  uint m_days_to_advance{0};
  uint m_day{1};
  QGraphicsScene m_scene;
};

#endif // PUZZLE_2020_24_DISPLAY_H
