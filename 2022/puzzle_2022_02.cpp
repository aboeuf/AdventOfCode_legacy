#include <2022/puzzle_2022_02.h>
#include <common.h>

namespace puzzle_2022_02 {

using Int = long long unsigned int;

enum class HandShape
{
    ERROR = 0u,
    ROCK = 1u,
    PAPER = 2u,
    SCISSORS = 3u
};

enum class Outcome
{
    ERROR = 1u,
    LOOSE = 0u,
    DRAW = 3u,
    WIN = 6u
};

inline HandShape getHandShapeFromString(const QString& input)
{
    if (input == QString("A") or input == QString("X"))
        return HandShape::ROCK;
    if (input == QString("B") or input == QString("Y"))
        return HandShape::PAPER;
    if (input == QString("C") or input == QString("Z"))
        return HandShape::SCISSORS;
    return HandShape::ERROR;
}

inline Int getHandShapeValue(const HandShape& shape)
{
    return static_cast<Int>(shape);
}


inline Outcome getOutcomeFromString(const QString& input)
{

    if (input == QString("X"))
        return Outcome::LOOSE;
    if (input == QString("Y"))
        return Outcome::DRAW;
    if (input == QString("Z"))
        return Outcome::WIN;
    return Outcome::ERROR;
}

inline HandShape getHandShapeForOutcome(const HandShape& opponent_hand_shape, const Outcome& outcome)
{
    switch (opponent_hand_shape) {
    case HandShape::ERROR:
        return HandShape::ERROR;
    case HandShape::ROCK:
        switch (outcome) {
        case Outcome::ERROR:
            return HandShape::ERROR;
        case Outcome::LOOSE:
            return HandShape::SCISSORS;
        case Outcome::DRAW:
            return HandShape::ROCK;
        case Outcome::WIN:
            return HandShape::PAPER;
        };
    case HandShape::PAPER:
        switch (outcome) {
        case Outcome::ERROR:
            return HandShape::ERROR;
        case Outcome::LOOSE:
            return HandShape::ROCK;
        case Outcome::DRAW:
            return HandShape::PAPER;
        case Outcome::WIN:
            return HandShape::SCISSORS;
        };
    case HandShape::SCISSORS:
        switch (outcome) {
        case Outcome::ERROR:
            return HandShape::ERROR;
        case Outcome::LOOSE:
            return HandShape::PAPER;
        case Outcome::DRAW:
            return HandShape::SCISSORS;
        case Outcome::WIN:
            return HandShape::ROCK;
        };
    }
    return HandShape::ERROR;
}


class Round {

public:
    Round(const QString& input, bool first_strategy) {
        const auto values = common::splitValues(input, QChar(' '));
        if (values.size() != 2)
            return;
        m_opponent_hand_shape = getHandShapeFromString(values[0]);
        if (first_strategy)
            m_self_hand_shape = getHandShapeFromString(values[1]);
        else
            m_self_hand_shape = getHandShapeForOutcome(m_opponent_hand_shape,
                                                       getOutcomeFromString(values[1]));
    }

    HandShape opponentHandShape() const { return m_opponent_hand_shape; }
    HandShape selfHandShape() const { return m_self_hand_shape; }

    bool isValid() const
    {
        return m_opponent_hand_shape != HandShape::ERROR and m_self_hand_shape != HandShape::ERROR;
    }

    Outcome getOutcome() const
    {
        switch (m_opponent_hand_shape) {
        case HandShape::ERROR:
            return Outcome::ERROR;
        case HandShape::ROCK:
            switch (m_self_hand_shape) {
            case HandShape::ERROR:
                return Outcome::ERROR;
            case HandShape::ROCK:
                return Outcome::DRAW;
            case HandShape::PAPER:
                return Outcome::WIN;
            case HandShape::SCISSORS:
                return Outcome::LOOSE;
            };
        case HandShape::PAPER:
            switch (m_self_hand_shape) {
            case HandShape::ERROR:
                return Outcome::ERROR;
            case HandShape::ROCK:
                return Outcome::LOOSE;
            case HandShape::PAPER:
                return Outcome::DRAW;
            case HandShape::SCISSORS:
                return Outcome::WIN;
            };
        case HandShape::SCISSORS:
            switch (m_self_hand_shape) {
            case HandShape::ERROR:
                return Outcome::ERROR;
            case HandShape::ROCK:
                return Outcome::WIN;
            case HandShape::PAPER:
                return Outcome::LOOSE;
            case HandShape::SCISSORS:
                return Outcome::DRAW;
            };
        }
        return Outcome::ERROR;
    }

    Int getOutcomeValue() const
    {
        const auto outcome = getOutcome();
        if (outcome == Outcome::ERROR)
            return 0u;
        return static_cast<Int>(outcome);
    }

    Int getValue() const
    {
        return getHandShapeValue(m_self_hand_shape) + getOutcomeValue();
    }

private:
    bool m_valid{false};
    HandShape m_opponent_hand_shape{HandShape::ERROR};
    HandShape m_self_hand_shape{HandShape::ERROR};
    Int m_value{0};
};

class Game {

public:
    Game(const QString& input, bool first_strategy) {
        const auto lines = common::splitLines(input);
        m_rounds.reserve(std::size(lines));
        for (const auto& line : lines)
            m_rounds.emplace_back(line, first_strategy);
    }

    bool isValid() const
    {
        return std::all_of(std::begin(m_rounds), std::end(m_rounds), [](const auto& round)
        {
            return round.isValid();
        });
    }

    Int getValue() const
    {
        auto res = Int{0};
        for (const auto& round : m_rounds)
            res += round.getValue();
        return res;
    }

private:
    std::vector<Round> m_rounds;
};

}

void Solver_2022_02_1::solve(const QString& input)
{
    const auto game = puzzle_2022_02::Game{input, true};
    emit output(QString("Game is %1").arg(game.isValid() ? "valid" : "invalid"));
    emit finished(QString("%1").arg(game.getValue()));
}

void Solver_2022_02_2::solve(const QString& input)
{
    const auto game = puzzle_2022_02::Game{input, false};
    emit output(QString("Game is %1").arg(game.isValid() ? "valid" : "invalid"));
    emit finished(QString("%1").arg(game.getValue()));
}
