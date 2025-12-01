#pragma once

#include <QBrush>
#include <QMap>
#include <QObject>
#include <QPen>
#include <QRect>
#include <QString>
#include <vector>

template <typename ObjectType> struct DisplayObject {

  DisplayObject() = default;

  DisplayObject(ObjectType &&object, QPen &&pen, QBrush &&brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(ObjectType &&object, QPen &&pen, const QBrush &brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(ObjectType &&object, const QPen &pen, QBrush &&brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(ObjectType &&object, const QPen &pen, const QBrush &brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(const ObjectType &object, QPen &&pen, QBrush &&brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(const ObjectType &object, QPen &&pen, const QBrush &brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(const ObjectType &object, const QPen &pen, QBrush &&brush)
      : object{object}, pen{pen}, brush{brush} {}

  DisplayObject(const ObjectType &object, const QPen &pen, const QBrush &brush)
      : object{object}, pen{pen}, brush{brush} {}

  ObjectType object;
  QPen pen;
  QBrush brush;
};

struct DisplayData {
  std::vector<DisplayObject<QRectF>> rectangles;
};

Q_DECLARE_METATYPE(DisplayData)

class Solver : public QObject {
  Q_OBJECT

signals:
  void finished(const QString &output);
  void output(const QString &data);
  void display(const DisplayData &data);

public:
  virtual ~Solver() {}
  virtual void solve(const QString &input) = 0;
};

struct Solvers {
  Solvers();
  ~Solvers();

  Solver *operator()(int year, int day, int puzzle) const;
  QMap<int, QMap<int, QMap<int, Solver *>>> m_solvers;
};
