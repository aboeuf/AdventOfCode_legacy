#pragma once

#include <QMap>
#include <QObject>
#include <QString>
#include <display/display.h>

class Solver : public QObject {
  Q_OBJECT

signals:
  void finished(const QString &output);
  void output(const QString &data);

public:
  virtual ~Solver() {}
  virtual void solve(const QString &input) = 0;
  Display *m_display{nullptr};
};

struct Solvers {
  Solvers();
  ~Solvers();

  Solver *operator()(int year, int day, int puzzle) const;
  QMap<int, QMap<int, QMap<int, Solver *>>> m_solvers;
};
