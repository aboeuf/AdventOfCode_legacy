#ifndef SOLVERS_H
#define SOLVERS_H

#include <QMap>
#include <QString>
#include <QObject>

class Solver : public QObject
{
  Q_OBJECT

signals:
  void finished(const QString& output) const;

public:
  virtual void solve(const QString& input) const = 0;
};

class Solvers : public QObject
{
  Q_OBJECT

public:
  Solvers();
  ~Solvers();
  void operator()(const QString& input, int year, int day, int puzzle) const;

signals:
  void finished(const QString& output) const;

public slots:
  void onSolved(const QString& output) const;

private:
  QMap<int, QMap<int, QMap<int, Solver*>>> m_solvers;
};

#endif // SOLVERS_H
