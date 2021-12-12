#pragma once

#include <QMap>
#include <QString>
#include <QObject>
#include <2019/intcodecomputer.h>

class Solver : public QObject
{
  Q_OBJECT

signals:
  void finished(const QString& output);
  void output(const QString& data);

public:
  virtual ~Solver() {}
  virtual void solve(const QString& input) = 0;
};

class Solvers
{
public:
  Solvers();
  ~Solvers();

  Solver* operator()(int year, int day, int puzzle) const;

private:
  QMap<int, QMap<int, QMap<int, Solver*>>> m_solvers;
};
