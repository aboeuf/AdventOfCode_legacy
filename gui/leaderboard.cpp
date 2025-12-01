#include <QDateTime>
#include <gui/leaderboard.h>

CompletionStar::CompletionStar(const QJsonObject &object) {
  JsonHelper helper;
  if (not helper.read(object, "get_star_ts", get_star_ts)) {
    throw std::runtime_error(QString("Cannot parse field \"get_star_ts\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  }
  if (not helper.read(object, "star_index", star_index)) {
    has_start_index = false;
  }
}

CompletionDay::CompletionDay(const QJsonObject &object) {
  JsonHelper helper;
  QJsonObject first_object;
  if (helper.read(object, "1", first_object))
    first = CompletionStar(first_object);
  QJsonObject second_object;
  if (helper.read(object, "2", second_object))
    second = CompletionStar(second_object);
  if (second.has_value() and not first.has_value())
    throw std::runtime_error("invalid CompletionDay");
}

qint64 CompletionDay::getFirstTime() const {
  return first.has_value() ? first->get_star_ts : 0;
}

qint64 CompletionDay::getSecondTime() const {
  return second.has_value() ? second->get_star_ts : 0;
}

Member::Member(const QJsonObject &object) {
  JsonHelper helper;
  if (not helper.read(object, "name", name))
    throw std::runtime_error(QString("Cannot parse field \"name\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  if (not helper.read(object, "stars", stars))
    throw std::runtime_error(QString("Cannot parse field \"starts\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  if (not helper.read(object, "last_star_ts", last_star_ts))
    throw std::runtime_error(QString("Cannot parse field \"last_star_ts\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  if (not helper.read(object, "local_score", local_score))
    throw std::runtime_error(QString("Cannot parse field \"local_score\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  if (not helper.read(object, "global_score", global_score)) {
    global_score = 0;
  }
  QJsonObject completions;
  if (not helper.read(object, "completion_day_level", completions))
    throw std::runtime_error(
        QString("Cannot parse field \"completion_day_level\"\n%1")
            .arg(helper.error())
            .toStdString());
  for (auto completion = completions.begin(); completion != completions.end();
       ++completion) {
    bool ok;
    const auto day = completion.key().toUInt(&ok);
    if (not ok)
      throw std::runtime_error(QString("Cannot cast %1 to uint")
                                   .arg(completion.key())
                                   .toStdString());
    completion_day_level[day] = CompletionDay{completion.value().toObject()};
  }
}

Leaderboard::Leaderboard(const QString &filepath) {
  JsonHelper helper;
  QJsonObject parsed;
  if (not helper.read(filepath, parsed))
    throw std::runtime_error(helper.error().toStdString());
  if (not helper.read(parsed, "owner_id", m_owner_id))
    throw std::runtime_error(QString("Cannot parse field \"owner_id\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  QJsonObject members;
  if (not helper.read(parsed, "members", members))
    throw std::runtime_error(QString("Cannot parse field \"members\"\n%1")
                                 .arg(helper.error())
                                 .toStdString());
  for (auto member = members.begin(); member != members.end(); ++member) {
    bool ok;
    const auto id = member.key().toULongLong(&ok);
    if (not ok)
      throw std::runtime_error(
          QString("Cannot cast %1 to qint64").arg(member.key()).toStdString());
    m_members[id] = Member(member.value().toObject());
  }
}

qint64 Leaderboard::ownerID() const { return m_owner_id; }

QString Leaderboard::ownerName() const { return (*this)[m_owner_id].name; }

std::vector<qint64> Leaderboard::ranking() const {
  auto res = std::vector<qint64>{};
  res.reserve(m_members.size());
  for (auto it = m_members.begin(); it != m_members.end(); ++it)
    res.emplace_back(it.key());
  std::sort(std::begin(res), std::end(res), [this](qint64 lhs, qint64 rhs) {
    return (*this)[lhs].local_score > (*this)[rhs].local_score;
  });
  return res;
}

Member Leaderboard::operator[](qint64 id) const {
  if (not m_members.contains(id))
    return Member();
  return m_members[id];
}
