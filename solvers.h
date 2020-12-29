#ifndef SOLVERS_H
#define SOLVERS_H

#include <QMap>
#include <QString>
#include <QObject>
#include <2019/intcodecomputer.h>

class Solver : public QObject
{
  Q_OBJECT

signals:
  void finished(const QString& output);
  void askInput(const QString& invite);
  void output(const QString& data);

public:
  virtual ~Solver() {}
  virtual void solve(const QString& input) = 0;

public slots:
  virtual void onInputReceived(const QString&);
};

class IntcodeComputerUsingSolver : public Solver
{
  Q_OBJECT

public:
  virtual ~IntcodeComputerUsingSolver();

signals:
  void integerInputReceived(int input);

public slots:
  void onInputReceived(const QString& input) override;
  virtual void onComputerStopped() = 0;

protected:
  event_2019::IntcodeComputer* m_computer{nullptr};
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

#endif // SOLVERS_H
