#pragma once

#include <QMap>
#include <QString>
#include <gui/jsonhelper.h>
#include <optional>

struct CompletionStar {
  CompletionStar() = default;
  CompletionStar(const QJsonObject &object);
  qint64 star_index{0};
  qint64 get_star_ts{0};
  bool has_start_index{true};
};

struct CompletionDay {
  CompletionDay() = default;
  CompletionDay(const QJsonObject &object);

  qint64 getFirstTime() const;
  qint64 getSecondTime() const;

  std::optional<CompletionStar> first{std::nullopt};
  std::optional<CompletionStar> second{std::nullopt};
};

struct Member {
  Member() = default;
  Member(const QJsonObject &object);

  QString name{""};
  qint64 local_score{0};
  qint64 global_score{0};
  qint64 stars{0};
  qint64 last_star_ts{0};
  QMap<unsigned int, CompletionDay> completion_day_level{};
};

class Leaderboard {
public:
  Leaderboard() = default;
  Leaderboard(const QString &filepath);

  qint64 ownerID() const;
  QString ownerName() const;

  std::vector<qint64> ranking() const;
  Member operator[](qint64 id) const;

private:
  qint64 m_owner_id{0};
  QMap<qint64, Member> m_members{};
};
